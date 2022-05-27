#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <string>
#include "../OS11_HTAPI/Element.h"
#include "../OS11_HTAPI/HashTable.h"
#pragma comment(lib, "D:\\1\\Lab_11\\Debug\\OS11_HTAPI.lib")

int main(int argc, char** argv)
{
	HANDLE hf = NULL;
	try
	{
		TCHAR szExe[] = L"OS11_START.exe";
		if (!HashTable::IsProcessPresent((wchar_t*)&szExe))
		{
			std::cout << "OS11_START does not work \n";
			system("pause");
			return -1;

		}
		HashTable::HTHANDLE* HT = HashTable::Open("D:\\1\\Lab_11\\Debug\\HashTable.ht");
		if (HT == NULL) throw "Invalid handle";
		hf = CreateFile(L"D:\\1\\Lab_11\\update.log", GENERIC_WRITE | GENERIC_READ, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hf == INVALID_HANDLE_VALUE) throw "INVALID_HANDLE_VALUE";
		SetFilePointer(hf, NULL, NULL, FILE_END);

		int key;
		int pload;
		HashTable::Element* updating;
		HashTable::Element* getting;
		if (!WriteFile(hf, "START UPDATE\n", 13, NULL, NULL)) throw "WriteFile";
		for (int i = 1; i <= 100; i++)
		{
			if (HT->Addr == NULL) throw "HT IS CLOSEN";
			key = i;
			pload = i + 7;
			updating = new HashTable::Element(&key, sizeof(key));
			std::cout << "updating..." << std::endl;
			if (!HashTable::Update(HT, updating, &pload, sizeof(pload))) {
				if (!WriteFile(hf, "-- Error: Updating element\n", 27, NULL, NULL)) throw "WriteFile";
				std::cout << HashTable::GetLastErrorr(HT) << std::endl;
			}
			else {
				if ((getting = HashTable::Get(HT, updating)) == NULL)
				{
					if (!WriteFile(hf, "-- Error: Getting element\n", 26, NULL, NULL)) throw "WriteFile";
					std::cout << HashTable::GetLastErrorr(HT) << std::endl;
				}
				else {
					if (!WriteFile(hf, "-- Updating and getting element\n", 32, NULL, NULL)) throw "WriteFile";
					HashTable::print(getting);
				}
				if (!HashTable::IsProcessPresent((wchar_t*)&szExe))
				{
					if (!CloseHandle(hf)) throw "CloseHandle";
					std::cout << "OS11_START does not work \n";
					system("pause");
					return -1;
				}
			Sleep(1000);
			}
		}
		system("pause");
		if (!CloseHandle(hf)) throw "CloseHandle";
	}
	catch (const char* error)
	{
		std::cout << "-- Error: " << error << std::endl;
	}
}