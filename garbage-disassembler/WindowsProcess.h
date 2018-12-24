#ifndef GUARD_WINDOWS_PROCESS_H
#define GUARD_WINDOWS_PROCESS_H
#pragma once

#include <string>
#include <windows.h>
#include <tlhelp32.h>

class WindowsProcess {

public:


	WindowsProcess(const PROCESSENTRY32 &);




	bool OpenHandle();

	
	bool LoadModule(const std::string &);




	const std::string & getProcessName() const;

	DWORD getProcessId() const;
	
	const PROCESSENTRY32 & getProcessInfo() const;


private:


	HANDLE pHandle;
	MODULEENTRY32 mod;
	
	std::string pName;
	DWORD pId;
	PROCESSENTRY32 pInfo;
};

#endif //GUARD_WINDOWS_PROCESS_H