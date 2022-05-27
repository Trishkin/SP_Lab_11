#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include "../OS11_HTAPI/Element.h"
#include "../OS11_HTAPI/HashTable.h"
using namespace std;

int main(int argc, char** argv)
{
	try
	{
		cout << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << " " << argv[5] << endl;
		HMODULE hmdll = LoadLibrary(TEXT("D:\\1\\Lab_11\\Debug\\OS11_HTAPI.dll"));
		HashTable::HTHANDLE* (*Create)(int, int, int, int, const char*) = (HashTable::HTHANDLE * (*)(int, int, int, int, const char*)) GetProcAddress(hmdll, "Create");
		HashTable::HTHANDLE* (*Close)(HashTable::HTHANDLE*) = (HashTable::HTHANDLE * (*)(HashTable::HTHANDLE*)) GetProcAddress(hmdll, "Close");
		/*cout << "capacity: ";
		int capacity;
		cin >> capacity;
		cout << "snapshotinterval: ";
		int snapshotinterval;
		cin >> snapshotinterval;
		cout << "maxkeylength: ";
		int maxkeylength;
		cin >> maxkeylength;
		cout << "maxdatalength: ";
		int maxdatalength;
		cin >> maxdatalength;
		cout << "filename: ";
		char filename[100];
		cin >> filename;*/

		HashTable::HTHANDLE* HT = Create(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5]);
		if (HT == NULL) throw "Invalid handle";

		std::cout << "--- HT-Storage Created ---" << std::endl;
		std::cout << "filename: " << HT->FileName << std::endl;
		std::cout << "snapshotinterval: " << HT->Data.SecSnapshotInterval << std::endl;
		std::cout << "capacity: " << HT->Data.Capacity << std::endl;
		std::cout << "maxkeylength: " << HT->Data.MaxKeyLength << std::endl;
		std::cout << "maxdatalength: " << HT->Data.MaxPayloadLength << std::endl;

		if (!Close(HT)) throw "Error close";
		FreeLibrary(hmdll);
	}
	catch (const char* error) { std::cout << "-- Error: " << error << std::endl;  }
}