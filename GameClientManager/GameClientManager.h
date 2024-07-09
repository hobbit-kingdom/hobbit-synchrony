#pragma once
#include "../GameManager/GameManager.h"    // Game management functionality


#include "GamePacket.h"
#include "ClientEntity.h"

#include "MainPlayer.h"
#include "OtherPlayer.h"
#include "LevelEntity.h"
#include "PodnitiiPredmet.h"

class GameClientManager : public GameManager
{
protected:
    static std::vector<ClientEntity*> clientEntities;
public:
    GameClientManager ();

    static void readPacket(std::vector<uint32_t>& packets, uint32_t playerIndex)
    {
        if (!HobbitMemoryAccess::isGameOpen())
            return;

        // convert packets into GamePackets
        std::vector<GamePacket> gamePackets;
        gamePackets = GamePacket::packetsToGamePackets(packets);

        // read the GamePackets
        for (GamePacket gamePacket : gamePackets)
        {
            for (uint32_t reader : gamePacket.getReadersIndexes())
            {
                clientEntities[reader]->readPacket(gamePacket, playerIndex);
            }
        }
    }
    static void writePacket(std::vector<uint32_t>& snapshotPackets, std::vector<uint32_t>& eventPackets)
    {
        std::vector<uint32_t> entityPackets;// entity packets
        std::vector<GamePacket> gamePackets;// packe of the game
        std::vector<uint32_t> processedGamePacket;

        if (!HobbitMemoryAccess::isGameOpen())
        {
            if (snapshotPackets.empty())
                return;

            //indicate the end of packet
            processedGamePacket = GamePacket::lastPacket();
            snapshotPackets.insert(snapshotPackets.end(), processedGamePacket.begin(), processedGamePacket.end());
            return;
        }

        // get game packet from all entities
        GamePacket pkt;
        std::vector<GamePacket> pkts;
        for (ClientEntity* e : clientEntities)
        {
            pkts = e->writePacket();
            for (GamePacket pkt : pkts)
            {
                if (pkt.getGameDataSize() != 0)
                    gamePackets.push_back(pkt);
            }
            e->finishedWritePacket();
        }

        // get packet from game packet
        for (GamePacket gamePacket : gamePackets)
        {
            processedGamePacket = gamePacket.getPacket();

            if (processedGamePacket.front() == (uint32_t)ReadType::Game_Snapshot)
            {
                snapshotPackets.insert(snapshotPackets.end(), processedGamePacket.begin() + 1, processedGamePacket.end());
            }
            else if (processedGamePacket.front() == (uint32_t)ReadType::Game_EventClient)
            {
                eventPackets.insert(eventPackets.end(), processedGamePacket.begin() + 1, processedGamePacket.end());
            }
        }

        //indicate the end of packet
        processedGamePacket = GamePacket::lastPacket();
        if (!snapshotPackets.empty())
            snapshotPackets.insert(snapshotPackets.end(), processedGamePacket.begin(), processedGamePacket.end());
        if (!eventPackets.empty())
            eventPackets.insert(eventPackets.end(), processedGamePacket.begin(), processedGamePacket.end());
    }
};