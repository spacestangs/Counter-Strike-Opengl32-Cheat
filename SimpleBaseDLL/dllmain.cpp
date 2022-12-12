// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

//==============================================================================
void __stdcall CallFunction()
{
	__asm{push edx}
}
//==============================================================================

//==============================================================================
BOOL APIENTRY DllMain(HANDLE hInst, DWORD dwReason, LPVOID lpReserved)
{
	// Declare 2 strings title and caption
	wchar_t const *title = L"\tCheat Activated!\r \n \n NUMPAD1 Wallhack \n NUMPAD2 Perfect Wallhack \n NUMPAD3 No Smoke \n NUMPAD4 Black sky \n NUMPAD5 LAMBERT \n NUMPAD6 No flash \n NUMPAD7 Night mode \n NUMPAD8 White Walls \n NUMPAD9 Inverted Models";
	wchar_t const *caption = L"\t MultiHack by AlexTazi";
	

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		HookOpenGL();
		MessageBox(NULL, title, caption, MB_OK);
	}
	return TRUE;
}
//==============================================================================
