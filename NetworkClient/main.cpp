#include <iostream>
#include "Client.h"


int main()
{
	Client client;
	char a;
	do
	{
		std::cout << "Enter [q] to close program: ";
		std::cin >> a;
		std::cout << std::endl;
	} while (a != 'q');
	return 0;
}
