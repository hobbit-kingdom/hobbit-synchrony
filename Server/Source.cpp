#include "MyServer.h"
#include <iostream>
#include <chrono>

using namespace std::chrono;

int main()
{
	auto start = high_resolution_clock::now();
	string s;
	cout << "Server is starting. Port is 6112.\n";

	if (Network::Initialize())
	{
		MyServer server;

		cout << "Input y and press ENTER after you've launched the level\n";
		cin >> s;
		server.FindHobbits();

		if (server.Initialize(IPEndpoint("0.0.0.0", 6112)))
		{
			while (true)
			{
				server.Frame();
				float tim = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
				if (tim > 300)
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