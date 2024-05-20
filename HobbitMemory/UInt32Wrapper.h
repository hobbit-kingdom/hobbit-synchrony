#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
class UInt32Wrapper {
public:
	// Constructor
	 // Default constructor
	UInt32Wrapper() : data(0) {}

	// Constructor taking any type and storing it in data
	template<typename TYPE>
	explicit UInt32Wrapper(TYPE value) : data(static_cast<uint32_t>(value)) {}

	// Templated function for reinterpretation of byte representation as other types
	template<typename T>
	static T reinterpret_as(const uint32_t& data) {
		static_assert(sizeof(T) == sizeof(uint32_t), "Size mismatch between types");
		T result;
		std::memcpy(&result, &data, sizeof(T));
		return result;
	}

	// Templated conversion operator calling the reinterpretation function
	template<typename T>
	operator T() const {
		return reinterpret_as<T>(data);
	}

	// Helper function for arithmetic operations
	template<typename T, typename Func>
	UInt32Wrapper performArithmeticOperation(T rhs, Func operation) const {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		T result = operation(dataAsT, rhs);
		uint32_t resultAsUint32;
		std::memcpy(&resultAsUint32, &result, sizeof(uint32_t));
		return UInt32Wrapper(resultAsUint32);
	}

	// Operator overloads for arithmetic operations
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt32Wrapper operator+(T rhs) const {
		return performArithmeticOperation(rhs, [](T x, T y) { return x += y; });
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt32Wrapper operator-(T rhs) const {
		return performArithmeticOperation(rhs, [](T x, T y) { return x -= y; });
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt32Wrapper operator*(T rhs) const {
		return performArithmeticOperation(rhs, [](T x, T y) { return x *= y; });
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt32Wrapper operator/(T rhs) const {
		return performArithmeticOperation(rhs, [](T x, T y) { return x /= y; });
	}


	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt32Wrapper& operator+=(T rhs) {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		dataAsT += rhs;
		std::memcpy(&data, &dataAsT, sizeof(uint32_t));
		return *this;
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt32Wrapper& operator-=(T rhs) {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		dataAsT -= rhs;
		std::memcpy(&data, &dataAsT, sizeof(uint32_t));
		return *this;
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt32Wrapper& operator*=(T rhs) {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		dataAsT *= rhs;
		std::memcpy(&data, &dataAsT, sizeof(uint32_t));
		return *this;
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt32Wrapper& operator/=(T rhs) {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		dataAsT /= rhs;
		std::memcpy(&data, &dataAsT, sizeof(uint32_t));
		return *this;
	}


	// Operator overloads for arithmetic operations
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	bool operator==(T rhs) const {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		return (dataAsT == rhs);
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	bool operator!=(T rhs) const {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		return dataAsT != rhs;
	}

	friend std::ostream& operator<<(std::ostream& os, const UInt32Wrapper& wrapper) {
		os << wrapper.data;
		return os;
	}
private:
	uint32_t data; // Stored uint32_t value
};