#ifndef GUARD_PROCLOAD_H
#define GUARD_PROCLOAD_H
#pragma once

#include <windows.h>
#include <tlhelp32.h>


namespace pld {

	SIZE_T EnumerateProcesses(PROCESSENTRY32 * iter, SIZE_T containerSize);

	HANDLE GetProcessHandle(const CHAR * processName, DWORD * processId = nullptr, DWORD desiredAccess = PROCESS_ALL_ACCESS);
	HANDLE GetProcessHandle(DWORD processId, DWORD desiredAccess = PROCESS_ALL_ACCESS);

	LPVOID GetModuleBaseAddress(HANDLE processHandle, const CHAR * moduleName);

	BYTE * GetProcessBaseAddress(DWORD processId, const CHAR * processName);
}

#endif //GUARD_PROCLOAD_H