#include "stdafx.h"
#include "public.h"
#include "Task.h"

Task::Task(const Task& t)
{
	name = t.name;
	detail = t.detail;
	ddl = t.ddl;
	dateknown = t.dateknown;
	done = t.done;
}

Task& Task::operator=(const Task &t)
{
	name = t.name;
	detail = t.detail;
	ddl = t.ddl;
	dateknown = t.dateknown;
	done = t.done;
	return *this;
}

CString Task::getName()
{
	CString Name = L"";
	if (done)
	{
		Name += L"[✔已完成]";
	}
	else
	{
		Name += L"["+(dateknown?data::taskEmerge(ddl):L"")+L"未完成][";
		if (!dateknown) Name += L"到期时间未定]";
		else Name += ddl.Format(L"%Y年%m月%d日%H:%M到期]");
	}
	Name += name;
	return Name;
}

void Task::Serialize(CArchive & ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << name << detail << ddl << dateknown << done;
	}
	else
	{
		ar >> name >> detail >> ddl >> dateknown >> done;
	}
}
