#pragma once


// SetDailyTaskDlg 对话框

class SetDailyTaskDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SetDailyTaskDlg)

public:
	SetDailyTaskDlg(int dlgType, int index, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SetDailyTaskDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SET_DAILYTASK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	int dlgType;
	int index;

	BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	int mRefSec;
	int mCurValue;
	int mMaxValue;
	int hour;
	int minute;
	int second;
	bool bConfirm;
	BOOL bDailyRef;
	CString strHour;
	CString strMinute;
	CString strSecond;
	CString strCurValue;
	CString strMaxValue;
	CString mName;
	afx_msg void OnConfirm();
	afx_msg void OnCancel();
	afx_msg void OnDailyRefreshChanged();
	BOOL bNoAlert;
};
