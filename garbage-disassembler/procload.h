#ifndef GUARD_PROCLOAD_H
#define GUARD_PROCLOAD_H
#pragma once

#include <windows.h>
#include <tlhelp32.h>


namespace pld {

	SIZE_T EnumerateProcesses(PROCESSENTRY32 * iter, SIZE_T containerSize);

	HANDLE GetProcessHandle(const CHAR * processName, DWORD desiredAccess = PROCESS_ALL_ACCESS);
	HANDLE GetProcessHandle(DWORD processId, DWORD desiredAccess = PROCESS_ALL_ACCESS);

	DWORD GetModuleBaseAddress(HANDLE processHandle, const CHAR * moduleName);

}

#endif //GUARD_PROCLOAD_H