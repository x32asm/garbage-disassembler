#include <iostream>
#include <vector>

#include "WindowsProcess.h"


std::vector<WindowsProcess> EnumProcesses() {
	std::vector<WindowsProcess> processes;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return processes;
	}

	PROCESSENTRY32 procInfo;
	procInfo.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hSnapshot, &procInfo)) {
		processes.push_back(WindowsProcess(procInfo));
	}
	else {
		CloseHandle(hSnapshot);
		return processes;
	}

	while (Process32Next(hSnapshot, &procInfo)) {
		processes.push_back(WindowsProcess(procInfo));
	}

	return processes;
}



int main(int argc, char ** argv) {
	_asm {
		mov eax, 10
		mov bl, al
	}

	return 0;
}