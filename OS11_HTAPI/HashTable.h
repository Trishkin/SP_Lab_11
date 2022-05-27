#pragma once
#include <Windows.h>
#include "Element.h"

namespace HashTable
{
	extern "C" struct __declspec(dllexport) HTData
	{
		HTData();
		HTData(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength);
		int Capacity;               // емкость хранилища в количестве элементов 
		int SecSnapshotInterval;    // переодичность сохранения в сек. 
		int MaxKeyLength;           // максимальная длина ключа
		int MaxPayloadLength;       // максимальная длина данных
	};

	extern "C" struct __declspec(dllexport) HTHANDLE    // блок управления HT
	{
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]);
		HTHANDLE(HTData* pHTData, const char FileName[512]);
		HANDLE hSnapThread;
		bool alive = true;
		HTData Data;
		int MaxContentSize;
		char    FileName[512];          // имя файла 
		HANDLE  File;                   // File HANDLE != 0, если файл открыт
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, если mapping создан  
		LPVOID  Addr;                   // Addr != NULL, если mapview выполнен  
		char    LastErrorMessage[512];  // сообщение об последней ошибке или 0x00  
		time_t  lastsnaptime;           // дата последнего snap'a (time()) 
		LPVOID keyAddr(int n) const;
		LPVOID payloadAddr(int n) const;
	};


	extern "C" __declspec(dllexport) HTHANDLE* Create   //  создать HT             
	(
		int	  Capacity,					   // емкость хранилища
		int   SecSnapshotInterval,		   // переодичность сохранения в сек.
		int   MaxKeyLength,                // максимальный размер ключа
		int   MaxPayloadLength,            // максимальный размер данных
		const char  FileName[512]          // имя файла 
	);

	extern "C" __declspec(dllexport) HTHANDLE* Open(const char FileName[512]);

	extern "C" __declspec(dllexport) BOOL Snap(const HTHANDLE* hthandle);

	extern "C" __declspec(dllexport) BOOL Close(HTHANDLE* hthandle);

	extern "C" __declspec(dllexport) BOOL Insert(const HTHANDLE* hthandle, const Element* element);

	extern "C" __declspec(dllexport) Element* Get(const HTHANDLE* hthandle, const Element* element);

	extern "C" __declspec(dllexport) BOOL Delete(const HTHANDLE* hthandle, const Element* element);

	extern "C" __declspec(dllexport) BOOL Update(const HTHANDLE* hthandle, const Element* oldelement, const void* newpayload, int newpayloadlength);

	extern "C" __declspec(dllexport) bool IsProcessPresent(wchar_t* szExe);

	extern "C" __declspec(dllexport) char* GetLastErrorr(HTHANDLE* ht);

};
