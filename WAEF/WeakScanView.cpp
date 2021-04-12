// WeakScanView.cpp : 实现文件
//

#include "stdafx.h"
#include "WAEF.h"
#include "WeakScanView.h"



// CWeakScanView

CListCtrl CWeakScanView::_scanlist;
CWeakScanMdl CWeakScanView::_weak_scan_mdl;
int	CWeakScanView::_itemCount;

IMPLEMENT_DYNCREATE(CWeakScanView, CFormView)

CWeakScanView::CWeakScanView()
	: CFormView(CWeakScanView::IDD)
{
	_first = true;
	_itemCount=0;
}

CWeakScanView::~CWeakScanView()
{
}

void CWeakScanView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCAN_LIST, _scanlist);
	DDX_Control(pDX, IDC_WEAK_LOGLIST, _loglist);
}

BEGIN_MESSAGE_MAP(CWeakScanView, CFormView)
	ON_BN_CLICKED(IDC_SCAN_START, &CWeakScanView::OnBnClickedScanStart)
	ON_BN_CLICKED(IDC_WEAK_REPORT, &CWeakScanView::OnBnClickedWeakReport)
	ON_NOTIFY(NM_RCLICK, IDC_SCAN_LIST, &CWeakScanView::OnRclickScanList)
	ON_LBN_DBLCLK(IDC_WEAK_LOGLIST, &CWeakScanView::OnLbnDblclkWeakLoglist)
	ON_COMMAND(ID_SCAN_CLEAR_LIST, &CWeakScanView::OnScanClearList)
END_MESSAGE_MAP()


// CWeakScanView 诊断

#ifdef _DEBUG
void CWeakScanView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CWeakScanView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWeakScanView 消息处理程序

void CWeakScanView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	if(_first)
	{
		_scanlist.InsertColumn(0,_T("URL列表"),LVCFMT_LEFT,510);
		_scanlist.InsertColumn(1,_T("扫描进度"),LVCFMT_LEFT,132);
		_scanlist.InsertColumn(2,_T("状态"),LVCFMT_LEFT,100);
		_scanlist.InsertColumn(3,_T("发现漏洞数"),LVCFMT_LEFT,100);
	//	_scanlist.SetItemState(0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		_scanlist.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		_weak_scan_mdl.init(&_scanlist, &_loglist);

		
		httpproxy.initproxy();
		httpproxy.setopt_port(8088);
		httpproxy.setopt_owner(GetSafeHwnd());
		httpproxy.setopt_notify(NotifyProc);
		httpproxy.setopt_mode(httpproxy_mode_getrequest);
		httpproxy.startproxy();

		_first = false;
	}
}

bool CWeakScanView::CompareUrl(CString url1, CString url2)
{
	int pos1=0, pos2;
	while((pos1=url1.Find('=',pos1+1))!=-1)
	{
		if((pos2 = url1.Find('&', pos1))==-1)
			url1.Delete(pos1+1, url1.GetLength()-pos1-1);
		else
			url1.Delete(pos1+1,pos2-pos1-1);
	}
	while((pos1=url2.Find('=',pos1+1))!=-1)
	{
		if((pos2 = url2.Find('&', pos1))==-1)
			url2.Delete(pos1+1, url2.GetLength()-pos1-1);
		else
			url2.Delete(pos1+1,pos2-pos1-1);
	}
	return url1 == url2;
}

int CWeakScanView::AddScanUrl(pstruct_scan_item pscan_item)
{
	int i=0;
	int count=_scanlist.GetItemCount();
	while(i<count)
	{
		if(CompareUrl(_scanlist.GetItemText(i,0),pscan_item->url.c_str()))
		{
			delete pscan_item;
			return 0;
		}
		i++;
	}
	pscan_item->idx = _itemCount;
	_scanlist.InsertItem(_itemCount, pscan_item->url.c_str());
	_scanlist.SetItemData(_itemCount++, (DWORD_PTR)pscan_item);
	_weak_scan_mdl.init_schedule(pscan_item);
	_weak_scan_mdl.update_item_state(pscan_item);

	return 0;
}

void CWeakScanView::OnBnClickedScanStart()
{
	// TODO: 在此添加控件通知处理程序代码
	_weak_scan_mdl.setopt_thread(10);
	_weak_scan_mdl.start();
}


void CWeakScanView::OnBnClickedWeakReport()
{
	// TODO: 在此添加控件通知处理程序代码
	HWND hParent = this->GetParent()->GetSafeHwnd();
	::PostMessage(hParent,MSG_SWITCH_VIEW, WEAK_REPORT_VIEW,0);

	int i;
	int itemCount = _scanlist.GetItemCount();
	for(i=0;i<itemCount;i++)
	{
		::SendMessage(hParent, MSG_ADDREPORT_ITEM, (WPARAM)_scanlist.GetItemText(i, 0).GetBuffer() ,(LPARAM)_scanlist.GetItemData(i));
	}
}


BOOL CWeakScanView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	// 重写PreTranslateMessage(MSG* pMsg), 否则无法响应Ctrl+V等消息
	if( IsDialogMessage( pMsg ) )
		return TRUE;
	else
		return CFormView::PreTranslateMessage(pMsg);
	//return CFormView::PreTranslateMessage(pMsg);
}


void CWeakScanView::OnRclickScanList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	CMenu	popup;
	CMenu*	pM;
	popup.LoadMenu(IDR_MENU_SCANPOP);
	pM = popup.GetSubMenu(0);

	CPoint	p;
	GetCursorPos(&p);

	int nItem=phdr->iItem;
	if(nItem==-1)
	{
		pM->EnableMenuItem(0, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		pM->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		pM->EnableMenuItem(3, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	}

	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);

	*pResult = 0;
}


void CWeakScanView::OnLbnDblclkWeakLoglist()
{
	// TODO: 在此添加控件通知处理程序代码
	CString url;
	_loglist.GetText(_loglist.GetCurSel(),url);
	int nPos = url.Find(' ', url.Find(' ')+1);
	if(nPos>0)
	{
		url = url.Mid(nPos+1, url.Find(' ', nPos+1)-nPos-1);
		ShellExecute(NULL, NULL,url, NULL, NULL, SW_SHOWNORMAL);
	}
}

LRESULT CALLBACK CWeakScanView::NotifyProc(string& request, httpproxy_mode mode)
{
	if(mode == httpproxy_mode_getrequest)
	{
		int pos1, pos2;
		target_method tmethod;
		string method, action, host, cookie, postdata;
		pos1 = request.find(' ');
		if(pos1 ==-1)
			return 0;
		pos2 = request.find(' ', pos1+5);
		if(pos2 ==-1)
			return 0;
		method = request.substr(0, pos1);
		if(method == "GET")
			tmethod = target_method_get;
		else if(method == "POST")
			tmethod = target_method_post;
		else
			return 0;
		action = request.substr(5, pos2-5);
		pos1 = request.find("Host: "), pos2;
		if(pos1 != -1)
		{
			pos2 = request.find("\r\n", pos1);
			host = request.substr(pos1+6, pos2-pos1-6);
			action = "http://" + host + action;
		}

		pos1 = request.find("Cookie: ");
		if(pos1 != -1)
		{
			pos2 = request.find("\r\n", pos1);
			cookie = request.substr(pos1+8, pos2-pos1-8);
		}

		if(tmethod == target_method_post)
		{
			pos1 = request.find("\r\n\r\n");
			if(pos1 != -1)
			{
				postdata = request.substr(pos1+4);
			}
		}

		url_type type;
		if(action.find(".php")!=-1)
			type = url_type_php;
		else if(action.find(".asp")!=-1)
			type = url_type_asp;
		else if(action.find(".aspx")!=-1)
			type = url_type_aspx;
		else if(action.find(".jsp")!=-1)
			type = url_type_jsp;
		else
			type = url_type_other;

		pstruct_scan_item pscan_item;
		if(tmethod == target_method_get)
		{
			pscan_item = new struct_scan_item(type, action, cookie);
			pscan_item->param = true;
		}
		else
		{
			pscan_item = new struct_scan_item(type, action, cookie, postdata);
			pscan_item->param = false;
		}
		pscan_item->method=tmethod;
		AddScanUrl(pscan_item);
	}
	return 0;
}

void CWeakScanView::OnScanClearList()
{
	// TODO: 在此添加命令处理程序代码
	int i=0;
	pstruct_scan_item pscan_item;
	int count=_scanlist.GetItemCount();
	while(i<count)
	{
		pscan_item = (pstruct_scan_item)_scanlist.GetItemData(i);
		FreeScanItem(pscan_item);
		i++;
	}
	_scanlist.DeleteAllItems();
}

void CWeakScanView::FreeScanItem(pstruct_scan_item pscan_item)
{
	pstruct_schedule_item pschedule_next, pschedule_item = pscan_item->pschedule_item;
	while(pschedule_item!=nullptr)
	{
		pschedule_next = pschedule_item->pNext;
		delete pschedule_item;
		pschedule_item = pschedule_next;
	}

	pstruct_weakness_item pweakness_next, pweakness_item = pscan_item->pweakness_item;
	while(pweakness_item!=nullptr)
	{
		pweakness_next = pweakness_item->pNext;
		delete pweakness_item;
		pweakness_item = pweakness_next;
	}
	delete pscan_item;
}