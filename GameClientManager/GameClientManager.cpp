#include "GameClientManager.h"
std::vector<ClientEntity*> GameClientManager::clientEntities;
std::vector<std::function<void()>> GameManager::listenersUpdate;

GameClientManager::GameClientManager() {
    clientEntities.push_back(new MainPlayer());
    clientEntities.push_back(new OtherPlayer());
    clientEntities.push_back(new LevelEntity());
    clientEntities.push_back(new PodnitiiPredmet());

    for (ClientEntity* e : clientEntities)
    {
        addListenerEnterNewLevel([e]() { e->enterNewLevel(); });
        addListenerExitLevel([e]() { e->exitLevel(); }); // assuming exitLevel is a method of ClientEntity
        addListenerUpdate([e]() { e->update(); }); // assuming update is a method of ClientEntity
        //addListenerOpenGame([e]() { e->openGame(); }); // assuming openGame is a method of ClientEntity
        //addListenerCloseGame([e]() { e->closeGame(); }); // assuming closeGame is a method of ClientEntity
    }
}