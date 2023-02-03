#pragma once
#include"stdafx.h"
#include"Task.h"
#include"DailyTask.h"
#include"utils.h"
#include"LogDlg.h"
#include<vector>
using std::vector;

#define DEFAULT_FILE "data.tl"
#define DEFAULT_BACKUP "data_backup.tl"

#define UPDATE_TIME 000

#define NEW_TASK 100
#define VIEW_TASK 101
#define CHANGE_TASK 102

#define NEW_DAILY 200
#define CHANGE_DAILY 201
#define VIEW_DAILY 202
#define UPDATE_DAILY 203

#define UPDATE_LOG 300

class data
{
private:
	static const CTimeSpan oneAlert;	//一级警告
	static const CTimeSpan twoAlert;		//二级警告
	static const CTimeSpan threeAlert;	//三级警告
	static int getLevel(DailyTask& t);
public:
	static vector<Task> mtaskList;
	static vector<DailyTask> mdailyTaskList;
	static CTime tTime;
	static CString log;
	static LogDlg* ldlg;

	static void sort();
	static bool compare(Task& a, Task& b);	//a比b优先
	static bool compare(DailyTask& a, DailyTask& b);	//a比b优先
	static CString taskEmerge(CTime& ddl);
	static bool getTip(CString& res, int& taskStatus);
	static bool getDlgTip(CString& res);
	static CString getAllTaskTip();
	static void addLog(CString mess);
};