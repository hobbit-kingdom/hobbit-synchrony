#include <iostream>
#include <vector>
#include <string>
#include "../GameManager/GameManager.h"
#include "../GameManager/HobbitMemoryAccess.h"
#include "../GameManager/HobbitProcessAnalyzer.h"
#include "../GameManager/ProcessAnalyzer.h"
#include "../GameManager/ProcessAnalyzerTypeWrapped.h"

    HobbitAnalyzer() {
        HobbitMemoryAccess::setHobbitMemoryAccess();
    }

    void run() {
        while (true) {
            std::cout << "Hobbit Analyzer Menu:" << std::endl;
            std::cout << "1. Find by pattern from array" << std::endl;
            std::cout << "2. Find by pattern from memory" << std::endl;
            std::cout << "3. Change all data that is found previously to another value" << std::endl;
            std::cout << "4. Display object address Bilbo is watching at" << std::endl;
            std::cout << "5. Stop execution" << std::endl;
            int choice;
            std::cout << "Enter your choice: ";
            std::cin >> choice;

            switch (choice) {
            case 1:
                findByPatternFromArray();
                break;
            case 2:
                findByPatternFromMemory();
                break;
            case 3:
                changeFoundData();
                break;
            case 4:
                displayObjectAddressBilboWatchingAtLoop();
                break;
            case 5:
                return;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
            }
        }
    }
private:
    std::vector<uint32_t> foundObjects;
    std::atomic<bool> exitWatchingAtLoop = false;


    void findByPatternFromArray() {
        std::string patternStr;
        std::cout << "Enter the pattern to search for: ";
        std::cin >> patternStr;
        uint32_t shift;
        std::cout << "Enter the shift value: ";
        std::cin >> shift;
        foundObjects = HobbitMemoryAccess::findObjectsByData(patternStr, shift);
        std::cout << "Found " << std::hex << foundObjects.size() << " objects." << std::endl;
        for (auto obj : foundObjects) {
            std::cout << "Address: " << obj << std::endl;
        }
    }


    void findByPatternFromMemory() {
        std::string patternStr;
        std::cout << "Enter the pattern to search for: ";
        std::cin >> patternStr;
        uint32_t shift;
        std::cout << "Enter the shift value: ";
        std::cin >> shift;
        foundObjects = HobbitMemoryAccess::findObjectsByData(patternStr, shift);
        std::cout << "Found " << foundObjects.size() << " objects." << std::endl;
        for (auto obj : foundObjects) {
            std::cout << "Address: " << obj << std::endl;
        }
    }

    void changeFoundData() {
        if (foundObjects.empty()) {
            std::cout << "No objects found previously. Please search for objects first." << std::endl;
            return;
        }
        std::string newDataStr;
        std::cout << "Enter the new data value (in hex): ";
        std::cin >> newDataStr;
        uint32_t shift;
        std::cout << "Enter the shift value: ";
        std::cin >> shift;
        uint32_t newData = stoul(newDataStr, 0, 16); // convert hex string to uint32_t
        for (uint32_t address : foundObjects) {
            HobbitMemoryAccess::memoryAccess.writeData(address + shift, newData);
        }
        std::cout << "Data changed successfully." << std::endl;
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