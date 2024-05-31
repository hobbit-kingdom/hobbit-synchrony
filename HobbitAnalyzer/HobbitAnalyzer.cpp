
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
void threadFunction(std::atomic<bool>& stopFlag) {


    while (!stopFlag.load()) {
        uint32_t activatedAddress = HobbitMemoryAccess::memoryAccess.readData(0x00773BD0);

        uint32_t foundObject = HobbitMemoryAccess::findObjectAddressByGUID(activatedAddress);

        std::cout << std::hex;
        std::cout << "The interactive thing Address1:" << foundObject << std::endl;
        HobbitMemoryAccess::memoryAccess.writeData(foundObject + 0x10, 0x02001122);
        std::cout << std::dec;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Thread is stopping...\n";
}


int main()
{
    GameManager gameManager;
    HobbitMemoryAccess::setHobbitMemoryAccess();

    // Adding the listener to the event
    bool wasHobbitOpen = false;
    bool isHobbitOpen = false;
    do
    {        
        std::atomic<bool> stopFlag(false);
        std::thread t(threadFunction, std::ref(stopFlag));

        std::string input;
        std::cout << "Enter 'q' to quit: ";
        while (std::cin >> input) {
            if (input == "q") {
                stopFlag.store(true);
                break;
            }
            std::cout << "Invalid input. Enter 'q' to quit: ";
        }

        t.join();
        std::cout << "Main thread joined the worker thread.\n";
        


    } while (std::cin.get() != 'q');
    return 0;
}