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


using namespace std;

namespace HashTable
{
	Element::Element()
	{
	}

	Element::Element(const void* key, int keylength)
	{
		this->key = (char*)key;
		this->keylength = keylength;
	}

	Element::Element(const void* key, int keylength, const void* payload, int  payloadlength)
	{
		this->key = (char*)key;
		this->keylength = keylength;
		this->payload = (char*)payload;
		this->payloadlength = payloadlength;
	}

	Element::Element(Element* oldelement, const void* newpayload, int  newpayloadlength)
	{
		oldelement->payload = newpayload;
		oldelement->payloadlength = newpayloadlength;
	}

	void print(const Element* element)
	{
		std::cout << "Key: " << *(int*)element->key << "\n"
			<< "Key lenght: " << element->keylength << "\n"
			<< "Payload: " << *(int*)element->payload << "\n"
			<< "Payload lenght: " << element->payloadlength << "\n";
	}
}