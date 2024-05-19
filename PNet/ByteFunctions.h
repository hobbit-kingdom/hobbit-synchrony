#pragma once


#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>
#include <memoryapi.h>
#include <tlhelp32.h>
#include <vector>
#include <unordered_map>
#include "Packet.h"


void floatToBytes(float floatValue, uint32_t& uintValue) noexcept;


float uint32ToFloat(uint32_t uintValue) noexcept;

HANDLE read_process_hobbit() noexcept;


int change_float_hobbit(LPVOID Address, float a) noexcept;



int change_1Byte_hobbit_no_switch(LPVOID Address, BYTE Znachenie) noexcept;


DWORD findPidByName(const char* name) noexcept;

//enum
//{
//	memQueryFlags_ = 0,
//	memQueryFlags_None = 0,
//	memQueryFlags_Read = 0b0001,
//	memQueryFlags_Write = 0b0010,
//	memQueryFlags_Execute = 0b0100,
//
//};

struct OppenedQuery;


bool getNextQuery(OppenedQuery& query, void*& low, void*& hi, int& flags) noexcept;


OppenedQuery initVirtualQuery(HANDLE process) noexcept;


bool readMemory(HANDLE process, void* start, size_t size, void* buff) noexcept;


std::vector<void*> findBytePatternInProcessMemory(HANDLE process, void* pattern, size_t patternLen) noexcept;


int read_int_value(LPVOID Address) noexcept;


LPDWORD ukazatel_hobbit(LPVOID Address) noexcept;


float read_float_value(LPVOID Address)noexcept;

