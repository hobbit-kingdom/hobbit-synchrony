
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "../GameManager/GameManager.h"
#include "../GameManager/HobbitMemoryAccess.h"
#include "../GameManager/HobbitProcessAnalyzer.h"
#include "../GameManager/ProcessAnalyzer.h"
#include "../GameManager/ProcessAnalyzerTypeWrapped.h"

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
uint32_t hexStringToUint32(const std::string& hexStr) {
    uint32_t address;
    std::stringstream ss;

    // Remove the "0x" prefix if present
    if (hexStr.substr(0, 2) == "0x") {
        ss << std::hex << hexStr.substr(2);
    }
    else {
        ss << std::hex << hexStr;
    }

    ss >> address;

    // Check for conversion errors
    if (ss.fail() || !ss.eof()) {
        throw std::invalid_argument("Invalid hexadecimal string.");
    }

    return address;
}

int main() {
    std::string hexAddress;
    ProcessAnalyzerTypeWrapped patw;
    std::vector<uint32_t> NPC = patw.searchProcessMemory(patw.getProcess("Meridian.exe"), 0x3101001C);
    for (uint32_t e : NPC) {
        std::cout << std::hex << e-29*4 << std::endl;
    }
   /* try {
        uint32_t address = hexStringToUint32(hexAddress);

        // Ensure the address fits in a uint32_t (0 to 2^32 - 1)
        if (address < 0 || address > 0xFFFFFFFF) {
            throw std::out_of_range("Address must be in the range of a uint32_t (0 to 4294967295).");
        }

        // Call the Process function with the address
    }
    catch (const std::exception& e) {
        std::cerr << "Invalid input: " << e.what() << std::endl;
    }*/
}