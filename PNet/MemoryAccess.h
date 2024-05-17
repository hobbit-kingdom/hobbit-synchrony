#pragma once
#include <vector>
#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memoryapi.h>
#include <tlhelp32.h>
#include <vector>

#include <cstring> // for std::memcpy
#include <type_traits> // for std::enable_if and std::is_arithmetic

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
class UInt64Wrapper {
public:
	// Constructor
	// Default constructor
	UInt64Wrapper() : data(0) {}

	// Constructor taking any type and storing it in data
	template<typename TYPE>
	explicit UInt64Wrapper(TYPE value) : data(static_cast<uint64_t>(value)) {}

	// Templated function for reinterpretation of byte representation as other types
	template<typename T>
	static T reinterpret_as(const uint64_t& data) {
		static_assert(sizeof(T) == sizeof(uint64_t), "Size mismatch between types");
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
	UInt64Wrapper performArithmeticOperation(T rhs, Func operation) const {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		T result = operation(dataAsT, rhs);
		uint64_t resultAsUint64;
		std::memcpy(&resultAsUint64, &result, sizeof(uint64_t));
		return UInt64Wrapper(resultAsUint64);
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt64Wrapper& operator=(T rhs) {
		data = static_cast<uint64_t>(rhs);
		return *this;
	}

	// Operator overloads for arithmetic operations
	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt64Wrapper operator+(T rhs) const {
		return performArithmeticOperation(rhs, [](T x, T y) { return x += y; });
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt64Wrapper operator-(T rhs) const {
		return performArithmeticOperation(rhs, [](T x, T y) { return x -= y; });
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt64Wrapper operator*(T rhs) const {
		return performArithmeticOperation(rhs, [](T x, T y) { return x *= y; });
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt64Wrapper operator/(T rhs) const {
		return performArithmeticOperation(rhs, [](T x, T y) { return x /= y; });
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt64Wrapper& operator+=(T rhs) {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		dataAsT += rhs;
		std::memcpy(&data, &dataAsT, sizeof(uint64_t));
		return *this;
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt64Wrapper& operator-=(T rhs) {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		dataAsT -= rhs;
		std::memcpy(&data, &dataAsT, sizeof(uint64_t));
		return *this;
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt64Wrapper& operator*=(T rhs) {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		dataAsT *= rhs;
		std::memcpy(&data, &dataAsT, sizeof(uint64_t));
		return *this;
	}

	template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	UInt64Wrapper& operator/=(T rhs) {
		T dataAsT;
		std::memcpy(&dataAsT, &data, sizeof(T));
		dataAsT /= rhs;
		std::memcpy(&data, &dataAsT, sizeof(uint64_t));
		return *this;
	}

	// Operator overloads for comparison operations
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

	friend std::ostream& operator<<(std::ostream& os, const UInt64Wrapper& wrapper) {
		os << wrapper.data;
		return os;
	}
private:
	uint64_t data; // Stored uint64_t value
};




namespace memoryAccess {
	using PROCESS = HANDLE;

	enum
	{
		memQueryFlags_ = 0,
		memQueryFlags_None = 0,
		memQueryFlags_Read = 0b0001,
		memQueryFlags_Write = 0b0010,
		memQueryFlags_Execute = 0b0100,

	};
	struct OppenedQuery
	{
		PROCESS queriedProcess = 0;
		char* baseQueriedPtr = (char*)0x0;
		bool oppened()
		{
			return queriedProcess != 0;
		}
	};

	static std::string executableName = "";

	class MemoryAccess
	{
	public:

		static std::string getExecutableName();
		static void setExecutableName(const std::string& newName);

		static HANDLE readProcess(const char* processName);
		static HANDLE readProcess();
		static DWORD readProcessID(const char* name);
		static DWORD readProcessID();

		static UInt32Wrapper ReadData(LPVOID Address)
		{
			uint32_t data = 0;
			HANDLE Process = readProcess();
			if (!ReadProcessMemory(Process, Address, &data, sizeof(data), NULL)) { // Reading the data from memory

				data = 0;
			}
			CloseHandle(Process);
			return UInt32Wrapper(data);
		}
		static UInt64Wrapper ReadData64(LPVOID Address)
		{
			uint64_t data = 0;
			HANDLE Process = readProcess();
			if (!ReadProcessMemory(Process, Address, &data, sizeof(data), NULL)) { // Reading the data from memory

				data = 0;
			}
			CloseHandle(Process);
			return UInt64Wrapper(data);
		}

		// templates
		template<typename T>
		static T WriteDataSwitcher(LPVOID Address, T newData, T initialData)
		{
			T data;  // Variable to store the data read from memory
			HANDLE Process = readProcess();
			if (!ReadProcessMemory(Process, Address, &data, sizeof(data), NULL)) { // Reading the data from memory
				CloseHandle(Process);
				return 1;
			}

			DWORD oldProtect;
			if (data == initialData) {
				SIZE_T dwSize = sizeof(newData);
				VirtualProtectEx(Process, Address, dwSize, PAGE_EXECUTE_READWRITE, &oldProtect);
				BOOL bWriteSuccess = WriteProcessMemory(Process, Address, &newData, dwSize, NULL);
				VirtualProtectEx(Process, Address, dwSize, oldProtect, &oldProtect);
				CloseHandle(Process);
				if (bWriteSuccess) return 0;
			}
			else {
				SIZE_T dwSize = sizeof(initialData);
				VirtualProtectEx(Process, Address, dwSize, PAGE_EXECUTE_READWRITE, &oldProtect);
				BOOL bWriteSuccess = WriteProcessMemory(Process, Address, &initialData, dwSize, NULL);
				VirtualProtectEx(Process, Address, dwSize, oldProtect, &oldProtect);
				CloseHandle(Process);
				if (bWriteSuccess) return 0;
			}
		}
		template<typename T>
		static T WriteData(LPVOID Address, T data)
		{
			HANDLE Process = readProcess();
			T temporary = data;
			if (!ReadProcessMemory(Process, Address, &temporary, sizeof(temporary), NULL)) { // Reading the value from memory
				CloseHandle(Process);
				return T(); // Return default value of type T if reading fails
			}
			DWORD oldProtect;

			SIZE_T dwSize = sizeof(data);
			VirtualProtectEx(Process, Address, dwSize, PAGE_EXECUTE_READWRITE, &oldProtect);
			BOOL bWriteSuccess = WriteProcessMemory(Process, Address, &data, dwSize, NULL);
			VirtualProtectEx(Process, Address, dwSize, oldProtect, &oldProtect);
			if (bWriteSuccess) return T(); // Return default value of type T if writing fails
		}


		// additional funcitons
		static std::vector<void*> findBytePatternInProcessMemory(void* pattern, size_t patternLen);
		
		static bool getNextQuery(OppenedQuery& query, void*& low, void*& hi, int& flags);
		static OppenedQuery initVirtualQuery(PROCESS process);
		
		private:
	};
}


