#include "stdafx.h"
#include "public.h"

vector<Task> data::mtaskList;
vector<DailyTask> data::mdailyTaskList;
CTime data::tTime = CTime::GetTickCount();
CString data::log = L"";
LogDlg* data::ldlg;
const CTimeSpan data::oneAlert = CTimeSpan(7, 0, 0, 0);
const CTimeSpan data::twoAlert = CTimeSpan(3, 0, 0, 0);
const CTimeSpan data::threeAlert = CTimeSpan(1, 0, 0, 0);

bool data::getTip(CString & res, int& taskStatus)
{
	bool bAlert;
	res = L"";
	int passed = 0, danger = 0, daily = 0;
	int sz = mtaskList.size();
	bAlert = false;
	CTime ddl;
	for (int i = 0; i < sz; i++) {
		if (!mtaskList[i].done&&mtaskList[i].dateknown)
		{
			ddl = mtaskList[i].ddl;
			if (ddl <= tTime) {
				passed++;
				bAlert = true;
			}
			else if (ddl - tTime <= twoAlert)
				danger++;
		}
	}
	sz = mdailyTaskList.size();
	for (int i = 0; i < sz; i++) {
		DailyTask& dtask = mdailyTaskList[i];
		if (dtask.bDailyRef&&dtask.getCurValue()
			|| !dtask.bDailyRef&&dtask.getCurValue() == dtask.maxValue) {
			daily++;
			if (!dtask.bNoAlert) {
				bAlert = true;
			}
		}
	}
	if (passed == 0 && danger == 0 && daily == 0) {
		res = L"任务进度正常";
		taskStatus = 0;
		return bAlert;
	}
	if (passed) res += int2str(passed) + L"个任务到期 ";
	if (danger) res += int2str(danger) + L"个任务紧急 ";
	if (daily) res += int2str(daily) + L"个日常待办 ";
	if (bAlert) taskStatus = 2; else taskStatus = 1;
	return bAlert;
}

bool data::getDlgTip(CString& res)
{
	res = L"";
	bool passed = false, daily = false;
	CTime ddl;
	int sz = mtaskList.size();
	for (int i = 0; i < sz; i++) {
		if (!mtaskList[i].done&&mtaskList[i].dateknown)
		{
			ddl = mtaskList[i].ddl;
			if (ddl <= tTime) {
				if (!passed) {
					res += L"已到期的任务：\n";
					passed = true;
				}
				res += L"\t" + mtaskList[i].name+L"\n";
			}
		}
	}
	sz = mdailyTaskList.size();
	for (int i = 0; i < sz; i++) {
		DailyTask& dtask = mdailyTaskList[i];
		if (!dtask.bNoAlert &&(
			dtask.bDailyRef&&dtask.getCurValue() || 
			!dtask.bDailyRef&&dtask.getCurValue() == dtask.maxValue)){
			if (!daily) {
				res += L"日常待办任务：\n";
				daily = true;
			}
			res += L"\t" + mdailyTaskList[i].name + L"\n";
		}
	}
	if (passed || daily) res += L"\n";
	return (passed||daily);
}

CString data::getAllTaskTip()
{
	CString res = L"";
	bool danger = false, passed = false, daily = false;
	CString strDanger = L"", strPassed = L"", strDaily = L"";
	CTime ddl;
	int sz = mtaskList.size();
	for (int i = 0; i < sz; i++) {

	}
	for (int i = 0; i < sz; i++) {
		if (!mtaskList[i].done&&mtaskList[i].dateknown)
		{
			ddl = mtaskList[i].ddl;
			if (ddl <= tTime) {
				passed = true;
				strPassed += L"\t" + mtaskList[i].name + L"\n";
			}
			else if (ddl - tTime <= twoAlert) {
				danger = true;
				strDanger += L"\t" + mtaskList[i].name + L"\n";
			}
		}
	}
	sz = mdailyTaskList.size();
	for (int i = 0; i < sz; i++) {
		DailyTask& dtask = mdailyTaskList[i];
		if (dtask.bDailyRef&&dtask.getCurValue() ||
			!dtask.bDailyRef&&dtask.getCurValue() == dtask.maxValue) {
			daily = true;
			strDaily += L"\t" + mdailyTaskList[i].name + L"\n";
		}
	}
	if (!(danger || daily || passed)) return L"暂无需要注意的任务。";
	if (passed) res += L"已到期的任务：\n" + strPassed;
	if (danger) res += L"紧急的任务：\n" + strDanger;
	if (daily) res += L"日常待办任务：\n" + strDaily;
	return res;
}

void data::addLog(CString mess)
{
#ifdef DEBUG
	log += CTime::GetTickCount().Format(L"[%H:%M:%S]") + mess + L"\r\n";
	ldlg->PostMessage(UPDATE_LOG);
#endif // DEBUG
}

int data::getLevel(DailyTask & t)
{
	int ans;
	if (t.bDailyRef) {
		if (t.getCurValue() == 1) ans = 3;
		else ans = 0;
	}
	else {
		if (t.getCurValue() == t.maxValue) ans = 2;
		else ans = 1;
	}
	if (!t.bNoAlert) ans += 4;
	return ans;
}

void data::sort()
{
	int sz = mtaskList.size();
	Task temp;
	for(int i=0;i<sz-1;i++)
		for(int j=i+1;j<sz;j++)
			if (!compare(mtaskList[i], mtaskList[j]))
			{
				temp = mtaskList[i];
				mtaskList[i] = mtaskList[j];
				mtaskList[j] = temp;
			}
	sz = mdailyTaskList.size();
	DailyTask dtemp;
	for (int i = 0; i < sz - 1; i++)
		for (int j = i + 1; j < sz; j++)
			if (!compare(mdailyTaskList[i], mdailyTaskList[j]))
			{
				dtemp = mdailyTaskList[i];
				mdailyTaskList[i] = mdailyTaskList[j];
				mdailyTaskList[j] = dtemp;
			}
}

bool data::compare(Task & a, Task & b)
{
	if (a.done != b.done) {
		if (b.done) return true; else return false;
	}
	if (a.dateknown != b.dateknown) {
		if (a.dateknown) return true; else return false;
	}
	if (!a.done) {
		return a.ddl <= b.ddl;
	}
	return a.ddl >= b.ddl;
}

bool data::compare(DailyTask & a, DailyTask & b)
{
	int alvl = getLevel(a), blvl = getLevel(b);
	if (alvl == blvl) return a.getFillTime() <= b.getFillTime();
	return alvl >= blvl;
}

CString data::taskEmerge(CTime& ddl)
{
	if (ddl <= tTime) return L"⚠⚠";
	CTimeSpan tLeft = ddl - tTime;
	if (tLeft<=threeAlert) return L"★★★";
	if (tLeft<=twoAlert) return L"★★";
	if (tLeft<=oneAlert) return L"★";
	return L"";
}