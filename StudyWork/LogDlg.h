#pragma once
#include "afxdialogex.h"


// LogDlg 对话框

class LogDlg : public CDialogEx
{
	DECLARE_DYNAMIC(LogDlg)

public:
	LogDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~LogDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString log;
protected:
	afx_msg LRESULT OnUpdateLog(WPARAM wParam, LPARAM lParam);
public:
	CEdit logEdit;
};
