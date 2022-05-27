#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"
#include "HashTable.h"
#include "Element.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <regex>
#include <TCHAR.H>
#include <time.h>
#include <stdio.h>
#include <tlhelp32.h>


using namespace std;

namespace HashTable
{
	int CurrentSize = 0;

	HTData::HTData() {
	}

	HTData::HTData(
		int Capacity,
		int SecSnapshotInterval,
		int MaxKeyLength,
		int MaxPayloadLength
	)
	{
		this->Capacity = Capacity;
		this->SecSnapshotInterval = SecSnapshotInterval;
		this->MaxKeyLength = MaxKeyLength;
		this->MaxPayloadLength = MaxPayloadLength;
	}

	HTHANDLE::HTHANDLE()
	{
		HTData* Data = new HTData(10, 10, 10, 10);
		this->Data = *Data;
	}

	HTHANDLE::HTHANDLE(int Capacity, int SecSnapshotInterval, int MaxKeyLength, int MaxPayloadLength, const char FileName[512])
	{
		HashTable::HTData* Data = new HashTable::HTData(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength);
		this->Data = *Data;
		strcpy(this->FileName, FileName);
	}

	HashTable::HTHANDLE::HTHANDLE(HashTable::HTData* pHTData, const char FileName[512])
	{
		this->Data = *pHTData;
		strcpy(this->FileName, FileName);
	}



	DWORD WINAPI snapCallback(LPVOID hthandle)
	{
		int snapTime = ((HashTable::HTHANDLE*)hthandle)->Data.SecSnapshotInterval * 1000;
		int slept = 0;

		for (;;)
		{
			if (!((HashTable::HTHANDLE*)hthandle)->alive)
				return TRUE;
			int sleepTime = snapTime < 1000 ? snapTime : 500;
			Sleep(sleepTime);
			slept += sleepTime;

			if (snapTime <= slept)
			{
				HashTable::Snap(((HashTable::HTHANDLE*)hthandle));
				slept = 0;
			}

			
		}
	}

	int HashBytes(LPCVOID bytes, int numOfBytes)
	{
		LPBYTE pByte = (LPBYTE)bytes;
		int res = 0;
		for (int i = 0; i < numOfBytes; i++)
		{
			res ^= *(pByte + i) << (8 * (i % sizeof(int)));
		}

		return (int)abs(((float)res / 1.61803));
	}

	LPVOID HashTable::HTHANDLE::keyAddr(int n) const
	{
		return (LPVOID)((LPBYTE)(LPBYTE)this->Addr + sizeof(HTData) + n * (this->Data.MaxKeyLength + this->Data.MaxPayloadLength));
	}


	LPVOID HashTable::HTHANDLE::payloadAddr(int n) const
	{
		return (LPVOID)((LPBYTE)(LPBYTE)this->Addr + sizeof(HTData) + n * (this->Data.MaxKeyLength + this->Data.MaxPayloadLength)
			+ this->Data.MaxKeyLength);
	}

	bool isMemEmpty(LPCVOID mem, int len)
	{
		LPVOID zeroBuf = malloc(len);
		memset(zeroBuf, 0, len);
		bool res = (0 == memcmp(zeroBuf, mem, len));
		free(zeroBuf);
		return res;
	}

	HTHANDLE* Create(int	Capacity, int SecSnapshotInterval, int MaxKeyLength,
		int MaxPayloadLength, const char FileName[512])
	{
		CurrentSize = 0;
		HashTable::HTHANDLE* ht = new HashTable::HTHANDLE(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength, FileName);

		wchar_t wtext[60];
		mbstowcs(wtext, ht->FileName, strlen(ht->FileName) + 1);

		ht->File = CreateFile
		(wtext, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		HashTable::HTData* Data = new HashTable::HTData(Capacity, SecSnapshotInterval, MaxKeyLength, MaxPayloadLength);

		DWORD numberOfDataBytesWritten;
		WriteFile(ht->File, Data, sizeof(HashTable::HTData), &numberOfDataBytesWritten, NULL);

		if (ht->FileMapping != NULL)
			return HashTable::Open(FileName);
		else
			return NULL;
	}

	HTHANDLE* Open(const char FileName[512])
	{
		CurrentSize = 0;
		HashTable::HTHANDLE* ht = new HTHANDLE();
		strcpy(ht->FileName, FileName);

		wchar_t wtext[60];
		mbstowcs(wtext, ht->FileName, strlen(ht->FileName) + 1);

		ht->File = CreateFile
		(wtext, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		string t;
		HashTable::HTData Data;

		DWORD numberOfBytesRead;
		bool r = ReadFile(ht->File, &Data, sizeof(HashTable::HTData), &numberOfBytesRead, NULL);

		ht->Data = Data;
		ht->MaxContentSize = (ht->Data.Capacity * (ht->Data.MaxKeyLength + ht->Data.MaxPayloadLength));
		ht->FileMapping = CreateFileMapping(ht->File, NULL, PAGE_READWRITE, NULL, sizeof(HashTable::HTData) + ht->MaxContentSize, NULL);
		if (!ht->FileMapping) {
			GetLastErrorr((HashTable::HTHANDLE*)ht);
		}
		ht->Addr = MapViewOfFile(ht->FileMapping, FILE_MAP_ALL_ACCESS, NULL, 0, sizeof(HTData) + ht->MaxContentSize);
		if (!ht->Addr) {
			GetLastErrorr((HashTable::HTHANDLE*)ht);
		}

		ht->hSnapThread = CreateThread(NULL, NULL, snapCallback, (LPVOID)ht, NULL, NULL);

		if (ht->FileMapping != NULL)
			return ht;
		else
			return NULL;
	}

	BOOL Snap(const HTHANDLE* hthandle)
	{
		try
		{
			FlushViewOfFile(hthandle->Addr, sizeof(HTData) + hthandle->MaxContentSize);
			FlushFileBuffers(hthandle->File);
			cout << "Snap Created \n";
			return true;
		}
		catch (const std::exception&)
		{
			cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
			return false;
		}
	}

	BOOL Close(HTHANDLE* hthandle)
	{
		try
		{
			hthandle->alive = false;
			WaitForSingleObject(hthandle->hSnapThread, INFINITE);
			UnmapViewOfFile(hthandle->Addr);
			if (!CloseHandle(hthandle->File))
				throw "Close File failed";
			if (!CloseHandle(hthandle->FileMapping))
				throw "Close File failed";
			return true;
		}
		catch (const std::exception&)
		{
			return false;
		}
	}

	BOOL Insert(const HTHANDLE* hthandle, const Element* element)
	{
		try
		{
			int firstEmptyRecordInd = -1;
			int maxKeyLength = hthandle->Data.MaxKeyLength;
			int hash = HashBytes(element->key, element->keylength) % hthandle->Data.Capacity;

			for (int ii = hash; ii < hthandle->Data.Capacity + hash; ii++)
			{
				int i = ii % hthandle->Data.Capacity;

				LPCVOID nKey = hthandle->keyAddr(i);

				if (isMemEmpty(nKey, maxKeyLength))
					firstEmptyRecordInd = firstEmptyRecordInd == -1 ? i : firstEmptyRecordInd;

				LPVOID maxLenKey = malloc(maxKeyLength);
				memset(maxLenKey, 0, maxKeyLength);
				memcpy(maxLenKey, element->key, element->keylength);

				if (!memcmp(nKey, maxLenKey, maxKeyLength))
				{
					free(maxLenKey);
					cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
					return false;
				}
				free(maxLenKey);
			}

			if (firstEmptyRecordInd == -1)
			{
				cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
				return false;
			}

			LPVOID nKey = hthandle->keyAddr(firstEmptyRecordInd);
			LPVOID nPayload = hthandle->payloadAddr(firstEmptyRecordInd);

			memcpy(nKey, element->key, element->keylength);
			memcpy(nPayload, element->payload, element->payloadlength);

		}
		catch (const std::exception&)
		{
			cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
			return false;
		}
	}
	Element* Get(const HTHANDLE* hthandle, const Element* element)
	{
		try
		{
			int maxKeyLength = hthandle->Data.MaxKeyLength;
			int maxPayloadLength = hthandle->Data.MaxPayloadLength;
			int hash = HashBytes(element->key, element->keylength) % hthandle->Data.Capacity;
			for (int ii = hash; ii < hthandle->Data.Capacity + hash; ii++)
			{
				int i = ii % hthandle->Data.Capacity;
				LPVOID maxLenKey = malloc(maxKeyLength);
				memset(maxLenKey, 0, maxKeyLength);
				memcpy(maxLenKey, element->key, element->keylength);
				LPVOID nKey = hthandle->keyAddr(i);
				if (0 == memcmp(maxLenKey, nKey, maxKeyLength))
				{
					LPVOID newElKey = malloc(element->keylength);
					memcpy(newElKey, nKey, element->keylength);
					LPVOID newElPayload = malloc(maxPayloadLength);
					memcpy(newElPayload, hthandle->payloadAddr(i), maxPayloadLength);
					HashTable::Element* el = new HashTable::Element(
						newElKey,
						element->keylength,
						newElPayload,
						hthandle->Data.MaxPayloadLength
					);
					free(maxLenKey);
					return el;
				}
				free(maxLenKey);
			}
			return NULL;
		}
		catch (const std::exception&)
		{
			cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
			return NULL;
		}
	}

	BOOL Delete(const HTHANDLE* hthandle, const Element* element)
	{
		try
		{
			int maxKeyLength = hthandle->Data.MaxKeyLength;
			int hash = HashBytes(element->key, element->keylength) % hthandle->Data.Capacity;
			for (int ii = hash; ii < hthandle->Data.Capacity + hash; ii++)
			{
				int i = ii % hthandle->Data.Capacity;
				LPVOID maxLenKey = malloc(maxKeyLength);
				memset(maxLenKey, 0, maxKeyLength);
				memcpy(maxLenKey, element->key, element->keylength);
				LPVOID nKey = hthandle->keyAddr(i);
				if (0 == memcmp(maxLenKey, nKey, maxKeyLength))
				{
					memset(nKey, 0, maxKeyLength + hthandle->Data.MaxPayloadLength);
					free(maxLenKey);
					return TRUE;
				}
				free(maxLenKey);
			}

		}
		catch (const std::exception&)
		{
			cout << GetLastErrorr((HashTable::HTHANDLE*)hthandle);
			return false;
		}
	}


	BOOL Update(const HTHANDLE* hthandle, const Element* oldelement, const void* newpayload, int newpayloadlength)
	{
		try
		{
			int maxKeyLength = hthandle->Data.MaxKeyLength;
			int maxPayloadLength = hthandle->Data.MaxPayloadLength;
			int hash = HashBytes(oldelement->key, oldelement->keylength) % hthandle->Data.Capacity;
			for (int ii = hash; ii < hthandle->Data.Capacity + hash; ii++)
			{
				int i = ii % hthandle->Data.Capacity;
				LPVOID maxLenKey = malloc(maxKeyLength);
				memset(maxLenKey, 0, maxKeyLength);
				memcpy(maxLenKey, oldelement->key, oldelement->keylength);
				LPVOID nKey = hthandle->keyAddr(i);
				if (0 == memcmp(nKey, maxLenKey, maxKeyLength))
				{
					LPVOID oldPayload = hthandle->payloadAddr(i);
					memset(oldPayload, 0, maxPayloadLength);
					memcpy(oldPayload, newpayload, newpayloadlength);
					free(maxLenKey);
					return TRUE;
				}
				free(maxLenKey);
			}
		}
		catch (const std::exception&)
		{
			GetLastErrorr((HashTable::HTHANDLE*)hthandle);
			return false;
		}
	}
	bool IsProcessPresent(wchar_t* szExe)
	{
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

		PROCESSENTRY32 pe;
		pe.dwSize = sizeof(PROCESSENTRY32);
		Process32First(hSnapshot, &pe);

		if (!_wcsicmp((wchar_t*)&pe.szExeFile, szExe))
		{
			return true;
		}

		while (Process32Next(hSnapshot, &pe))
		{
			if (!_wcsicmp((wchar_t*)&pe.szExeFile, szExe))
			{
				return true;
			}
		}

		return false;
	}
	char* GetLastErrorr(HTHANDLE* ht)
	{
		string message = string("Îרטבךא ןנט נאבמעו ס פאיכמל: ").append(ht->FileName);
		return (char*)message.c_str();
	}
}