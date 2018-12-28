#include <iostream>
#include <ios>
#include <iomanip>
#include <vector>

#include "procload.h"
#include "priviledges.h"

#include <psapi.h>
#include <strsafe.h>
#include <string>


void printProcesses(const std::vector<PROCESSENTRY32> & procs) {
	std::size_t numDigits = std::to_string(procs.size()).size();
	for (std::size_t i = 0; i < procs.size(); ++i) {
		std::cout << std::setw(numDigits) << std::left << i << " | " << procs[i].szExeFile << '\n';
	}
	std::cout << std::endl;
}

void printModules(const std::vector<MODULEENTRY32> & procs) {
	std::size_t numDigits = std::to_string(procs.size()).size();
	for (std::size_t i = 0; i < procs.size(); ++i) {
		std::cout << std::setw(numDigits) << std::left << i << " | " << procs[i].szModule << '\n';
	}
	std::cout << std::endl;
}


void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}


int main(int argc, char ** argv) {
	pvlg::acquirePrivilegeByName(SE_DEBUG_NAME);


	std::string szProcessName;
	std::vector<PROCESSENTRY32> processes; processes.resize(512);
	SIZE_T cNumberOfProcesses = pld::EnumerateProcesses(&processes[0], processes.size());
	processes.resize(cNumberOfProcesses);

	printProcesses(processes);


	std::cin >> szProcessName;


	DWORD pId;
	HANDLE hProcess = pld::GetProcessHandle(szProcessName.c_str(), &pId);
	LPVOID lpvBaseAddr = pld::GetModuleBaseAddress(hProcess, szProcessName.c_str());
	if (lpvBaseAddr == NULL) {
		ErrorExit(TEXT((LPSTR)"GetModuleBaseAddress()"));
	}

	std::printf("%s - %p\n", szProcessName.c_str(), lpvBaseAddr);

	const SIZE_T nSize = 1024;
	char buffer[nSize];
	SIZE_T cNumberOfBytesRead;
	ReadProcessMemory(hProcess, lpvBaseAddr, buffer, nSize, &cNumberOfBytesRead);
	for (int i = 0; i < nSize; ++i) {
		short op = buffer[i] & 0xFF;
		std::printf("%.2X ", op);
	}

	return 0;
}