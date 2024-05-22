#include "Client.h"
#include <iostream>

namespace PNet
{
	bool Client::Connect(IPEndpoint ip)
	{
		isConnected = false;
		Socket socket = Socket(ip.GetIPVersion());
		if (socket.Create() == PResult::P_Success)
		{
			if (socket.SetBlocking(true) != PResult::P_Success)
				return false;


			std::cout << "Socket successfully created." << std::endl;
			if (socket.Connect(ip) == PResult::P_Success)
			{
				if (socket.SetBlocking(false) == PResult::P_Success)
				{
					connection = TCPConnection(socket, ip);
					master_fd.fd = connection.socket.GetHandle();
					master_fd.events = POLLRDNORM;
					master_fd.revents = 0;
					isConnected = true;
					OnConnect();
					return true;
				}
			}
			else
			{
			}
			socket.Close();
		}
		else
		{
			std::cerr << "Socket failed to create." << std::endl;
		}
		OnConnectFail();
		return false;
	}

	bool Client::IsConnected()
	{
		return isConnected;
	}

	bool Client::Frame()
	{
		if (connection.pm_outgoing.HasPendingPackets())
		{
			master_fd.events = POLLRDNORM | POLLWRNORM;
		}

		use_fd = master_fd;

		if (WSAPoll(&use_fd, 1, 1) > 0)
		{

			if (use_fd.revents & POLLERR) //If error occurred on this socket
			{
				CloseConnection("POLLERR");
				return false;
			}

			if (use_fd.revents & POLLHUP) //If poll hangup occurred on this socket
			{
				CloseConnection("POLLHUP");
				return false;
			}

			if (use_fd.revents & POLLNVAL) //If invalid socket
			{
				CloseConnection("POLLNVAL");
				return false;
			}

			if (use_fd.revents & POLLRDNORM) //If normal data can be read without blocking
			{
				int bytesReceived = 0;

				if (connection.pm_incoming.currentTask == PacketManagerTask::ProcessPacketSize)
				{
					bytesReceived = recv(use_fd.fd, (char*)&connection.pm_incoming.currentPacketSize + connection.pm_incoming.currentPacketExtractionOffset, sizeof(uint16_t) - connection.pm_incoming.currentPacketExtractionOffset, 0);
				}
				else //Process Packet Contents
				{
					bytesReceived = recv(use_fd.fd, (char*)&connection.buffer + connection.pm_incoming.currentPacketExtractionOffset, connection.pm_incoming.currentPacketSize - connection.pm_incoming.currentPacketExtractionOffset, 0);
				}


				if (bytesReceived == 0) //If connection was lost
				{
					CloseConnection("Recv==0");
					return false;
				}

				if (bytesReceived == SOCKET_ERROR) //If error occurred on socket
				{
					int error = WSAGetLastError();
					if (error != WSAEWOULDBLOCK)
					{
						CloseConnection("Recv<0");
						return false;
					}
				}

				if (bytesReceived > 0)
				{
					connection.pm_incoming.currentPacketExtractionOffset += bytesReceived;
					if (connection.pm_incoming.currentTask == PacketManagerTask::ProcessPacketSize)
					{
						if (connection.pm_incoming.currentPacketExtractionOffset == sizeof(uint16_t))
						{
							connection.pm_incoming.currentPacketSize = ntohs(connection.pm_incoming.currentPacketSize);
							if (connection.pm_incoming.currentPacketSize > PNet::g_MaxPacketSize)
							{
								CloseConnection("Packet size too large.");
								return false;
							}
							connection.pm_incoming.currentPacketExtractionOffset = 0;
							connection.pm_incoming.currentTask = PacketManagerTask::ProcessPacketContents;
						}
					}
					else //Processing packet contents
					{
						if (connection.pm_incoming.currentPacketExtractionOffset == connection.pm_incoming.currentPacketSize)
						{
							std::shared_ptr<Packet> packet = std::make_shared<Packet>();
							packet->buffer.resize(connection.pm_incoming.currentPacketSize + 2);
							memcpy(&packet->buffer[2], connection.buffer, connection.pm_incoming.currentPacketSize);

							connection.pm_incoming.Append(packet);

							connection.pm_incoming.currentPacketSize = 0;
							connection.pm_incoming.currentPacketExtractionOffset = 0;
							connection.pm_incoming.currentTask = PacketManagerTask::ProcessPacketSize;
						}
					}
				}
			}

			if (use_fd.revents & POLLWRNORM) //If normal data can be written without blocking
			{
				PacketManager& pm = connection.pm_outgoing;
				while (pm.HasPendingPackets())
				{
					PacketManager& pm = connection.pm_outgoing;
					while (pm.HasPendingPackets())
					{
						char* bufferPtr = &pm.Retrieve()->buffer[0];
						pm.currentPacketSize = pm.Retrieve()->buffer.size();
						int bytesSent = send(use_fd.fd, (char*)(bufferPtr)+pm.currentPacketExtractionOffset, pm.currentPacketSize - pm.currentPacketExtractionOffset, 0);
						if (bytesSent > 0)
						{
							pm.currentPacketExtractionOffset += bytesSent;
						}

						if (pm.currentPacketExtractionOffset == pm.Retrieve()->buffer.size()) //If full packet size was sent
						{
							pm.currentPacketExtractionOffset = 0;
							pm.Pop();
						}
						else //If full packet was not sent, break out of the loop for sending outgoing packets for this connection - we'll have to try again next time we are able to write normal data without blocking
						{
							break;
						}
					}
				}
				if (!connection.pm_outgoing.HasPendingPackets())
				{
					master_fd.events = POLLRDNORM;
				}
			}
		}

		// Loop while there are pending packets in the incoming packet manager
		while (connection.pm_incoming.HasPendingPackets())
		{
			// Retrieve the front packet from the incoming packet manager
			std::shared_ptr<Packet> frontPacket = connection.pm_incoming.Retrieve();
			// Process the retrieved packet, close connection if processing fails
			if (!ProcessPacket(frontPacket))
			{
				CloseConnection("Failed to process incoming packet.");
				return false;
			}
			// Remove the front packet from the packet manager
			connection.pm_incoming.Pop();
		}

		// Loop while there are pending packets in the incoming packet manager
		while (connection.pm_incoming.HasPendingPackets())
		{
			// Retrieve the front packet from the incoming packet manager
			std::shared_ptr<Packet> frontPacket = connection.pm_incoming.Retrieve();
			// Process the retrieved packet, close connection if processing fails
			if (!ProcessPacket(frontPacket))
			{
				CloseConnection("Failed to process incoming packet.");
				return false;
			}
			// Clear all packets from the packet manager
			connection.pm_incoming.Clear();
		}

		return false;
	}


	bool Client::FrameOnePacket()
	{

			if (connection.pm_outgoing.HasPendingPackets())
			{
				master_fd.events = POLLRDNORM | POLLWRNORM;
			}

			use_fd = master_fd;

			// read the buffer
			while (true)
			{
				if (WSAPoll(&use_fd, 1, 1) > 0)
				{
					if (use_fd.revents & POLLERR)
					{
						CloseConnection("POLLERR");
						return false;
					}

					else if (use_fd.revents & POLLHUP)
					{
						CloseConnection("POLLHUP");
						return false;
					}

					else if (use_fd.revents & POLLNVAL)
					{
						CloseConnection("POLLNVAL");
						return false;
					}

					else if (use_fd.revents & POLLRDNORM)
					{
						int bytesReceived = 0;

						if (connection.pm_incoming.currentTask == PacketManagerTask::ProcessPacketSize)
						{
							bytesReceived = recv(use_fd.fd, (char*)&connection.pm_incoming.currentPacketSize + connection.pm_incoming.currentPacketExtractionOffset, sizeof(uint16_t) - connection.pm_incoming.currentPacketExtractionOffset, 0);
						}
						else
						{
							bytesReceived = recv(use_fd.fd, (char*)&connection.buffer + connection.pm_incoming.currentPacketExtractionOffset, connection.pm_incoming.currentPacketSize - connection.pm_incoming.currentPacketExtractionOffset, 0);
						}

						if (bytesReceived == 0)
						{
							CloseConnection("Recv==0");
						}

						if (bytesReceived == SOCKET_ERROR)
						{
							int error = WSAGetLastError();
							if (error != WSAEWOULDBLOCK)
							{
								CloseConnection("Recv<0");
								return false;
							}
						}

						if (bytesReceived > 0) // If data was received successfully
						{
							connection.pm_incoming.currentPacketExtractionOffset += bytesReceived; // Update the extraction offset

							if (connection.pm_incoming.currentTask == PacketManagerTask::ProcessPacketSize)
							{
								// If the current task is to process the packet size
								if (connection.pm_incoming.currentPacketExtractionOffset == sizeof(uint16_t))
								{
									// If we have received enough bytes to determine the packet size
									connection.pm_incoming.currentPacketSize = ntohs(connection.pm_incoming.currentPacketSize); // Convert packet size from network to host byte order

									if (connection.pm_incoming.currentPacketSize > PNet::g_MaxPacketSize)
									{
										// If the packet size exceeds the maximum allowed size
										CloseConnection("Packet size too large."); // Close the connection due to large packet size
										return false; // Return false to indicate an error
									}

									connection.pm_incoming.currentPacketExtractionOffset = 0; // Reset the extraction offset
									connection.pm_incoming.currentTask = PacketManagerTask::ProcessPacketContents; // Move to the next task: processing packet contents
								}
							}
							else
							{
								// If the current task is to process the packet contents
								if (connection.pm_incoming.currentPacketExtractionOffset == connection.pm_incoming.currentPacketSize)
								{
									// If we have received the entire packet
									std::shared_ptr<Packet> packet = std::make_shared<Packet>(); // Create a new packet
									packet->buffer.resize(connection.pm_incoming.currentPacketSize + 2); // Resize the packet buffer to accommodate the packet size and header
									memcpy(&packet->buffer[2], connection.buffer, connection.pm_incoming.currentPacketSize); // Copy the received packet data into the packet buffer

									connection.pm_incoming.Append(packet); // Append the packet to the incoming packet manager

									connection.pm_incoming.currentPacketSize = 0; // Reset the current packet size
									connection.pm_incoming.currentPacketExtractionOffset = 0; // Reset the extraction offset
									connection.pm_incoming.currentTask = PacketManagerTask::ProcessPacketSize; // Move back to the task of processing packet size for the next packet
								}
							}
						}
					}

					else if (use_fd.revents & POLLWRNORM)
					{
						PacketManager& pm = connection.pm_outgoing;
						while (pm.HasPendingPackets())
						{
							char* bufferPtr = &pm.Retrieve()->buffer[0];
							pm.currentPacketSize = pm.Retrieve()->buffer.size();
							int bytesSent = send(use_fd.fd, (char*)(bufferPtr)+pm.currentPacketExtractionOffset, pm.currentPacketSize - pm.currentPacketExtractionOffset, 0);
							if (bytesSent > 0)
							{
								pm.currentPacketExtractionOffset += bytesSent;
							}

							if (pm.currentPacketExtractionOffset == pm.Retrieve()->buffer.size())
							{
								pm.currentPacketExtractionOffset = 0;
								pm.Pop();
							}
							else
							{
								break;
							}
						}
						if (!connection.pm_outgoing.HasPendingPackets())
						{
							master_fd.events = POLLRDNORM;
							break;
						}
					}
					else break;
				}
				else break;
			}

			// Process all pending incoming packets
			while (true)
			{
				if (!connection.pm_incoming.HasPendingPackets())
				{
					connection.pm_incoming.Clear();
					break;
				}
				std::shared_ptr<Packet> packet = connection.pm_incoming.Retrieve();
				packets.push_back(packet);
				connection.pm_incoming.Pop(); // Remove the packet from the queue

				if (!ProcessPacket(packet))
				{
					CloseConnection("Failed to process incoming packet.");
					return false;
				}
			}		
    return true;
}


	bool Client::ProcessPacket(std::shared_ptr<Packet> packet)
	{
		std::cout << "Packet received with size: " << packet->buffer.size() << std::endl;
		return true;
	}

	void Client::OnConnect()
	{
		std::cout << "Successfully connected!" << std::endl;
	}

	void Client::OnConnectFail()
	{
		std::cout << "Failed to connect." << std::endl;
	}

	void Client::OnDisconnect(std::string reason)
	{
		std::cout << "Lost connection. Reason: " << reason << "." << std::endl;
	}

	void Client::CloseConnection(std::string reason)
	{
		OnDisconnect(reason);
		master_fd.fd = 0;
		isConnected = false;
		connection.Close();
	}

	void Client::SendPacket()
	{
		return;
	}
}