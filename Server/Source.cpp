#include "MyServer.h"
#include <iostream>
#include <chrono>

using namespace std::chrono;

int main()
{
	auto start = high_resolution_clock::now();

	if (Network::Initialize())
	{
		MyServer server;

		if (server.Initialize(IPEndpoint("0.0.0.0", 6112)))
		{
			while (true)
			{
				server.Update();
				server.Frame();
				float tim = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
				if (tim > 220)
				{
					start = high_resolution_clock::now();
					server.SendPackets();
				}
			}
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;
}