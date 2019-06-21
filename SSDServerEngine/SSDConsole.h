#pragma once
#include <stdio.h>

class SSDConsole 
{
	static FILE*				pConOut;


public:
	static void ConsoleOperation();
	static void ConsoleQuit();
};