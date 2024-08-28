#pragma once
#include <cstdint>

#include "MainPlayer.h"
#include "OtherPlayer.h"
#include "LevelEntity.h"
#include "PodnitiiPredmet.h"



class ClientEnitiesList
{
public:
    std::vector<ClientEntity*> clientEntities;
    ClientEnitiesList() {
        clientEntities.push_back(new MainPlayer());
        clientEntities.push_back(new OtherPlayer());
        clientEntities.push_back(new LevelEntity());
        clientEntities.push_back(new PodnitiiPredmet());       
    }
};