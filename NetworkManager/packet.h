#pragma once

#include <vector>
#include <cstdint>
#include <iostream>
#include <memory>
#include <cstring>

namespace net
{
	template <typename T>
	class connection;

	template <typename T>
	struct packet_header
	{
		T id{};
		uint32_t size = 0;
	};

	template <typename T>
	struct packet
	{
		packet_header<T> header{};
		std::vector<uint8_t> body;

		size_t size() const
		{
			return body.size();
		}

		friend std::ostream& operator<<(std::ostream& os, const packet<T>& pkt)
		{
			os << "ID:" << int(pkt.header.id) << " Size:" << pkt.header.size;
			return os;
		}

		template<typename DataType>
		friend packet<T>& operator<<(packet<T>& pkt, const DataType& data)
		{
			static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pushed into vector");
			size_t i = pkt.body.size();
			pkt.body.resize(pkt.body.size() + sizeof(DataType));
			std::memcpy(pkt.body.data() + i, &data, sizeof(DataType));
			pkt.header.size = pkt.size();
			return pkt;
		}

		template<typename DataType>
		friend packet<T>& operator>>(packet<T>& pkt, DataType& data)
		{
			static_assert(std::is_standard_layout<DataType>::value, "Data is too complex to be pulled from vector");
			size_t i = pkt.body.size() - sizeof(DataType);
			std::memcpy(&data, pkt.body.data() + i, sizeof(DataType));
			pkt.body.resize(i);
			pkt.header.size = pkt.size();
			return pkt;
		}
	};

	template <typename T>
	struct owned_packet
	{
		std::shared_ptr<connection<T>> remote = nullptr;
		packet<T> pkt;

		friend std::ostream& operator<<(std::ostream& os, const owned_packet<T>& pkt)
		{
			os << pkt.pkt;
			return os;
		}
	};
}
