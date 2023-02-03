// SetDailyTaskDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "StudyWork.h"
#include "SetDailyTaskDlg.h"
#include "afxdialogex.h"
#include "public.h"


// SetDailyTaskDlg 对话框

IMPLEMENT_DYNAMIC(SetDailyTaskDlg, CDialogEx)

SetDailyTaskDlg::SetDailyTaskDlg(int dlgType, int index, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SET_DAILYTASK, pParent)
	, bDailyRef(FALSE)
	, dlgType(dlgType)
	, index(index)
	, strHour(_T("0"))
	, strMinute(_T("0"))
	, strSecond(_T("0"))
	, strCurValue(_T("0"))
	, strMaxValue(_T("1"))
	, mName(_T(""))
	, bNoAlert(FALSE)
{

}

SetDailyTaskDlg::~SetDailyTaskDlg()
{
}

void SetDailyTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, bDailyRef);
	DDX_Text(pDX, IDC_EDIT6, strHour);
	DDX_Text(pDX, IDC_EDIT7, strMinute);
	DDX_Text(pDX, IDC_EDIT8, strSecond);
	DDX_Text(pDX, IDC_EDIT2, strCurValue);
	DDX_Text(pDX, IDC_EDIT3, strMaxValue);
	DDX_Text(pDX, IDC_EDIT1, mName);
	DDX_Check(pDX, IDC_CHECK2, bNoAlert);
}

BOOL SetDailyTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DailyTask dtask;
	switch (dlgType) {
	case NEW_DAILY:
		this->SetWindowTextW(L"添加任务");
		break;
	case CHANGE_DAILY:
		this->SetWindowTextW(L"修改任务");
		dtask = data::mdailyTaskList[index];
		mName = dtask.name;
		bDailyRef = dtask.bDailyRef;
		bNoAlert = dtask.bNoAlert;
		strHour = int2str(dtask.refSec / 3600);
		strMinute = int2str((dtask.refSec % 3600) / 60);
		if (bDailyRef) {
			GetDlgItem(IDC_EDIT2)->EnableWindow(false);
			GetDlgItem(IDC_EDIT3)->EnableWindow(false);
			GetDlgItem(IDC_EDIT8)->EnableWindow(false);
		}
		else {
			strSecond = int2str(dtask.refSec % 60);
			strCurValue = int2str(dtask.getCurValue());
			strMaxValue = int2str(dtask.maxValue);
		}
		UpdateData(false);
		break;
	case VIEW_DAILY:
		this->SetWindowTextW(L"查看任务");
		dtask = data::mdailyTaskList[index];
		mName = dtask.name;
		bDailyRef = dtask.bDailyRef;
		bNoAlert = dtask.bNoAlert;
		strHour = int2str(dtask.refSec / 3600);
		strMinute = int2str((dtask.refSec % 3600) / 60);
		GetDlgItem(IDC_EDIT1)->EnableWindow(false);
		GetDlgItem(IDC_EDIT2)->EnableWindow(false);
		GetDlgItem(IDC_EDIT3)->EnableWindow(false);
		GetDlgItem(IDC_EDIT6)->EnableWindow(false);
		GetDlgItem(IDC_EDIT7)->EnableWindow(false);
		GetDlgItem(IDC_EDIT8)->EnableWindow(false);
		GetDlgItem(IDC_CHECK1)->EnableWindow(false);
		GetDlgItem(IDC_CHECK2)->EnableWindow(false);
		GetDlgItem(IDC_BUTTON8)->EnableWindow(false);
		if (!bDailyRef){
			strSecond = int2str(dtask.refSec % 60);
			strCurValue = int2str(dtask.getCurValue());
			strMaxValue = int2str(dtask.maxValue);
		}
		UpdateData(false);
		break;
	case UPDATE_DAILY:		
		this->SetWindowTextW(L"更新数据");
		dtask = data::mdailyTaskList[index];
		mName = dtask.name;
		bDailyRef = dtask.bDailyRef;
		bNoAlert = dtask.bNoAlert;
		GetDlgItem(IDC_EDIT1)->EnableWindow(false);
		GetDlgItem(IDC_EDIT3)->EnableWindow(false);
		GetDlgItem(IDC_EDIT6)->EnableWindow(false);
		GetDlgItem(IDC_EDIT7)->EnableWindow(false);
		GetDlgItem(IDC_EDIT8)->EnableWindow(false);
		GetDlgItem(IDC_CHECK1)->EnableWindow(false);
		GetDlgItem(IDC_CHECK2)->EnableWindow(false);
		if (!bDailyRef) {
			strHour = int2str(dtask.refSec / 3600);
			strMinute = int2str((dtask.refSec % 3600) / 60);
			strSecond = int2str(dtask.refSec % 60);
			strCurValue = int2str(dtask.getCurValue());
			strMaxValue = int2str(dtask.maxValue);
		}
		else {
			MessageBox(L"在UPDATE_DAILY阶段出现错误的每日刷新任务");
			EndDialog(0);
		}
		UpdateData(false);
		break;
	default:
		MessageBox(L"Wrong code in SetTaskDlg:\n    " + int2str(dlgType));
		EndDialog(0);
		break;
	}
	return TRUE;
}


BEGIN_MESSAGE_MAP(SetDailyTaskDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &SetDailyTaskDlg::OnConfirm)
	ON_BN_CLICKED(IDC_BUTTON8, &SetDailyTaskDlg::OnCancel)
	ON_BN_CLICKED(IDC_CHECK1, &SetDailyTaskDlg::OnDailyRefreshChanged)
END_MESSAGE_MAP()


// SetDailyTaskDlg 消息处理程序


void SetDailyTaskDlg::OnConfirm()
{
	// TODO: 在此添加控件通知处理程序代码
	if (dlgType == VIEW_DAILY) {
		EndDialog(0);
	}
	UpdateData(true);
	if (mName.GetLength() == 0)
	{
		MessageBox(L"请输入任务名称");
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
	hour = str2int(strHour);
	minute = str2int(strMinute);
	if (minute > 59) {
		MessageBox(L"分钟超出范围(0-59)");
		return;
	}
	if (!bDailyRef) {
		if (strCurValue.GetLength() == 0)
		{
			MessageBox(L"请输入当前堆叠数");
			return;
		}
		if (strMaxValue.GetLength() == 0)
		{
			MessageBox(L"请输入堆叠最大数");
			return;
		}
		if (strSecond.GetLength() == 0)
		{
			MessageBox(L"请输入秒数");
			return;
		}
		mCurValue = str2int(strCurValue);
		mMaxValue = str2int(strMaxValue);
		second = str2int(strSecond);
		if (mMaxValue==0) {
			MessageBox(L"最大堆叠数不能为0");
			return;
		}
		if (mCurValue > mMaxValue) {
			MessageBox(L"当前堆叠数大于最大堆叠数，已被置为最大堆叠数");
			mCurValue = mMaxValue;
		}
		if (second > 59) {
			MessageBox(L"秒数超出范围(0-59)");
			return;
		}
		mRefSec = hour * 3600 + minute * 60 + second;
		if (mRefSec == 0) {
			MessageBox(L"堆叠时间不得为0");
			return;
		}
	}
	else {
		if (hour > 23) {
			MessageBox(L"小时超出范围(0-23)");
			return;
		}
		mRefSec = hour * 3600 + minute * 60;
	}
	bConfirm = true;
	EndDialog(0);
}


void SetDailyTaskDlg::OnCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	bConfirm = false;
	EndDialog(0);
}


void SetDailyTaskDlg::OnDailyRefreshChanged()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if (bDailyRef) {
		GetDlgItem(IDC_EDIT2)->EnableWindow(false);
		GetDlgItem(IDC_EDIT3)->EnableWindow(false);
		GetDlgItem(IDC_EDIT8)->EnableWindow(false);
	}
	else {
		GetDlgItem(IDC_EDIT2)->EnableWindow(true);
		GetDlgItem(IDC_EDIT3)->EnableWindow(true);
		GetDlgItem(IDC_EDIT8)->EnableWindow(true);
	}
}
