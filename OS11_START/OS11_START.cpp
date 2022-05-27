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
		cout << argv[1] << endl;
		HMODULE hmdll = LoadLibrary(TEXT("D:\\1\\Lab_11\\Debug\\OS11_HTAPI.dll"));
		HashTable::HTHANDLE* (*Open)(const char*) = (HashTable::HTHANDLE * (*)(const char*)) GetProcAddress(hmdll, "Open");
		HashTable::HTHANDLE* (*Snap)(HashTable::HTHANDLE*) = (HashTable::HTHANDLE * (*)(HashTable::HTHANDLE*)) GetProcAddress(hmdll, "Snap");
		HashTable::HTHANDLE* (*GetLastErrorr)(HashTable::HTHANDLE*) = (HashTable::HTHANDLE * (*)(HashTable::HTHANDLE*)) GetProcAddress(hmdll, "GetLastErrorr");
		HashTable::HTHANDLE* (*Close)(HashTable::HTHANDLE*) = (HashTable::HTHANDLE * (*)(HashTable::HTHANDLE*)) GetProcAddress(hmdll, "Close");
		/*cout << "filename: ";
		char filename[100];
		cin >> filename;*/
		HashTable::HTHANDLE* HT = Open(argv[1]);
		if (HT == NULL) throw "Invalid handle";

		std::cout << "--- HT-Storage Started ---" << std::endl;
		std::cout << "filename: " << HT->FileName << std::endl;
		std::cout << "snapshotinterval: " << HT->Data.SecSnapshotInterval << std::endl;
		std::cout << "capacity: " << HT->Data.Capacity << std::endl;
		std::cout << "maxkeylength: " << HT->Data.MaxKeyLength << std::endl;
		std::cout << "maxdatalength: " << HT->Data.MaxPayloadLength << std::endl;

		std::cout << std::endl << "Press any key to close HT: ";
		char a;
		std::cin >> a;
		if (!Snap(HT)) throw GetLastErrorr(HT);
		if (!Close(HT)) throw "Error close";
		FreeLibrary(hmdll);
	}
	catch (const char* error) { std::cout << "-- Error: " << error << std::endl; }
}