#include "SSDConsole.h"
#include <stdio.h>
#include <Windows.h>


FILE*				SSDConsole::pConOut = nullptr;

void SSDConsole::ConsoleOperation() 
{
	AllocConsole();

	freopen_s(&pConOut,"CONOUT$", "wt", stdout);
}
void SSDConsole::ConsoleQuit() 
{
	if(nullptr != pConOut)
		FreeConsole();

}