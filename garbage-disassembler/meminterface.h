#ifndef GUARD_MEM_INTERFACE_H
#define GUARD_MEM_INTERFACE_H
#pragma once

#include <windows.h>


class MemInterface {
public:

	MemInterface(DWORD processId);
	MemInterface(const CHAR * processName);

	~MemInterface();

private:

};

#endif //GUARD_MEM_INTERFACE_H