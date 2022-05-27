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
		hf = CreateFile(L"D:\\1\\Lab_11\\delete.log", GENERIC_WRITE | GENERIC_READ, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hf == INVALID_HANDLE_VALUE) throw "INVALID_HANDLE_VALUE";
		SetFilePointer(hf, NULL, NULL, FILE_END);

		int key;
		HashTable::Element* deleting;
		if (!WriteFile(hf, "START DELETE\n", 13, NULL, NULL)) throw "WriteFile";
		for (int i = 1; i <= 100; i++)
		{
				if (HT->Addr == NULL) throw "HT IS CLOSEN";
				key = i;
				deleting = new HashTable::Element(&key, sizeof(key));
				std::cout << "deleting..." << std::endl;
				if (!HashTable::Delete(HT, deleting))
				{
					if (!WriteFile(hf, "-- Error: Deleting element\n", 27, NULL, NULL)) throw "WriteFile";
					std::cout << "--Error: " << HashTable::GetLastErrorr(HT) << std::endl;
				}
				else if (!WriteFile(hf, "-- Deleting element\n", 20, NULL, NULL)) throw "WriteFile";
				if (!HashTable::IsProcessPresent((wchar_t*)&szExe))
				{
					if (!CloseHandle(hf)) throw "CloseHandle";
					std::cout << "OS11_START does not work \n";
					system("pause");
					return -1;
				}
				Sleep(1000);
		}
		system("pause");
		if (!CloseHandle(hf)) throw "CloseHandle";
	}
	catch (const char* error)
	{
		std::cout << "-- Error: " << error << std::endl;
	}
}