#include "pch.h"
#include "CSource2Client.h"

typedef void* (*CreateInterfaceFn)(const char* pName, int* pReturnCode);

inline void* GetInterface(const char* dllname, const char* interfacename) {
	CreateInterfaceFn CreateInterface = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandle(dllname), "CreateInterface"));

	int returnCode = 0;
	void* interface = CreateInterface(interfacename, &returnCode);

	return interface;
}

void console()
{
	if (!AllocConsole()) {
		// Add some error handling here.
		// You can call GetLastError() to get more info about the error.
		return;
	}

	// std::cout, std::clog, std::cerr, std::cin
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();

	// std::wcout, std::wclog, std::wcerr, std::wcin
	HANDLE hConOut = CreateFile(("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFile(("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);
	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();
}

DWORD WINAPI Thread(HMODULE hModule)
{
	console();

	CSource2Client* client = reinterpret_cast<CSource2Client*>(GetInterface("client.dll", "Source2Client002"));


	for (ClientClass* classes = client->GetAllClasses(); classes; classes = classes->m_pNext) {
		if (!classes->recvTable || !classes->recvTable->netVarsArray || !classes->m_pClassName)
			continue;
		printf("%s - NumOfVars: %d; Class Name; Variable Type; Offset (Hex); Offset (Decimal)\n", classes->m_pClassName, classes->recvTable->numOfVars);

		//printf("Count: %d", classes->recvTable->numOfVars);

		if (std::strcmp(classes->m_pClassName, "C_BasePlayer") == 0)
		{
			for (int i = 0; i < classes->recvTable->numOfVars; i++) {

				Netvar* var = classes->recvTable->netVarsArray[i].netVar;

				if (!var || !var->netvarName || !var->typeName || !var->offset)
					break;
				printf("\t[%d]; %s; %s; 0x%x; %d\n", i + 1, var->netvarName, var->typeName, var->offset, var->offset);
			}
		}
	
		
		
	}

	/*FILE* stream;
	errno_t err = fopen_s(&stream, "all_classes.txt", "w+");

	if (err == 0)
	{
		for (ClientClass* classes = client->GetAllClasses(); classes; classes = classes->m_pNext) {
			if (!classes->recvTable || !classes->recvTable->netVarsArray || !classes->m_pClassName)
				continue;

			fprintf(stream, "%s - NumOfVars: %d; Class Name; Variable Type; Offset (Hex); Offset (Decimal)\n", classes->m_pClassName, classes->recvTable->numOfVars);
			for (int i = 0; i < classes->recvTable->numOfVars; i++) {
				Netvar* var = classes->recvTable->netVarsArray[i].netVar;
				if (!var
					|| !var->netvarName
					|| !var->typeName)
					break;

				fprintf(stream, "\t[%d]; %s; %s; 0x%x; %d\n", i + 1, var->netvarName, var->typeName, var->offset, var->offset);
			}
		}

		err = fclose(stream);
	}*/

    //FreeLibraryAndExitThread(hModule, 0);

	return 1;
}


DWORD WINAPI Thread2(HMODULE hModule)
{
		MessageBoxW(nullptr,
			L"Hello from dllToInject!",
			L"Hello",
			MB_ICONINFORMATION);
		FreeLibraryAndExitThread(hModule, 0);
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  reason, LPVOID lpReserved) {

	if (reason == DLL_PROCESS_ATTACH)
	{
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Thread, hModule, 0, 0);
		//CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Thread, hModule, 0, 0));
	}

    return TRUE;
}