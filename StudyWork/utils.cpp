#include "stdafx.h"
#include "utils.h"

CString int2str(int x, int len)
{
	CString ans, style = L"%d";
	if (len >= 0) style.Format(L"%%.%dd", len);
	ans.Format(style, x);
	return ans;
}

int str2int(CString str)
{
	int ans = 0;
	for (int i = 0; i < str.GetLength(); i++) ans = ans * 10 + str[i] - L'0';
	return ans;
}

DWORD getThreadStatus(CWinThread* thread)
{
	if (thread == NULL) return NULL;
	DWORD code;
	bool res = GetExitCodeThread(thread->m_hThread, &code);
	if (!res) return NULL;
	return code;
}

bool endThread(CWinThread* thread)
{
	if (getThreadStatus(thread)==STILL_ACTIVE)
		thread->PostThreadMessage(WM_QUIT, NULL, NULL);
	return true;
}