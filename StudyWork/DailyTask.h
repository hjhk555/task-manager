#pragma once
#include"stdafx.h"

class DailyTask :public CObject {
public:
	CString name;
	int maxValue;
	int recValue;
	CTime recTime;
	int refSec;
	bool bDailyRef;
	bool bNoAlert;

	DailyTask()
	{
		name = L"";
		bDailyRef = false;
		bNoAlert = false;
	}
	DailyTask(const DailyTask& t);
	DailyTask& operator=(const DailyTask&);
	DailyTask(CString name, int maxValue, bool bDailyRef, int refSec, bool bNoAlert) {
		this->name = name;
		this->maxValue = maxValue;
		this->bDailyRef = bDailyRef;
		this->refSec = refSec;
		this->bNoAlert = bNoAlert;
	}
	CString getName();
	void Serialize(CArchive& ar);
	int getCurValue();
	CTime getFillTime();
};