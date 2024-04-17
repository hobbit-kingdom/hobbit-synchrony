#include "MyClient.h"
#include <iostream>
#include <chrono>
#include <fstream>


using namespace std::chrono;
using namespace std;

int main()
{
	const char* serverIp;
	cout << "The Client is starting...\n";

	string s;

	ifstream file("server_ip.txt");
	string line;

	if (file.is_open())
	{
		if (std::getline(file, line)) serverIp = line.c_str();
		else
		{
			cerr << "Ip is not found in server_ip.txt.\n";
			return 0;
		}
		file.close();
	}
	else
	{
		cerr << "Unable to open file server_ip.txt.\n";
		return 0;
	}

	cout << "Server IP is " << serverIp << ". Port is 6112. Trying to connect..\n";

	auto start = high_resolution_clock::now();
	if (Network::Initialize())
	{
		MyClient client;

		cout << "Input y and press ENTER after you've launched the level\n";
		cin >> s;

		client.FindHobbits();
		if (client.Connect(IPEndpoint(serverIp, 6112)))
		{
			while (client.IsConnected())
			{
				float tim = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
				client.Frame();
				if (tim > 300)
				{
					start = high_resolution_clock::now();
					client.SendPacket();

				}
			}
		}
	}
	Network::Shutdown();
	system("pause");
	return 0;
}