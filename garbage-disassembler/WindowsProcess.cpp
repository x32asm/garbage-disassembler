#include "WindowsProcess.h"
#include <iostream>


WindowsProcess::WindowsProcess(const PROCESSENTRY32 & procInfo) :
	pInfo(procInfo) {
}




bool WindowsProcess::OpenHandle() {
	pHandle = OpenProcess(PROCESS_VM_READ, TRUE, pId);

	if (pHandle == NULL) {
		DWORD lastErr = GetLastError();
		if (lastErr == ERROR_INVALID_PARAMETER) {
			std::printf("Invalid process id, make sure the process is still open.");
		}
		else if (lastErr == ERROR_ACCESS_DENIED) {
			std::printf("Handle access denied.");
		}
		return false;
	}
	return true;
}


bool WindowsProcess::LoadModule(const std::string & moduleName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pId);

	if (hSnapshot == INVALID_HANDLE_VALUE) {
		mod.modBaseAddr = 0;
		return false;
	}

	MODULEENTRY32 modEntry;
	modEntry.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &modEntry)) {
		if (!strcmp(modEntry.szModule, moduleName.c_str())) {
			CloseHandle(hSnapshot);
			mod = modEntry;
			return true;
		}
	}
	else {
		CloseHandle(hSnapshot);
		mod.modBaseAddr = 0;
		return false;
	}

	while (Module32Next(hSnapshot, &modEntry)) {
		if (!strcmp(modEntry.szModule, moduleName.c_str())) {
			CloseHandle(hSnapshot);
			mod = modEntry;
			return true;
		}
	}

	CloseHandle(hSnapshot);
	mod.modBaseAddr = 0;
	return false;
}




const std::string & WindowsProcess::getProcessName() const {
	return pName;
}

DWORD WindowsProcess::getProcessId() const {
	return pId;
}

const PROCESSENTRY32 & WindowsProcess::getProcessInfo() const {
	return pInfo;
}