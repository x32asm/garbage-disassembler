#ifndef GUARD_MEM_INTERFACE_H
#define GUARD_MEM_INTERFACE_H
#pragma once

#include <windows.h>
#include <string>


#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif

#ifdef ENVIRONMENT64
#define MDWORD DWORD64
#else
#define MDWORD DWORD
#endif


class MemInterface {
public:

	MemInterface(DWORD processId);
	MemInterface(const std::string & processName);
	MemInterface(const CHAR * processName);

	~MemInterface();



	BOOL Open(DWORD desiredAccess = PROCESS_ALL_ACCESS);



	MDWORD GetModuleOffset(const std::string & moduleName);



	SIZE_T ReadNext(LPVOID buffer, SIZE_T bufferLen);
	SIZE_T ReadSegment(LPVOID address, LPVOID buffer, SIZE_T bufferLen);



	DWORD ProcessID() const;
	const std::string & ProcessName() const;

	LPVOID BaseAddress() const;
	MDWORD ExeModuleOffset() const;
	MDWORD CurrentOffset() const;

private:

	BOOL GetProcessHandleWithProcessName(DWORD desiredAccess = PROCESS_ALL_ACCESS);
	BOOL GetProcessHandleWithProcessID(DWORD desiredAccess = PROCESS_ALL_ACCESS);

	BOOL GetBaseAddress();
	BOOL GetExeModuleOffset();



	DWORD dwProcessID;
	std::string szProcessName;

	HANDLE hProcess;
	LPVOID lpvBaseAddr;
	MDWORD dwExeModuleOffset;
	MDWORD dwCurrentOffset;
};


namespace ofs {
	LPVOID CalcAddress(LPVOID baseAddress, MDWORD offset);
}


#endif //GUARD_MEM_INTERFACE_H