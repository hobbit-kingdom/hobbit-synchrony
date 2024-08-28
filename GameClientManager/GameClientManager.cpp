#include "GameClientManager.h"
ClientEnitiesList GameClientManager::clientEnitiesList;
std::vector<std::function<void()>> GameManager::listenersUpdate;

GameClientManager::GameClientManager() {
    

    for (ClientEntity* e : clientEnitiesList.clientEntities)
    {
        addListenerEnterNewLevel([e]() { e->enterNewLevel(); });
        addListenerExitLevel([e]() { e->exitLevel(); }); // assuming exitLevel is a method of ClientEntity
        addListenerUpdate([e]() { e->update(); }); // assuming update is a method of ClientEntity
        //addListenerOpenGame([e]() { e->openGame(); }); // assuming openGame is a method of ClientEntity
        //addListenerCloseGame([e]() { e->closeGame(); }); // assuming closeGame is a method of ClientEntity
    }
}