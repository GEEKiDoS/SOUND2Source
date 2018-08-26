#include "MainForm.h"
#include <windows.h>
#include <stdio.h>

using namespace Sound2Source;
using namespace System::Windows::Forms;
using namespace System;

void ACMMain();

[System::STAThreadAttribute]
int main(array<String^> ^args) {
#ifdef _DEBUG
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	SetConsoleTitleA("Debug Console");
#endif

	ACMMain();

	Application::EnableVisualStyles();
	MainForm^ window = gcnew MainForm();
	if (args->Length >= 1) {
		window->ProcessArgs(args);
	}
	Application::Run(window);
	
	return 0;
}