#include "priviledges.h"


BOOL pvlg::acquirePrivilegeByName(const TCHAR * szPrivilegeName) {
	HANDLE				htoken;
	TOKEN_PRIVILEGES	tkp;
	DWORD				dwerr;

	//---------------- adjust process token privileges to grant privilege
	if (szPrivilegeName == NULL) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL, szPrivilegeName, &(tkp.Privileges[0].Luid)))
		return FALSE;

	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &htoken))
		return FALSE;

	if (!AdjustTokenPrivileges(htoken, FALSE, &tkp, 0, NULL, NULL) || GetLastError() != ERROR_SUCCESS) {   // may equal ERROR_NOT_ALL_ASSIGNED
		dwerr = GetLastError();
		CloseHandle(htoken);
		SetLastError(dwerr);
		return FALSE;
	}

	CloseHandle(htoken);
	SetLastError(ERROR_SUCCESS);

	return TRUE;
}