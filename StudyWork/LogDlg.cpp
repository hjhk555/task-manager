// LogDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "StudyWork.h"
#include "afxdialogex.h"
#include "LogDlg.h"
#include "public.h"

// LogDlg 对话框

IMPLEMENT_DYNAMIC(LogDlg, CDialogEx)

LogDlg::LogDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOG, pParent)
	, log(_T(""))
{
	
}

LogDlg::~LogDlg()
{
}

void LogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, log);
	DDX_Control(pDX, IDC_EDIT, logEdit);
}


BEGIN_MESSAGE_MAP(LogDlg, CDialogEx)
	ON_MESSAGE(UPDATE_LOG, &LogDlg::OnUpdateLog)
END_MESSAGE_MAP()


// LogDlg 消息处理程序


LRESULT LogDlg::OnUpdateLog(WPARAM wParam, LPARAM lParam)
{
	log = data::log;
	UpdateData(false);
	logEdit.LineScroll(logEdit.GetLineCount());
	return 0;
}
