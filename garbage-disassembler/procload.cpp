#include "procload.h"

#include <psapi.h>
#include <stdexcept>


SIZE_T pld::EnumerateProcesses(PROCESSENTRY32 * pContainer, SIZE_T nContainerSize) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return 0;
	}

	PROCESSENTRY32 procInfo;
	procInfo.dwSize = sizeof(PROCESSENTRY32);

	SIZE_T count = 0;

	if (Process32First(hSnapshot, &procInfo)) {
		if (count >= nContainerSize) {
			throw std::runtime_error("not enough space allocated in container.");
		}
		pContainer[count++] = procInfo;
	}
	else {
		CloseHandle(hSnapshot);
		return 0;
	}

	while (Process32Next(hSnapshot, &procInfo)) {
		if (count >= nContainerSize) {
			throw std::runtime_error("not enough space allocated in container.");
		}
		pContainer[count++] = procInfo;
	}

	CloseHandle(hSnapshot);
	return count;
}


HANDLE pld::GetProcessHandle(const CHAR * szProcessName, DWORD * pdwProcessID, DWORD dwDesiredAccess) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	PROCESSENTRY32 procInfo;
	procInfo.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &procInfo)) {
		do {
			if (!strcmp(szProcessName, procInfo.szExeFile)) {
				CloseHandle(hSnapshot);
				if (pdwProcessID != nullptr) *pdwProcessID = procInfo.th32ProcessID;
				return OpenProcess(dwDesiredAccess, TRUE, procInfo.th32ProcessID);
			}
		} while (Process32Next(hSnapshot, &procInfo));
	}

	CloseHandle(hSnapshot);
	return NULL;
}

HANDLE pld::GetProcessHandle(DWORD dwProcessID, DWORD dwDesiredAccess) {
	return OpenProcess(dwDesiredAccess, TRUE, dwProcessID);
}

#include <iostream>
LPVOID pld::GetModuleBaseAddress(HANDLE hProcess, const CHAR * szModuleName) {
	HMODULE hModules[1024];
	CHAR szBuffer[255];
	DWORD cModules;
	MODULEINFO mModInfo;
	LPVOID lpvBase = NULL;

	if (EnumProcessModules(hProcess, hModules, sizeof(HMODULE), &cModules)) {
		std::cout << "cModules: " << cModules / sizeof(HMODULE) << '\n';
		for (SIZE_T i = 0; i < cModules / sizeof(HMODULE); ++i) {
			GetModuleBaseName(hProcess, hModules[i], szBuffer, sizeof(szBuffer));
			std::cout << szBuffer << '\n';
			if (!strcmp(szModuleName, szBuffer)) {
				GetModuleInformation(hProcess, hModules[i], &mModInfo, sizeof(MODULEINFO));
				lpvBase = mModInfo.EntryPoint;
				break;
			}
		}
	}

	return lpvBase;
}


BYTE * pld::GetProcessBaseAddress(DWORD dwProcessID, const CHAR * szProcessName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);

	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return NULL;
	}

	MODULEENTRY32 modInfo;
	modInfo.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &modInfo)) {
		do {
			if (!strcmp(szProcessName, modInfo.szModule)) {
				CloseHandle(hSnapshot);
				return modInfo.modBaseAddr;
			}
		} while (Module32Next(hSnapshot, &modInfo));
	}

	CloseHandle(hSnapshot);
	return NULL;
}