#pragma once


// SetTask 对话框

class SetTaskDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SetTaskDlg)

public:
	SetTaskDlg(int dlgType, int index, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SetTaskDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SET_TASK };
#endif

	BOOL OnInitDialog();
	int dlgType;
	int index;
	bool bConfirm;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString mName;
	CString mDetail;
	CString strYear;
	CString strMonth;
	CString strDay;
	int mYear;
	int mMonth;
	int mDay;
	int mHour;
	int mMinute;
	CTime ddl;
	BOOL mDateUnknown;
	BOOL mDone;
	afx_msg void Confirm();
	afx_msg void Cancel();
	afx_msg void dateUnknownClicked();
	CString strHour;
	CString strMinute;
};
