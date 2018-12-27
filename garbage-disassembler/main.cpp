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
	ExitProcess(dw);
}


int main(int argc, char ** argv) {
	pvlg::acquirePrivilegeByName(SE_DEBUG_NAME);
	/*const std::size_t dwSize = 32767;



	std::vector<PROCESSENTRY32> processes; processes.resize(dwSize);

	std::size_t pCount = load::EnumerateProcesses(&processes[0], dwSize);
	processes.resize(pCount);
	printProcesses(processes);*/


	const CHAR * szProcessName = "ac_client.exe";
	HANDLE hProcess = pld::GetProcessHandle(szProcessName);
	DWORD dwBaseAddr = pld::GetModuleBaseAddress(hProcess, szProcessName);

	std::printf("%s - %.8X\n", szProcessName, dwBaseAddr);

	const SIZE_T nSize = 1024;
	CHAR buffer[nSize];
	DWORD cNumberOfBytesRead;
	ReadProcessMemory(hProcess, (LPVOID)dwBaseAddr, buffer, nSize, &cNumberOfBytesRead);
	for (int i = 0; i < nSize; ++i) {
		std::printf("%.2X ", buffer[i]);
	}
	

	/*std::vector<MODULEENTRY32> modules; modules.resize(dwSize);
	
	std::size_t mCount = load::EnumModules(&modules[0], dwSize, processes[0].th32ProcessID);
	modules.resize(mCount);
	printModules(modules);*/


	//MODULEENTRY32 m;
	//for (std::size_t i = 0; i < processes.size(); ++i) {
	//	if (!load::EnumModules(&modules[0], dwSize, processes[i].th32ProcessID)) {
	//		if ((m = load::FindModule(processes[i].szExeFile, processes[i].th32ProcessID)).modBaseSize < 0) {
	//			std::cout << processes[i].szExeFile << '\n';
	//		}
	//		else if (!strcmp(processes[i].szExeFile, "firefox.exe")) {
	//			std::cout << reinterpret_cast<DWORD*>(m.modBaseAddr) << ';' << m.modBaseSize << std::setw(7) << "- firefox.exe\n";
	//			std::printf("%p;%d\t- firefox.exe\n", m.modBaseAddr, m.modBaseSize);
	//		}
	//	}
	//}


	return 0;
}
