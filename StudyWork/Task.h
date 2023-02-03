#pragma once
#include"stdafx.h"

class Task:public CObject
{
public:
	CString name;
	CString detail;
	CTime ddl;
	bool dateknown;
	bool done;
	Task()
	{
		name =detail= L"";
		dateknown = done = false;
	}
	Task(const Task&);
	Task& operator=(const Task&);
	Task(CString Name, CString Detail, bool Dateknown)
	{
		name = Name;
		detail = Detail;
		dateknown = Dateknown;
		done = false;
	}
	CString getName();
	void Serialize(CArchive& ar);
};