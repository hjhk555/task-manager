// SetTask.cpp: 实现文件
//

#include "stdafx.h"
#include "StudyWork.h"
#include "SetTaskDlg.h"
#include "afxdialogex.h"
#include "public.h"


// SetTask 对话框

IMPLEMENT_DYNAMIC(SetTaskDlg, CDialogEx)

SetTaskDlg::SetTaskDlg(int dlgType, int index, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SET_TASK, pParent)
	, dlgType(dlgType)
	, index(index)
	, mName(_T(""))
	, mDetail(_T(""))
	, strYear(_T(""))
	, strMonth(_T(""))
	, strDay(_T(""))
	, mDateUnknown(FALSE)
	, mDone(FALSE)
	, strHour(_T(""))
	, strMinute(_T(""))
{

}

SetTaskDlg::~SetTaskDlg()
{
}

BOOL SetTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	switch (dlgType)
	{
	case NEW_TASK:
		GetDlgItem(IDC_CHECK3)->EnableWindow(false);
		this->SetWindowText(L"添加任务");
		strYear = int2str(data::tTime.GetYear());
		strMonth = int2str(data::tTime.GetMonth());
		strDay = int2str(data::tTime.GetDay());
		strHour = L"0";
		strMinute = L"0";
		UpdateData(false);
		break;
	case VIEW_TASK:
		GetDlgItem(IDC_EDIT1)->EnableWindow(false);
		((CEdit*)GetDlgItem(IDC_EDIT2))->SetReadOnly(true);
		GetDlgItem(IDC_EDIT3)->EnableWindow(false);
		GetDlgItem(IDC_EDIT4)->EnableWindow(false);
		GetDlgItem(IDC_EDIT5)->EnableWindow(false);
		GetDlgItem(IDC_HOUR)->EnableWindow(false);
		GetDlgItem(IDC_MINUTE)->EnableWindow(false);
		GetDlgItem(IDC_CHECK2)->EnableWindow(false);
		GetDlgItem(IDC_CHECK3)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
		mName = data::mtaskList[index].name;
		mDetail = data::mtaskList[index].detail;
		if (data::mtaskList[index].dateknown)
		{
			ddl = data::mtaskList[index].ddl;
			strYear = int2str(ddl.GetYear());
			strMonth = int2str(ddl.GetMonth());
			strDay = int2str(ddl.GetDay());
			strHour = int2str(ddl.GetHour());
			strMinute = int2str(ddl.GetMinute());
		}
		else mDateUnknown = true;
		if (data::mtaskList[index].done) mDone = true;
		UpdateData(false);
		GetDlgItem(IDC_EDIT2)->PostMessage(EM_SETSEL, 0, 0);
		this->SetWindowText(L"查看任务");
		break;
	case CHANGE_TASK:
		mName = data::mtaskList[index].name;
		mDetail = data::mtaskList[index].detail;
		if (data::mtaskList[index].dateknown)
		{
			ddl = data::mtaskList[index].ddl;
			strYear = int2str(ddl.GetYear());
			strMonth = int2str(ddl.GetMonth());
			strDay = int2str(ddl.GetDay());
			strHour = int2str(ddl.GetHour());
			strMinute = int2str(ddl.GetMinute());
		}
		else
		{
			mDateUnknown = true;
			strYear = int2str(data::tTime.GetYear());
			strMonth = int2str(data::tTime.GetMonth());
			strDay = int2str(data::tTime.GetDay());
			strHour = L"0";
			strMinute = L"0";
			GetDlgItem(IDC_EDIT3)->EnableWindow(false);
			GetDlgItem(IDC_EDIT4)->EnableWindow(false);
			GetDlgItem(IDC_EDIT5)->EnableWindow(false);
			GetDlgItem(IDC_HOUR)->EnableWindow(false);
			GetDlgItem(IDC_MINUTE)->EnableWindow(false);
		}
		if (data::mtaskList[index].done) mDone = true;
		UpdateData(false);
		this->SetWindowText(L"修改任务");
		break;
	default:
		MessageBox(L"Wrong code in SetTaskDlg:\n    " + int2str(dlgType));
		EndDialog(0);
		break;
	}
	return TRUE;
}

void SetTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, mName);
	DDX_Text(pDX, IDC_EDIT2, mDetail);
	DDX_Text(pDX, IDC_EDIT3, strYear);
	DDX_Text(pDX, IDC_EDIT4, strMonth);
	DDX_Text(pDX, IDC_EDIT5, strDay);
	DDX_Check(pDX, IDC_CHECK2, mDateUnknown);
	DDX_Check(pDX, IDC_CHECK3, mDone);
	DDX_Text(pDX, IDC_HOUR, strHour);
	DDX_Text(pDX, IDC_MINUTE, strMinute);
}


BEGIN_MESSAGE_MAP(SetTaskDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &SetTaskDlg::Confirm)
	ON_BN_CLICKED(IDC_BUTTON2, &SetTaskDlg::Cancel)
	ON_BN_CLICKED(IDC_CHECK2, &SetTaskDlg::dateUnknownClicked)
END_MESSAGE_MAP()


// SetTask 消息处理程序


void SetTaskDlg::Confirm()
{
	// TODO: 在此添加控件通知处理程序代码
	if (dlgType == VIEW_TASK) {
		EndDialog(0);
	}
	UpdateData(true);
	if (mName.GetLength() == 0)
	{
		MessageBox(L"请输入任务名称");
		return;
	}
	if (!mDateUnknown)
	{
		if (strYear.GetLength() == 0)
		{
			MessageBox(L"请输入年份");
			return;
		}
		if (strMonth.GetLength() == 0)
		{
			MessageBox(L"请输入月份");
			return;
		}
		if (strDay.GetLength() == 0)
		{
			MessageBox(L"请输入日数");
			return;
		}
		if (strHour.GetLength() == 0)
		{
			MessageBox(L"请输入小时");
			return;
		}
		if (strMinute.GetLength() == 0)
		{
			MessageBox(L"请输入分钟");
			return;
		}
		mYear = str2int(strYear);
		if (mYear < 2000) 
		{
			MessageBox(L"请输入2000年后的年份");
			return;
		}
		mMonth = str2int(strMonth);
		mDay = str2int(strDay);
		mHour = str2int(strHour);
		mMinute = str2int(strMinute);
		int days[13] = { 0,31,28,31,30,31,30,31,31,30,31,30,31 };
		if (mYear % 400 == 0
			|| mYear % 100 != 0 && mYear % 4 == 0) days[2] = 29;
		if (mMonth < 1 || mMonth>12)
		{
			MessageBox(L"月份超出范围(1-12)");
			return;
		}
		if (mDay < 1 || mDay>days[mMonth])
		{
			CString dayLimit;
			dayLimit.Format(L"%d", days[mMonth]);
			MessageBox(L"日数超出范围(1-" + dayLimit + L")");
			return;
		}
		if (mHour>23) {
			MessageBox(L"小时超出范围(0-23)");
			return;
		}
		if (mMinute>59) {
			MessageBox(L"分钟超出范围(0-59)");
			return;
		}
		ddl = CTime(mYear, mMonth, mDay, mHour, mMinute, 0);
	}
	bConfirm = true;
	EndDialog(0);
}


void SetTaskDlg::Cancel()
{
	// TODO: 在此添加控件通知处理程序代码
	bConfirm = false;
	EndDialog(0);
}


void SetTaskDlg::dateUnknownClicked()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (mDateUnknown)
	{
		GetDlgItem(IDC_EDIT3)->EnableWindow(false);
		GetDlgItem(IDC_EDIT4)->EnableWindow(false);
		GetDlgItem(IDC_EDIT5)->EnableWindow(false);
		GetDlgItem(IDC_HOUR)->EnableWindow(false);
		GetDlgItem(IDC_MINUTE)->EnableWindow(false);
	}
	else 
	{
		GetDlgItem(IDC_EDIT3)->EnableWindow(true);
		GetDlgItem(IDC_EDIT4)->EnableWindow(true);
		GetDlgItem(IDC_EDIT5)->EnableWindow(true);
		GetDlgItem(IDC_HOUR)->EnableWindow(true);
		GetDlgItem(IDC_MINUTE)->EnableWindow(true);
	}
}
