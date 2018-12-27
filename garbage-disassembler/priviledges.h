#ifndef GUARD_PRIVILEDGES_H
#define GUARD_PRIVILEDGES_H
#pragma once

#include <windows.h>


namespace pvlg {
	BOOL acquirePrivilegeByName(const TCHAR * szPrivilegeName);
}

#endif //GUARD_PRIVILEDGES_H