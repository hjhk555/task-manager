#pragma once
#include "stdafx.h"
CString int2str(int x, int len = -1);
int str2int(CString str);

DWORD getThreadStatus(CWinThread* thread);
bool endThread(CWinThread* thread);