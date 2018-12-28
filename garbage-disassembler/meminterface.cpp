#include "meminterface.h"

#include <tlhelp32.h>
#include <psapi.h>


MemInterface::MemInterface(DWORD dwProcessID) :
	dwProcessID(dwProcessID), szProcessName(), hProcess(NULL),
	lpvBaseAddr(NULL), dwExeModuleOffset(NULL), dwCurrentOffset(0) {
}

MemInterface::MemInterface(const std::string & szProcessName) :
	dwProcessID(NULL), szProcessName(szProcessName), hProcess(NULL),
	lpvBaseAddr(NULL), dwExeModuleOffset(NULL), dwCurrentOffset(0) {
}

MemInterface::MemInterface(const CHAR * szProcessName) :
	dwProcessID(NULL), szProcessName(szProcessName), hProcess(NULL),
	lpvBaseAddr(NULL), dwExeModuleOffset(NULL), dwCurrentOffset(0) {
}


MemInterface::~MemInterface() {
	if (hProcess != NULL) CloseHandle(hProcess);
}





BOOL MemInterface::Open(DWORD dwDesiredAccess) {
	if (hProcess != NULL && hProcess != INVALID_HANDLE_VALUE) CloseHandle(hProcess);
	if (dwProcessID != NULL) {
		if (!GetProcessHandleWithProcessID(dwDesiredAccess)) {
			return FALSE;
		}
	}
	else {
		if (!GetProcessHandleWithProcessName(dwDesiredAccess)) {
			return FALSE;
		}
	}

	if (!GetBaseAddress()) {
		return FALSE;
	}
	if (!GetExeModuleOffset()) {
		return FALSE;
	}

	return TRUE;
}




MDWORD MemInterface::GetModuleOffset(const std::string & szModuleName) {
	HMODULE hModules[1024];
	CHAR szBuffer[255];
	DWORD cModules;
	MODULEINFO mModInfo;

	if (EnumProcessModules(hProcess, hModules, sizeof(HMODULE), &cModules)) {
		for (SIZE_T i = 0; i < cModules / sizeof(HMODULE); ++i) {
			GetModuleBaseName(hProcess, hModules[i], szBuffer, sizeof(szBuffer));
			if (!strcmp(szProcessName.c_str(), szBuffer)) {
				GetModuleInformation(hProcess, hModules[i], &mModInfo, sizeof(MODULEINFO));
				return (MDWORD)mModInfo.EntryPoint - (MDWORD)lpvBaseAddr;
			}
		}
	}

	return NULL;
}




SIZE_T MemInterface::ReadNext(LPVOID buffer, SIZE_T bufferLen) {
	SIZE_T cNumberOfBytesRead;
	ReadProcessMemory(hProcess, (LPVOID)((MDWORD)lpvBaseAddr + dwCurrentOffset), buffer, bufferLen, &cNumberOfBytesRead);
	dwCurrentOffset += cNumberOfBytesRead;
	return cNumberOfBytesRead;
}

SIZE_T MemInterface::ReadSegment(LPVOID address, LPVOID buffer, SIZE_T bufferLen) {
	SIZE_T cNumberOfBytesRead;
	ReadProcessMemory(hProcess, address, buffer, bufferLen, &cNumberOfBytesRead);
	dwCurrentOffset = (MDWORD)address + cNumberOfBytesRead;
	return cNumberOfBytesRead;
}




BOOL MemInterface::GetProcessHandleWithProcessName(DWORD dwDesiredAccess) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	dwProcessID = NULL;
	hProcess = NULL;

	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	PROCESSENTRY32 procInfo;
	procInfo.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &procInfo)) {
		do {
			if (!szProcessName.compare(procInfo.szExeFile)) {
				CloseHandle(hSnapshot);
				dwProcessID = procInfo.th32ProcessID;
				hProcess = OpenProcess(dwDesiredAccess, TRUE, procInfo.th32ProcessID);
				return TRUE;
			}
		} while (Process32Next(hSnapshot, &procInfo));
	}

	CloseHandle(hSnapshot);
	return FALSE;
}

BOOL MemInterface::GetProcessHandleWithProcessID(DWORD dwDesiredAccess) {
	hProcess = OpenProcess(dwDesiredAccess, TRUE, dwProcessID);

	szProcessName.resize(255);
	SIZE_T nStrLen = GetProcessImageFileName(hProcess, (LPSTR)&szProcessName[0], (DWORD)szProcessName.size());
	szProcessName.resize(nStrLen); szProcessName = szProcessName.substr(szProcessName.find_last_of('\\') + 1);

	return !(hProcess == INVALID_HANDLE_VALUE || hProcess == NULL);
}


BOOL MemInterface::GetBaseAddress() {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessID);

	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	MODULEENTRY32 modInfo;
	modInfo.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &modInfo)) {
		do {
			if (!szProcessName.compare(modInfo.szModule)) {
				CloseHandle(hSnapshot);
				lpvBaseAddr = modInfo.modBaseAddr;
				return TRUE;
			}
		} while (Module32Next(hSnapshot, &modInfo));
	}

	CloseHandle(hSnapshot);
	return FALSE;
}

BOOL MemInterface::GetExeModuleOffset() {
	HMODULE hModules[1024];
	CHAR szBuffer[255];
	DWORD cModules;
	MODULEINFO mModInfo;

	if (EnumProcessModules(hProcess, hModules, sizeof(HMODULE), &cModules)) {
		for (SIZE_T i = 0; i < cModules / sizeof(HMODULE); ++i) {
			GetModuleBaseName(hProcess, hModules[i], szBuffer, sizeof(szBuffer));
			if (!szProcessName.compare(szBuffer)) {
				GetModuleInformation(hProcess, hModules[i], &mModInfo, sizeof(MODULEINFO));
				dwExeModuleOffset = (MDWORD)mModInfo.EntryPoint - (MDWORD)lpvBaseAddr;
				return TRUE;
			}
		}
	}

	return FALSE;
}




DWORD MemInterface::ProcessID() const {
	return dwProcessID;
}

const std::string & MemInterface::ProcessName() const {
	return szProcessName;
}


LPVOID MemInterface::BaseAddress() const {
	return lpvBaseAddr;
}


MDWORD MemInterface::ExeModuleOffset() const {
	return dwExeModuleOffset;
}

MDWORD MemInterface::CurrentOffset() const {
	return dwCurrentOffset;
}




LPVOID ofs::CalcAddress(LPVOID lpvBaseAddr, MDWORD dwOffset) {
	return (LPVOID)((MDWORD)lpvBaseAddr + dwOffset);
}