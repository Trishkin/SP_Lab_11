#pragma once
#include <Windows.h>
#include "Element.h"

namespace HashTable
{
	extern "C" struct __declspec(dllexport) HTData
	{
		HTData();
		HTData(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength);
		int Capacity;               // ������� ��������� � ���������� ��������� 
		int SecSnapshotInterval;    // ������������� ���������� � ���. 
		int MaxKeyLength;           // ������������ ����� �����
		int MaxPayloadLength;       // ������������ ����� ������
	};

	extern "C" struct __declspec(dllexport) HTHANDLE    // ���� ���������� HT
	{
		HTHANDLE();
		HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512]);
		HTHANDLE(HTData* pHTData, const char FileName[512]);
		HANDLE hSnapThread;
		bool alive = true;
		HTData Data;
		int MaxContentSize;
		char    FileName[512];          // ��� ����� 
		HANDLE  File;                   // File HANDLE != 0, ���� ���� ������
		HANDLE  FileMapping;            // Mapping File HANDLE != 0, ���� mapping ������  
		LPVOID  Addr;                   // Addr != NULL, ���� mapview ��������  
		char    LastErrorMessage[512];  // ��������� �� ��������� ������ ��� 0x00  
		time_t  lastsnaptime;           // ���� ���������� snap'a (time()) 
		LPVOID keyAddr(int n) const;
		LPVOID payloadAddr(int n) const;
	};


	extern "C" __declspec(dllexport) HTHANDLE* Create   //  ������� HT             
	(
		int	  Capacity,					   // ������� ���������
		int   SecSnapshotInterval,		   // ������������� ���������� � ���.
		int   MaxKeyLength,                // ������������ ������ �����
		int   MaxPayloadLength,            // ������������ ������ ������
		const char  FileName[512]          // ��� ����� 
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
