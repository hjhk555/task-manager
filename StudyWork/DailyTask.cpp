#include "stdafx.h"
#include "DailyTask.h"
#include "public.h"

DailyTask::DailyTask(const DailyTask & t)
{
		name = t.name;
		maxValue = t.maxValue;
		recValue = t.recValue;
		recTime = t.recTime;
		refSec = t.refSec;
		bDailyRef = t.bDailyRef;
		bNoAlert = t.bNoAlert;
}

DailyTask & DailyTask::operator=(const DailyTask & t)
{
	name = t.name;
	maxValue = t.maxValue;
	recValue = t.recValue;
	recTime = t.recTime;
	refSec = t.refSec;
	bDailyRef = t.bDailyRef;
	bNoAlert = t.bNoAlert;
	return *this;
}

CString DailyTask::getName()
{
	CString res = L"";
	int cur=getCurValue();
	if (bNoAlert) {
		res += L"[����]";
	}
	if (bDailyRef) {
		if (cur) {
			res += L"[δ���]";
		}
		else {
			res += L"[�����]";
		}
		res += L"[ÿ��" + int2str(refSec / 3600,2) + L":" + int2str((refSec % 3600) / 60,2) + L"ˢ��]" + name;
	}
	else {
		if (cur >= maxValue) {
			res += L"[������]"+name+L"[" + int2str(maxValue) + L"/" + int2str(maxValue) + L"]" ;
		}
		else {
			res += L"[��ȴ��]" + getFillTime().Format(L"[%d��%H:%M����]") +name+ L"[" + int2str(cur) + L"/" + int2str(maxValue) + L"]";
		}
	}
	return res;
}

void DailyTask::Serialize(CArchive & ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << name << maxValue << recValue << recTime << refSec << bDailyRef << bNoAlert;
	}
	else
	{
		ar >> name >> maxValue >> recValue >> recTime >> refSec >> bDailyRef >> bNoAlert;
	}
}

int DailyTask::getCurValue()
{
	CTime curT = data::tTime;
	if (bDailyRef) {
		if (recTime < getFillTime() - CTimeSpan(1, 0, 0, 0)) return 1;
		return 0;
	}
	CTimeSpan tsPass = data::tTime - recTime;
	int tPass=(int)tsPass.GetTotalSeconds();
	int cur = recValue + tPass / refSec;
	if (cur < 0) cur = 0;
	if (cur > maxValue) cur = maxValue;
	return cur;
}

CTime DailyTask::getFillTime()
{
	if (!bDailyRef) {
		CTimeSpan addTime = CTimeSpan((maxValue - recValue)*refSec);
		return recTime + addTime;
	}
	else {
		CTime curT = data::tTime;
		CTime refTime(curT.GetYear(), curT.GetMonth(), curT.GetDay(), refSec / 3600, (refSec % 3600) / 60, 0);
		if (curT >= refTime) {
			refTime += CTimeSpan(1, 0, 0, 0);
		}
		return refTime;
	}
}
