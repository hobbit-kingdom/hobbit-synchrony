
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "../GameManager/GameManager.h"
#include "../GameManager/HobbitMemoryAccess.h"


#include <iostream>
#include <thread>
#include <atomic>

HobbitMemoryAccess hobbitMemoryAccess;

bool displayType = 0;
uint32_t continousSpeed = 0;

void displayObjectsAddressByData()
{

    std::string dataStr;
    uint32_t shift;
    std::cout << "[b] Go Back to Menu " << std::endl;
    std::cout << "Enter data: ";
    std::cin >> dataStr;
    std::cout << "Enter shift: ";
    std::cin >> shift;

    std::vector<uint32_t> foundObject = HobbitMemoryAccess::findObjectsByData(dataStr, shift);

    std::cout << std::hex;
    for (uint32_t e : foundObject)
    {
        std::cout << "The interactive thing Address1:" << e << std::endl;
        // HobbitMemoryAccess::memoryAccess.writeData(e + 0x10, 0x02001122);

    }
    std::cout << std::dec;
}
void displayObjectAddressBibloWatchingAt()
{
    //00773BD0
    //uint32_t activatedAddress = HobbitMemoryAccess::memoryAccess.readData(0x0075EC20);
    uint32_t activatedAddress = HobbitMemoryAccess::memoryAccess.readData(0x00773BD0);

    uint32_t foundObject1 = HobbitMemoryAccess::findObjectAddressByGUID(activatedAddress);

    std::cout << std::hex;
    std::cout << "The interactive thing Address1:" << foundObject1 << std::endl;
    //HobbitMemoryAccess::memoryAccess.writeData(foundObject1 + 0x10, 0x02001122);
    std::cout << std::dec;

    std::this_thread::sleep_for(std::chrono::seconds(1));
}
void displayStackAddress()
{
    std::cout << "StackAdrres:" << HobbitMemoryAccess::getObjectStackAddress() << std::endl;
}

void displayMenuSettings()
{
    std::cout << std::boolalpha;
    char input;
    do
    {
        std::cin >> input;
        std::cout << std::endl << "SETTINGS" << std::endl;
        std::cout << "[1] Continuous " << displayType << std::endl;
        std::cout << "[2] Set Continuous Freequency" << continousSpeed << std::endl;
        std::cout << "[a] Apply Changes " << std::endl;

        switch (input)
        {
        case '1':
            std::cout << "Changed Continous from " << displayType << " to " << !displayType << std::endl;
            displayType = !displayType;
            break;
        case '2':
            std::cout << "Chaning Frequency" << std::endl;
            std::cout << "Current Frequency:" << continousSpeed << std::endl;
            std::cout << "Enter New Frequency: ";
            std::cin >> input;
            break;
        }
    }while(input != 'a');

}


int main()
{
    GameManager gameManager;
    char input = 0;
    do
    {

        std::cout << std::endl << "MAIN MENU" << std::endl;
        std::cout << "[1] Object address by data" << std::endl;
        std::cout << "[2] Display What Bilbo is looking at" << std::endl;
        std::cout << "[3] DisplayData" << std::endl;
        std::cout << "[4] Settings" << std::endl;
        std::cout << "[q] Close Program" << std::endl;
        std::cin >> input;
        switch (input)
        {
        case '1':
            displayObjectsAddressByData();
            break;
        case '2':
            displayObjectAddressBibloWatchingAt();
            break;
        case '3':
            displayStackAddress();
            break;
        case 's':
            displayMenuSettings();
            break;
        case 'q':
            std::cout << "Closing program" << std::endl;
            break;
        }

    } while (input != 'q');

    

    return 0;
}
