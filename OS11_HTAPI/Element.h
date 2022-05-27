#pragma once
#include <Windows.h>

namespace HashTable
{

	extern "C" struct __declspec(dllexport) Element
	{
		Element();
		Element(const void* key, int keylength);                                             // for Get
		Element(const void* key, int keylength, const void* payload, int  payloadlength);    // for Insert
		Element(Element* oldelement, const void* newpayload, int  newpayloadlength);         // for update
		const void* key;         // значение ключа 
		int keylength;           // рахмер ключа
		const void* payload;     // данные 
		int payloadlength;       // размер данных
	};

	extern "C" __declspec(dllexport) void print
	(
		const Element* element
	);
}