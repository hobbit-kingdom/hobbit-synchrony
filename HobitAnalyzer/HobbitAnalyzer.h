#include <iostream>
#include <vector>
#include <string>
#include "../GameManager/GameManager.h"

class HobbitAnalyzer {
public:
    GameManager gameManager;

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

    void displayObjectAddressBilboWatchingAtLoop() {
        std::thread inputThread([&]() {
            while (true) {
                std::cout << "Enter 'q' to go back to the main menu: ";
                char input;
                std::cin >> input;
                if (input == 'q') {
                    exitWatchingAtLoop = true;
                    break;
                }
            }
            });

        while (!exitWatchingAtLoop) {
            uint32_t activatedAddress = HobbitMemoryAccess::memoryAccess.readData(0x00773BD0);
            uint32_t foundObject1 = HobbitMemoryAccess::findObjectAddressByGUID(activatedAddress);
            
            // make it pickup
            // HobbitMemoryAccess::memoryAccess.writeData(foundObject1 + 0x10, 0x02001122);

            std::cout << std::hex;
            std::cout << "The interactive thing Address1:" << foundObject1 << std::endl;
            std::cout << std::dec;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        inputThread.join();
        exitWatchingAtLoop = false;
    }
};

int main() {
    HobbitAnalyzer analyzer;
    analyzer.run();
    return 0;
}
