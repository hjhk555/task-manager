
// StudyWorkDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "StudyWork.h"
#include "StudyWorkDlg.h"
#include "SetDailyTaskDlg.h"
#include "afxdialogex.h"
#include "public.h"
#include "Task.h"
#include "DailyTask.h"
#include "SetTaskDlg.h"
#include "LogDlg.h"
#include <fstream>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 线程函数
UINT timer(LPVOID pParam);
UINT detectSitting(LPVOID pParam);
UINT leftAlert(LPVOID pParam);
UINT preLeftAlert(LPVOID pParam);

// CStudyWorkDlg 对话框

CStudyWorkDlg::CStudyWorkDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STUDYWORK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CStudyWorkDlg::~CStudyWorkDlg()
{
#ifdef DEBUG
	delete ldlg;
#endif // DEBUG
}

void CStudyWorkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, taskList);
	//  DDX_Text(pDX, IDC_SIT_TIME, sitTip);
}

BEGIN_MESSAGE_MAP(CStudyWorkDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CStudyWorkDlg::Save)
	ON_BN_CLICKED(IDC_BUTTON2, &CStudyWorkDlg::AddTask)
	ON_LBN_SELCHANGE(IDC_LIST1, &CStudyWorkDlg::changeSelect)
	ON_BN_CLICKED(IDC_BUTTON3, &CStudyWorkDlg::ViewTask)
	ON_BN_CLICKED(IDC_BUTTON4, &CStudyWorkDlg::DeleteTask)
	ON_BN_CLICKED(IDC_BUTTON5, &CStudyWorkDlg::Finish)
	ON_BN_CLICKED(IDC_BUTTON7, &CStudyWorkDlg::ChangeTask)
	ON_BN_CLICKED(IDC_BUTTON6, &CStudyWorkDlg::DeleteAllDone)
	ON_LBN_DBLCLK(IDC_LIST1, &CStudyWorkDlg::onDoubleClick)
	ON_MESSAGE(UPDATE_TIME,&CStudyWorkDlg::updateTime)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON9, &CStudyWorkDlg::ForceExit)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_DAILY_SHOW, &CStudyWorkDlg::OnDailyShow)
	ON_BN_CLICKED(IDC_BUTTON8, &CStudyWorkDlg::OnAbout)
	ON_BN_CLICKED(IDC_BUTTON10, &CStudyWorkDlg::showTaskCondition)
	ON_BN_CLICKED(IDC_BUTTON_PAUSEALERT, &CStudyWorkDlg::setAlertPause)
	ON_BN_CLICKED(IDC_LOG, &CStudyWorkDlg::OnBnClickedLog)
END_MESSAGE_MAP()


// CStudyWorkDlg 消息处理程序

BOOL CStudyWorkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

#ifdef DEBUG
	ldlg = new LogDlg;
	ldlg->Create(IDD_LOG, this);
	data::ldlg = ldlg;
#else
	GetDlgItem(IDC_BUTTON9)->ShowWindow(false);
	GetDlgItem(IDC_LOG)->ShowWindow(false);
#endif

	//进程唯一
	HANDLE hMap;
	hMap = CreateFileMapping((HANDLE)
		0xFFFFFFFF, NULL, PAGE_READWRITE, 0, 100, _T("Task Manager"));
	if (hMap != NULL && (GetLastError() == ERROR_ALREADY_EXISTS))
	{
		CloseHandle(hMap);
		hMap = NULL;
		exit(0);
	}

	data::tTime = CTime::GetTickCount();
	alertStartTime = data::tTime.Format("%H:%M");
	CString curT = data::tTime.Format(L"%Y年%m月%d日 %H:%M");
	GetDlgItem(IDC_TIME)->SetWindowText(curT);

	bSitting = false;
	tSitStart = data::tTime;
	AfxBeginThread(detectSitting, this);

	readData();
	giveAlert();
	AfxBeginThread(timer, this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CStudyWorkDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CStudyWorkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CStudyWorkDlg::readData()
{
	bool doneFile = false;
	//检查备份文件
	ifstream fin(DEFAULT_BACKUP, ios::in);
	bool noFile = !fin;	//无文件
	fin.close();
	if (!noFile) {
		if (MessageBox(L"检测到上一次程序异常退出导致数据损失。\n请问是否需要恢复数据？", L"数据恢复确认@任务管理器", MB_YESNO | MB_DEFBUTTON1|MB_ICONWARNING) == IDYES) {
			readFile(_T(DEFAULT_BACKUP));
			doneFile = true;
		}
	}
	//读取文件
	if (!doneFile) {
		ifstream fin(DEFAULT_FILE, ios::in);
		noFile = !fin;
		fin.close();
		if (!noFile){
			readFile(_T(DEFAULT_FILE));
		}
	}
	update();
}

void CStudyWorkDlg::readFile(CString filename)
{
	CFile file(filename, CFile::modeRead);
	CArchive ar(&file, CArchive::load);
	int n;
	ar >> n;
	Task temp;
	for (int i = 0; i < n; i++)
	{
		temp.Serialize(ar);
		data::mtaskList.push_back(temp);
	}
	ar >> n;
	DailyTask dtemp;
	for (int i = 0; i < n; i++) {
		dtemp.Serialize(ar);
		data::mdailyTaskList.push_back(dtemp);
	}
}

void CStudyWorkDlg::writeFile(CString filename)
{
	ofstream fout(filename, ios::out);
	fout.close();
	CFile file(filename, CFile::modeWrite);
	CArchive ar(&file, CArchive::store);
	ar << data::mtaskList.size();
	for (UINT i = 0; i < data::mtaskList.size(); i++) data::mtaskList[i].Serialize(ar);
	ar << data::mdailyTaskList.size();
	for (UINT i = 0; i < data::mdailyTaskList.size(); i++) data::mdailyTaskList[i].Serialize(ar);
}


void CStudyWorkDlg::update()
{
	data::sort();
	taskList.ResetContent();
	if (bShowDaily) {
		for (UINT i = 0; i < data::mdailyTaskList.size(); i++) taskList.AddString(data::mdailyTaskList[i].getName());
	}
	else {
		for (UINT i = 0; i < data::mtaskList.size(); i++) taskList.AddString(data::mtaskList[i].getName());
	}

	sitTimeUpdate();

	CString tip;
	if (bPauseAlert) {
		int tLeft = pauseTime - (data::tTime - tPause).GetMinutes();
		if (tLeft <= 0) {
			setAlertPause();
		}
		tip = L"警告已暂停，将在" + int2str(tLeft) + L"分钟后恢复警告";
	}
	if (!bPauseAlert) {
		bAlert = data::getTip(tip, taskStatus);
		bAlert = bAlert || bSitAlert;

		if (oldAlert != bAlert) {
			alertStartTime = data::tTime.Format("%H:%M");
			alertCount = 0;
		}

		oldAlert = bAlert;
	}
	GetDlgItem(IDC_TIP)->SetWindowText(tip);
	UpdateData(false);
	Invalidate(true);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON5)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON7)->EnableWindow(false);
}

void CStudyWorkDlg::viewDailyTask()
{
	int index = taskList.GetCurSel();
	if (index != CB_ERR) {
		SetDailyTaskDlg sdtDlg(VIEW_DAILY, index);
		sdtDlg.DoModal();
	}
}

void CStudyWorkDlg::addDailyTask()
{
	SetDailyTaskDlg sdtDlg(NEW_DAILY,0);
	sdtDlg.DoModal();
	if (sdtDlg.bConfirm) {
		DailyTask temp(sdtDlg.mName,sdtDlg.mMaxValue,sdtDlg.bDailyRef,sdtDlg.mRefSec,sdtDlg.bNoAlert);
		if (temp.bDailyRef) {
			temp.recTime = CTime::GetTickCount() - CTimeSpan(1, 0, 0, 0);
		}
		else {
			temp.recTime = CTime::GetTickCount();
			temp.recValue = sdtDlg.mCurValue;
		}
		data::mdailyTaskList.push_back(temp);
		update();
		writeFile(_T(DEFAULT_BACKUP));
	}
}

void CStudyWorkDlg::changeDailyTask()
{
	int index = taskList.GetCurSel();
	if (index != CB_ERR) {
		SetDailyTaskDlg sdtDlg(CHANGE_DAILY, index);
		sdtDlg.DoModal();
		if (sdtDlg.bConfirm) {
			DailyTask temp(sdtDlg.mName, sdtDlg.mMaxValue, sdtDlg.bDailyRef, sdtDlg.mRefSec, sdtDlg.bNoAlert);
			if (temp.bDailyRef) {
				temp.recTime = CTime::GetTickCount() - CTimeSpan(1, 0, 0, 0);
			}
			else {
				temp.recTime = CTime::GetTickCount();
				temp.recValue = sdtDlg.mCurValue;
			}
			data::mdailyTaskList[index] = temp;
			update();
			writeFile(_T(DEFAULT_BACKUP));
		}
	}
}

void CStudyWorkDlg::deleteDailyTask()
{
	int index = taskList.GetCurSel();
	if (index != CB_ERR) {
		if (MessageBox(L"您确定要删除该任务吗?\n任务名称：" + data::mdailyTaskList[index].name, L"删除确认", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			for (UINT i = index; i < data::mdailyTaskList.size() - 1; i++) {
				data::mdailyTaskList[i] = data::mdailyTaskList[i + 1];
			}
			data::mdailyTaskList.pop_back();
			update();
			writeFile(_T(DEFAULT_BACKUP));
		}
	}
}

void CStudyWorkDlg::recordDailyTask()
{
	int index = taskList.GetCurSel();
	if (index != CB_ERR) {
		DailyTask& dtemp = data::mdailyTaskList[index];
		if (dtemp.bDailyRef) {
			if (!dtemp.getCurValue()) {
				MessageBox(L"该任务已经完成，请等待明日刷新");
				return;
			}
			if (MessageBox(L"您确定该任务已完成吗?\n任务名称：" + dtemp.name, L"完成确认", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
				dtemp.recTime = CTime::GetTickCount();
				update();
				writeFile(_T(DEFAULT_BACKUP));
			}
		}
		else if (dtemp.maxValue == 1) {
			if (dtemp.getCurValue() == 1) {
				if (MessageBox(L"您确定该任务已完成吗?\n任务名称：" + dtemp.name, L"完成确认", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
					dtemp.recTime = CTime::GetTickCount();
					dtemp.recValue = 0;
					update();
					writeFile(_T(DEFAULT_BACKUP));
				}
			}
			else {
				MessageBox(L"该任务正在冷却，请等待下次刷新");
				return;
			}
		}else{
			SetDailyTaskDlg sdtDlg(UPDATE_DAILY, index);
			sdtDlg.DoModal();
			if (sdtDlg.bConfirm) {
				dtemp.recTime = CTime::GetTickCount();
				dtemp.recValue = sdtDlg.mCurValue;
				update();
				writeFile(_T(DEFAULT_BACKUP));
			}
		}
	}
}

void CStudyWorkDlg::doneDailyTask()
{
	CString warn = L"您确定已完成下列每日刷新的任务吗?\n注：该操作对勿扰任务无效\n";
	bool notnull = false;
	for (UINT i = 0; i < data::mdailyTaskList.size(); i++) {
		DailyTask& dtemp = data::mdailyTaskList[i];
		if (!dtemp.bNoAlert&&dtemp.bDailyRef&&dtemp.getCurValue() == 1) {
			warn += L"\t" + dtemp.name + L"\n";
			notnull = true;
		}
	}
	if (!notnull) {
		MessageBox(L"所有未勿扰的每日刷新任务均已完成。");
		return;
	}
	if (MessageBox(warn, L"完成确认", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
	{
		for (UINT i = 0; i < data::mdailyTaskList.size(); i++) {
			DailyTask& dtemp= data::mdailyTaskList[i];
			if (!dtemp.bNoAlert&&dtemp.bDailyRef&&dtemp.getCurValue()==1) {
				dtemp.recTime = CTime::GetTickCount();
			}
		}
		update();
		writeFile(_T(DEFAULT_BACKUP));
	}
}

bool CStudyWorkDlg::getSitTip(CString& sitTip, int& sitStatus)
{
	if (bSitting) {
		CTime curT = CTime::GetTickCount();
		CTimeSpan sitTime = curT - tSitStart;
		CTimeSpan leftTime = curT - tLeftStart;
		if (leftTime >= CTimeSpan(0, 0, 1, 0)) {
			sitTip = L"您已离开电脑" + int2str(leftTime.GetTotalMinutes()) + L"分钟";
			sitStatus = 1;
		}
		else {
			sitTip = L"您已经连续使用电脑" + int2str(sitTime.GetTotalHours()) + L"小时" + int2str(sitTime.GetMinutes()) + L"分钟";
			if (sitTime.GetTotalMinutes() >= maxSitTime) sitStatus = 2; else sitStatus = 1;
		}
		if (sitTime.GetTotalMinutes() >= maxSitTime) {
			//data::addLog(L"sitip: true");
			return true;
		}
	}
	else {
		sitTip = L"当前未使用电脑";
		sitStatus = 0;
	}
	//data::addLog(L"sitip: false");
	return false;
}

void CStudyWorkDlg::sitTimeUpdate()
{
	bool oldSitAlert = bSitAlert;
	bSitAlert = getSitTip(sitTip, sitStatus);
	if (oldSitAlert != bSitAlert) {
		//data::addLog(L"alertCount reset by sitAlert change");
		alertCount = 0;
	}
	GetDlgItem(IDC_SIT_TIME)->SetWindowText(sitTip);
}

bool CStudyWorkDlg::getSitDlgTip(CString& res)
{
	res = L"";
	CTimeSpan sitTime = CTime::GetTickCount() - tSitStart;
	if (bSitting&&sitTime.GetTotalMinutes() >= maxSitTime) {
		res = L"您已经连续使用电脑" + int2str(sitTime.GetTotalHours()) + L"小时" + int2str(sitTime.GetMinutes())
			+ L"分钟，请起身并适当活动！\n注：您需要离开电脑" + int2str(maxLeftTime) + L"分钟以解除警告。\n";
		return true;
	}
	return false;
}

void CStudyWorkDlg::AddTask()
{
	// TODO: 在此添加控件通知处理程序代码
	if (bShowDaily) {
		addDailyTask();
		return;
	}
	SetTaskDlg STDlg(NEW_TASK,0);
	STDlg.DoModal();
	if (STDlg.bConfirm)
	{
		Task temp(STDlg.mName, STDlg.mDetail, !STDlg.mDateUnknown);
		if (temp.dateknown)
		{
			temp.ddl = STDlg.ddl;
		}
		data::mtaskList.push_back(temp);
		update();
		writeFile(_T(DEFAULT_BACKUP));
	}
}


void CStudyWorkDlg::changeSelect()
{
	// TODO: 在此添加控件通知处理程序代码
	if (taskList.GetCurSel() != CB_ERR)
	{
		GetDlgItem(IDC_BUTTON3)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON4)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON5)->EnableWindow(true);
		GetDlgItem(IDC_BUTTON7)->EnableWindow(true);
	}
}


void CStudyWorkDlg::ViewTask()
{
	// TODO: 在此添加控件通知处理程序代码
	if (bShowDaily) {
		viewDailyTask();
		return;
	}
	if (taskList.GetCurSel() != CB_ERR)
	{
		SetTaskDlg STDlg(VIEW_TASK, taskList.GetCurSel());
		STDlg.DoModal();
	}
}


void CStudyWorkDlg::DeleteTask()
{
	// TODO: 在此添加控件通知处理程序代码
	if (bShowDaily) {
		deleteDailyTask();
		return;
	}
	if (taskList.GetCurSel() != CB_ERR)
	{
		int index = taskList.GetCurSel();
		CString warn = L"您确定要删除该任务吗?\n任务名称：" + data::mtaskList[index].name;
		if (!data::mtaskList[index].done) warn += L"\n\n警告：该任务尚未完成！";
		if (MessageBox(warn,L"删除确认",MB_OKCANCEL|MB_DEFBUTTON2)==IDOK)
		{
			for (UINT i = index+1; i < data::mtaskList.size(); i++) data::mtaskList[i-1] = data::mtaskList[i];
			data::mtaskList.pop_back();
			update();
			writeFile(_T(DEFAULT_BACKUP));
		}
	}
}


void CStudyWorkDlg::Finish()
{
	// TODO: 在此添加控件通知处理程序代码
	if (bShowDaily) {
		recordDailyTask();
		return;
	}
	if (taskList.GetCurSel() != CB_ERR)
	{
		int index = taskList.GetCurSel();
		if (data::mtaskList[index].done)
		{
			MessageBox(L"该任务已完成");
			return;
		}
		CString warn = L"您确定该任务已经完成吗?\n任务名称：" + data::mtaskList[index].name;
		if (MessageBox(warn, L"任务完成确认", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			data::mtaskList[index].done = true;
			update();
			writeFile(_T(DEFAULT_BACKUP));
		}
	}
}


void CStudyWorkDlg::ChangeTask()
{
	// TODO: 在此添加控件通知处理程序代码
	if (bShowDaily) {
		changeDailyTask();
		return;
	}
	if (taskList.GetCurSel() != CB_ERR)
	{
		int index = taskList.GetCurSel();
		SetTaskDlg STDlg(CHANGE_TASK, index);
		STDlg.DoModal();
		if (STDlg.bConfirm)
		{
			data::mtaskList[index].name = STDlg.mName;
			data::mtaskList[index].detail = STDlg.mDetail;
			data::mtaskList[index].ddl = STDlg.ddl;
			data::mtaskList[index].dateknown = !STDlg.mDateUnknown;
			data::mtaskList[index].done = STDlg.mDone;
			update();
			writeFile(_T(DEFAULT_BACKUP));
		}
	}
}


void CStudyWorkDlg::DeleteAllDone()
{
	// TODO: 在此添加控件通知处理程序代码
	if (bShowDaily) {
		doneDailyTask();
		return;
	}
	CString warn = L"您确定要删除所有已完成的任务吗?";
	if (MessageBox(warn, L"删除确认",MB_OKCANCEL|MB_DEFBUTTON2)==IDOK)
	{
		UINT pt = 0;
		while (pt < data::mtaskList.size())
		{
			if (data::mtaskList[pt].done)
			{
				for (UINT i = pt + 1; i < data::mtaskList.size(); i++) data::mtaskList[i - 1] = data::mtaskList[i];
				data::mtaskList.pop_back();
			}
			else pt++;
		}
		update();
		writeFile(_T(DEFAULT_BACKUP));
	}
}

void CStudyWorkDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CString warn = L"要在退出前保存文件吗？";
	int res = MessageBox(warn, L"退出确认", MB_YESNOCANCEL | MB_DEFBUTTON3);
	if (res == IDCANCEL) {
		return;
	}
	if (res==IDYES) {
		writeFile();
	}
	if (res == IDNO) {
		warn = L"不保存将会导致已进行的修改失效！\n您确定要不保存并退出吗？";
		if (MessageBox(warn, L"警告", MB_YESNO | MB_DEFBUTTON2) == IDNO) return;
	}
	DeleteFile(_T(DEFAULT_BACKUP));
	EndDialog(0);
}

void CStudyWorkDlg::Save()
{
	CString warn = L"您确定要保存文件吗？";
	if (MessageBox(warn, L"保存确认", MB_YESNO|MB_DEFBUTTON2)==IDYES)
	{
		writeFile();
		//MessageBox(L"文件保存成功。");
		DeleteFile(_T(DEFAULT_BACKUP));
	}
}

void CStudyWorkDlg::ForceExit()
{
	// TODO: 在此添加控件通知处理程序代码
	CString warn = L"您确定要强制退出吗？\n\n强制退出不会保存所做的更改。\n请勿在非特殊情况下使用强制退出。";
	if (MessageBox(warn, L"强制退出确认", MB_YESNO | MB_DEFBUTTON2) == IDYES) EndDialog(0);
}

void CStudyWorkDlg::onDoubleClick()
{
	if (bShowDaily) {
		recordDailyTask();
	}
	else {
		ViewTask();
	}
}

UINT timer(LPVOID pParam)
{
	CStudyWorkDlg* parent = (CStudyWorkDlg*)pParam;
	CTime curTime = CTime::GetTickCount();
	Sleep((60 - curTime.GetSecond()) * 1000);
	parent->PostMessage(UPDATE_TIME);
	return 0;
}

UINT detectSitting(LPVOID pParam)
{
	CStudyWorkDlg* parent = (CStudyWorkDlg*)pParam;
	CTime curT;
	CPoint Cpt;
	CTimeSpan leftTime;
	CWinThread *leftAlertThread{}, *preLeftAlertThread{};
	bool preSit = false;
	bool leftCount = false;
	//常量获取
	const CTimeSpan maxLeftTime(0, 0, parent->maxLeftTime, 0);
	const CTimeSpan preLeftTime = maxLeftTime - CTimeSpan(0, 0, 0, parent->preLeftSecond);
	const CTimeSpan leftCountStart(0, 0, 1, 0);
	const int maxCursorDiff = parent->maxCursorDiff;

	GetCursorPos(&Cpt);
	int preCurPosX = Cpt.x;
	int preCurPosY = Cpt.y;

	while (true) {
		curT = CTime::GetTickCount();
		GetCursorPos(&Cpt);
		if (abs(preCurPosX - Cpt.x) > maxCursorDiff || abs(preCurPosY - Cpt.y) > maxCursorDiff) {
			//data::addLog(L"移动");
			parent->tLeftStart = curT;
			if (!parent->bSitting) {
				parent->bSitting = true;
				parent->tSitStart = curT;
				endThread(leftAlertThread);
				parent->sitTimeUpdate();
			}
			if (leftCount) parent->sitTimeUpdate();
			leftCount = false;
			if (preSit) endThread(preLeftAlertThread);
			preSit = false;
		}else if (parent->bSitting) {
			leftTime = curT - parent->tLeftStart;
			//data::addLog(L"无移动 " + int2str(leftTime.GetTotalSeconds())); 
			if (leftTime >= leftCountStart) {
				if (!leftCount) parent->sitTimeUpdate();
				leftCount = true;
			}
			if (leftTime >= maxLeftTime) {
				parent->bSitting = false;
				/* 警告解除提示（已停用）
				if (preSit) endThread(preLeftAlertThread);
				preSit = false;
				leftAlertThread = AfxBeginThread(leftAlert, parent);
				*/
				parent->sitTimeUpdate();
			}
			/* 警告解除预提示（已禁用）
			else if (!preSit && leftTime >= preLeftTime) {
				preLeftAlertThread = AfxBeginThread(preLeftAlert, parent);
				preSit = true;
			}
			*/
		}
		//data::addLog(preSit ? L" preSit: true" : L" preSit: false");
		//data::addLog(leftCount ? L" leftCount: true" : L" leftCount: false");
		preCurPosX = Cpt.x;
		preCurPosY = Cpt.y;
		Sleep(1000);
	}
	return 0;
}

UINT leftAlert(LPVOID pParam)
{
	CStudyWorkDlg* parent = (CStudyWorkDlg*)pParam;
	::MessageBox(NULL, L"您已离开电脑" + int2str(parent->maxLeftTime) + L"分钟，久坐警告解除", L"警告解除", MB_ICONINFORMATION | MB_SYSTEMMODAL);
	return 0;
}

UINT preLeftAlert(LPVOID pParam)
{
	CStudyWorkDlg* parent = (CStudyWorkDlg*)pParam;
	::MessageBox(NULL, L"久坐警告将在"+int2str(parent->preLeftSecond) + L"秒后解除\n若您正坐在电脑前，请及时进行鼠标操作以继续久坐计时", L"警告即将解除", MB_ICONINFORMATION | MB_SYSTEMMODAL);
	return 0;
}

UINT alert(LPVOID pParam) {
	CString dlgTip, sitDlgTip;
	CStudyWorkDlg* mainDlg = (CStudyWorkDlg*)pParam;
	bool common = data::getDlgTip(dlgTip), sit = mainDlg->getSitDlgTip(sitDlgTip);
	if (common || sit) {
		::MessageBox(NULL, L"首次警告时间："+mainDlg->alertStartTime+data::tTime.Format(L"\n本次警告时间：%H:%M\n\n")+dlgTip+sitDlgTip, L"任务警告", MB_ICONWARNING | MB_SYSTEMMODAL);
	}
	return 0;
}

LRESULT CStudyWorkDlg::updateTime(WPARAM wParam, LPARAM lParam)
{
	data::tTime = CTime::GetTickCount();
	CString curT = data::tTime.Format(L"%Y年%m月%d日 %H:%M");
	GetDlgItem(IDC_TIME)->SetWindowText(curT);
	update();
	giveAlert();
	AfxBeginThread(timer, this);
	return 0;
}



HBRUSH CStudyWorkDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	if (pWnd->GetDlgCtrlID() == IDC_TIP) {
		if (taskStatus==2||bPauseAlert) {
			pDC->SetTextColor(RGB(255, 0, 0));
		}
		else if (taskStatus == 1) {
			pDC->SetTextColor(RGB(180, 120, 0));
		}
		else {
			pDC->SetTextColor(RGB(0, 200, 0));
		}
	}

	if (pWnd->GetDlgCtrlID() == IDC_SIT_TIME) {
		if (sitStatus == 2) {
			pDC->SetTextColor(RGB(255, 0, 0));
		}
		else if (sitStatus == 1) {
			pDC->SetTextColor(RGB(180, 120, 0));
		}
		else {
			pDC->SetTextColor(RGB(0, 200, 0));
		}
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CStudyWorkDlg::OnDailyShow()
{
	// TODO: 在此添加控件通知处理程序代码
	bShowDaily = !bShowDaily;
	if (bShowDaily) {
		GetDlgItem(IDC_BUTTON2)->SetWindowTextW(L"添加日常任务");
		GetDlgItem(IDC_STATIC4)->SetWindowTextW(L"日常任务");
		GetDlgItem(IDC_DAILY_SHOW)->SetWindowTextW(L"显示一般任务");
		GetDlgItem(IDC_BUTTON5)->SetWindowTextW(L"更新状态/完成");
		GetDlgItem(IDC_BUTTON6)->SetWindowTextW(L"完成每日任务");
	}
	else {
		GetDlgItem(IDC_BUTTON2)->SetWindowTextW(L"添加一般任务");
		GetDlgItem(IDC_STATIC4)->SetWindowTextW(L"一般任务");
		GetDlgItem(IDC_DAILY_SHOW)->SetWindowTextW(L"显示日常任务");
		GetDlgItem(IDC_BUTTON5)->SetWindowTextW(L"标记为已完成");
		GetDlgItem(IDC_BUTTON6)->SetWindowTextW(L"删除已完成");
	}
	update();
}

BOOL CStudyWorkDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return true;  //将ESC键的消息屏蔽 
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CStudyWorkDlg::giveAlert()
{
	if (!bAlert) {
		endThread(alertThread);
	}
	else if (!bPauseAlert) {
		//data::addLog(L"alertCount: " + int2str(alertCount));
		if (alertCount == 0) {
			endThread(alertThread);
			alertThread=AfxBeginThread(alert, this);
			alertCount = maxAlertCount;
		}
		alertCount--;
	}
}

void CStudyWorkDlg::OnAbout()
{
	// TODO: 在此添加控件通知处理程序代码
	CString mess = L"该程序基于MFC制作\n制作人：黄嘉铧\n更新日期：2022/1/15";
	MessageBox(mess, L"关于", MB_OK|MB_ICONINFORMATION);
}

void CStudyWorkDlg::showTaskCondition()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox(data::getAllTaskTip(), L"任务情报", MB_OK|MB_ICONINFORMATION);
}


void CStudyWorkDlg::setAlertPause()
{
	// TODO: 在此添加控件通知处理程序代码
	if (bPauseAlert) {
		bPauseAlert = false;
		alertCount = 0;
		GetDlgItem(IDC_BUTTON_PAUSEALERT)->SetWindowTextW(L"暂停警告");
	}
	else {
		bPauseAlert = true;
		tPause = CTime::GetTickCount();
		GetDlgItem(IDC_BUTTON_PAUSEALERT)->SetWindowTextW(L"恢复警告");
	}
	update();
}


void CStudyWorkDlg::OnBnClickedLog()
{
#ifdef DEBUG
	ldlg->ShowWindow(true);
#endif // DEBUG
}
