
// StudyWorkDlg.h: 头文件
//

#include"public.h"
#include"utils.h"
#include"LogDlg.h"

#pragma once

// CStudyWorkDlg 对话框
class CStudyWorkDlg : public CDialogEx
{
// 构造
public:
	CStudyWorkDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CStudyWorkDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STUDYWORK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
// 数据
	const UINT maxAlertCount = 5;	//警告间隔 分钟
	const int pauseTime = 30;		//暂停时间 分钟
	const int maxSitTime = 60;		//久坐提醒 分钟
	const int maxLeftTime = 10;		//离开提示 分钟
	const int preLeftSecond = 10;	//离开前提示 秒
	const int maxCursorDiff = 1;	//鼠标检测误差 像素

	CFile file;
	int taskStatus = 0;
	bool bAlert = true;
	bool bShowDaily = false;
	int alertCount = 0;
	bool bPauseAlert = false;
	CTime tPause;
	CWinThread* alertThread{};
	CString alertStartTime;

	bool bSitting = false;
	bool bSitAlert = false;
	int sitStatus = 0;
	CTime tSitStart, tLeftStart;
	CString sitTip;
	bool oldAlert = false;

	LogDlg *ldlg;

// 函数
	void readData();
	void readFile(CString filename);
	void writeFile(CString filename = _T(DEFAULT_FILE));
	void update();
	void viewDailyTask();
	void addDailyTask();
	void changeDailyTask();
	void deleteDailyTask();
	void recordDailyTask();
	void doneDailyTask();
	bool getSitTip(CString& sitTip, int& sitStatus);
	void sitTimeUpdate();
	bool getSitDlgTip(CString& res);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void giveAlert();
	CListBox taskList;
	afx_msg void AddTask();
	afx_msg void changeSelect();
	afx_msg void ViewTask();
	afx_msg void DeleteTask();
	afx_msg void Finish();
	afx_msg void ChangeTask();
	afx_msg void DeleteAllDone();
	afx_msg void OnClose();
	afx_msg void Save();
	afx_msg void ForceExit();
	afx_msg void onDoubleClick();
	afx_msg LRESULT updateTime(WPARAM wParam, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDailyShow();
//	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnAbout();
	afx_msg void showTaskCondition();
	afx_msg void setAlertPause();
	afx_msg void OnBnClickedLog();
};
