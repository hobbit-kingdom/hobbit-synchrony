#include "TCPConnection.h"

namespace PNet
{
	void TCPConnection::Close()
	{
		socket.Close();
	}

	std::string TCPConnection::ToString()
	{
		return stringRepresentation;
	}
}
