#pragma once
#include <vector>


class GamePacket
{
	uint32_t packetReadType = 0;
	uint32_t packetType = 0;
	std::vector<uint32_t> readersIndexes;
	std::vector<uint32_t> gameData;
	bool isLastPacket = false;
public:
	static const uint32_t PACKET_FlAG = 0xffffffff;

	GamePacket(uint32_t newRecieverType, std::vector<uint32_t> recieverIndex, std::vector<uint32_t> packet, uint32_t newPacketType) 
		: packetReadType(newRecieverType), readersIndexes(recieverIndex), gameData(packet), packetType(newPacketType){}
	GamePacket(uint32_t newRecieverType, uint32_t recieverIndex, std::vector<uint32_t> packet) : packetReadType(newRecieverType)
	{
		readersIndexes.push_back(recieverIndex);

		// find packet type
		packetType = packet.front();
		packet.erase(packet.begin());

		// packet size 
		uint32_t packetSize = packet.front();
		packet.erase(packet.begin());

		// store Game Data
		gameData.insert(gameData.end(), packet.begin(), packet.begin() + packetSize);
	}
	GamePacket(uint32_t newRecieverType, uint32_t recieverIndex, uint32_t newPacketType) : packetReadType(newRecieverType), packetType(newPacketType)
	{
		readersIndexes.push_back(recieverIndex);
	}

	GamePacket(){}

	
	void pushBackReader(uint32_t readerIndex)
	{
		readersIndexes.push_back(readerIndex);
	}
	void pushBackGamePacket(uint32_t gamePacket)
	{
		gameData.push_back(gamePacket);
	}

	uint32_t getPacketType()
	{
		return packetType;
	}
	// reader's indexes
	std::vector<uint32_t> getReadersIndexes()
	{
		return readersIndexes;
	}
	// game data packets
	std::vector<uint32_t> getGameData()
	{
		return gameData;
	}

	//number of readers
	uint32_t getReadersIndexesSize()
	{
		return readersIndexes.size();
	}
	//number of game packet
	uint32_t getGameDataSize()
	{
		return gameData.size();
	}
	
	
	// size of the whole packet
	// includes type, size, and flags when needed
	uint32_t getPacketSize()
	{
		uint32_t size = readersIndexes.size();
		// include flags
		if (readersIndexes.size() > 1)
			size += 2;
		// packetType
		++size;
		// packetSize
		++size;
		//game data
		size += gameData.size();

		return size;
	}

	// final packet (ready to send)
	std::vector<uint32_t> getPacket()
	{
		std::vector<uint32_t> finalPacket;

		finalPacket.push_back(packetReadType);
		if (readersIndexes.size() == 0)
		{
			return finalPacket;
		}


		// set recievers indexes
		if (readersIndexes.size() > 1)
			finalPacket.push_back(PACKET_FlAG);
		
		finalPacket.insert(finalPacket.end(), readersIndexes.begin(), readersIndexes.end());

		if (readersIndexes.size() > 1)
			finalPacket.push_back(PACKET_FlAG);

		// set type
		finalPacket.push_back(packetType);

		// set packet size
		finalPacket.push_back(getGameDataSize());

		// set the data
		finalPacket.insert(finalPacket.end(), gameData.begin(), gameData.end());

		//check if the endPacket
		if (isLastPacket)
		{
			finalPacket.push_back(PACKET_FlAG);
			finalPacket.push_back(PACKET_FlAG);
		}


		return finalPacket;
	}

	// mark the end of packet (no more game packet after that)
	static std::vector<uint32_t> lastPacket()
	{
		return std::vector<uint32_t>(2, PACKET_FlAG);
	}

	static std::vector<GamePacket> packetsToGamePackets(std::vector<uint32_t>& packets)
	{
		std::vector<GamePacket> gamePackets;

		// flags that are used for 
		uint32_t currentFlag = 0;
		uint32_t previousFlag = 0;

		// type and size of game packets
		uint32_t packetType = 0;
		uint32_t packetSize = 0;

		// read packages loop
		std::vector<uint32_t> storePackets;
		while (currentFlag != PACKET_FlAG && previousFlag != PACKET_FlAG)
		{
			previousFlag = currentFlag;
			currentFlag = packets.front();

			// set current flag
			if (currentFlag == PACKET_FlAG)
			{
				packets.erase(packets.begin());
				continue;
			}


			// multiple readers from packet
			if (previousFlag == PACKET_FlAG && currentFlag != PACKET_FlAG)
			{
				storePackets = packets;
				// find all readers indexes
				std::vector<uint32_t> readers;
				while (currentFlag != PACKET_FlAG)
				{
					readers.push_back(packets.front());
					packets.erase(packets.begin());
					currentFlag = packets.front();
				}
				// remove the end flag from packet
				packets.erase(packets.begin());


				// store the GamePacket
				gamePackets.push_back(GamePacket(PACKET_FlAG, readers, packets, packetType));

				// the type and size of packet
				packets.erase(packets.begin());
				packets.erase(packets.begin());

				// remove the processed packets
				packets.erase(packets.begin(), packets.begin() + packetSize);

				// set current flag after the reading
				if (currentFlag == PACKET_FlAG)
				{
					packets.erase(packets.begin());
				}

				currentFlag = packets.front();
				continue;
			}

			// one reader if packet
			uint32_t reader = packets.front();
			packets.erase(packets.begin());
			// store the GamePacket
			gamePackets.push_back(GamePacket(PACKET_FlAG, reader, packets));
			// the type 
			packets.erase(packets.begin());
			// packet size
			packetSize = packets.front();
			packets.erase(packets.begin());
			
			// remove the processed packets
			packets.erase(packets.begin(), packets.begin() + packetSize);
		}
		return gamePackets;
	}
};

