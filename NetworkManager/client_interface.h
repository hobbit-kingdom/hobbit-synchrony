#pragma once

#include "net_common.h"
#include "tsqueue.h"
#include "connection.h"
#include "PacketType.h"
#include "server_interface.h"

namespace net
{
	template<typename T>
	class client_interface
	{
	public:
		client_interface() {}
		virtual ~client_interface()
		{
			Disconnect();
		}

		bool Connect(const std::string& host, const uint16_t port)
		{
			try
			{
				asio::ip::tcp::resolver resolver(m_context);
				asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));
				m_connection = std::make_shared<connection<T>>(connection<T>::owner::client, m_context, asio::ip::tcp::socket(m_context), m_qMessagesIn);
				m_connection->ConnectToServer(endpoints);
				m_threadContext = std::thread([this]() { m_context.run(); });
			}
			catch (std::exception& e)
			{
				std::cerr << "Client Exception: " << e.what() << "\n";
				return false;
			}

			return true;
		}

		void Disconnect()
		{
			if (IsConnected())
			{
				m_connection->Disconnect();
			}

			m_context.stop();
			if (m_threadContext.joinable()) m_threadContext.join();
			m_connection.reset();
		}

		bool IsConnected()
		{
			if (m_connection)
				return m_connection->IsConnected();
			else
				return false;
		}

		void Send(const packet<T>& msg)
		{
			if (IsConnected())
				m_connection->Send(msg);
		}

		tsqueue<owned_packet<T>>& Incoming()
		{
			return m_qMessagesIn;
		}
		
		virtual void OnClientValidated(std::shared_ptr<connection<T>> client)
		{

		}
	protected:
		asio::io_context m_context;
		std::thread m_threadContext;
		std::shared_ptr<connection<T>> m_connection;
		tsqueue<owned_packet<T>> m_qMessagesIn;
	};
}
