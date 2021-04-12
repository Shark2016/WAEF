// WeakScanView.cpp : 实现文件
//

#include "stdafx.h"
#include "WAEF.h"
#include "SiteSpiderView.h"
#include "Resource.h"

#include "WeakScanMdl.h"


// CSiteSpiderView

IMPLEMENT_DYNCREATE(CSiteSpiderView, CFormView)

HWND			CSiteSpiderView::_static_hWnd;
HWND			CSiteSpiderView::_hParenthWnd;
CTreeCtrl		CSiteSpiderView::m_TreeCtrl;
CListCtrl		CSiteSpiderView::m_SessionList;
CString			CSiteSpiderView::m_strCookie;
string			CSiteSpiderView::_exportlist;

CSiteSpiderView::CSiteSpiderView()
	: CFormView(CSiteSpiderView::IDD)
	, m_strTargetUrl(_T(""))
	, _deepth(0)
{
	/*
	m_ImgList.Create(GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		ILC_COLOR24, 50, 50);
	m_ImgList.SetBkColor(GetSysColor(COLOR_WINDOW)); 
	m_ImgList.Add(ExtractIcon(AfxGetApp()->m_hInstance, L"shell32.dll",6)); 
	m_ImgList.Add(ExtractIcon(AfxGetApp()->m_hInstance, L"shell32.dll",3)); 
	*/
	CWinApp* pApp = AfxGetApp();
	//m_ImgList.Create(16,16,ILC_COLOR, 2, 2);
	m_ImgList.Create(GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		ILC_COLOR24, 50, 50);
	m_ImgList.SetBkColor(GetSysColor(COLOR_WINDOW)); 
	int array_icon_id[] = {
		IDI_ICON_HOST,
		IDI_ICON_DIR,
		IDI_ICON_PHP,
		IDI_ICON_ASP, 
		IDI_ICON_ASPX, 
		IDI_ICON_JSP,
		IDI_ICON_HTML,
		IDI_ICON_PIC,
		IDI_ICON_JS,
		IDI_ICON_CSS,
		IDI_ICON_DOC,
		IDI_ICON_FILE,
		IDI_ICON_FILE,

		IDI_ICON_PARAM,
		IDI_ICON_PARAM,
		-1};
	int i=0;
	while(array_icon_id[i]!=-1)
	{
		m_ImgList.Add(pApp->LoadIcon(array_icon_id[i]));
		i++;
	}
	
	m_strCookie = _T("");
	_first = true;
}

CSiteSpiderView::~CSiteSpiderView()
{
}

void CSiteSpiderView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_RESULT, m_TreeCtrl);
	DDX_Text(pDX, IDC_TARGET_URL, m_strTargetUrl);
	//  DDX_CBIndex(pDX, IDC_COMBO_DEEP, _deepth);
	//  DDV_MinMaxInt(pDX, _deepth, 1, 10);
	DDX_CBIndex(pDX, IDC_COMBO_DEEP, _deepth);
	DDX_Control(pDX, IDC_SESSION_LIST, m_SessionList);
	DDX_Text(pDX, IDC_EDIT_COOKIE, m_strCookie);
}

BEGIN_MESSAGE_MAP(CSiteSpiderView, CFormView)
	ON_BN_CLICKED(IDC_SPIDER_START, &CSiteSpiderView::OnBnClickedSpiderStart)
	ON_BN_CLICKED(IDC_SPIDER_STOP, &CSiteSpiderView::OnBnClickedSpiderStop)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_RESULT, &CSiteSpiderView::OnRclickTreeResult)
	ON_BN_CLICKED(IDC_BTN_SET_SESSION, &CSiteSpiderView::OnBnClickedBtnSetSession)
	ON_COMMAND(ID_TREEPOP_OPEN_URL, &CSiteSpiderView::OnTreepopOpenUrl)
	ON_COMMAND(ID_TREEPOP_COPY_URL, &CSiteSpiderView::OnTreepopCopyUrl)
	ON_COMMAND(ID_TREEPOP_CHECK_NODE, &CSiteSpiderView::OnTreepopCheckNode)
	ON_COMMAND(ID_TREEPOP_CLEAR_NODE, &CSiteSpiderView::OnTreepopClearNode)
	ON_COMMAND(ID_TREEPOP_CLEAR_TREE, &CSiteSpiderView::OnTreepopClearTree)
	ON_NOTIFY(NM_DBLCLK, IDC_SESSION_LIST, &CSiteSpiderView::OnNMDblclkSessionList)
	ON_COMMAND(ID_TREEPOP_EXPORT_NODE, &CSiteSpiderView::OnTreepopExportNode)
END_MESSAGE_MAP()


// CSiteSpiderView 诊断

#ifdef _DEBUG
void CSiteSpiderView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSiteSpiderView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSiteSpiderView 消息处理程序

void CSiteSpiderView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	if(_first)
	{
		m_TreeCtrl.SetImageList(&m_ImgList,TVSIL_NORMAL); 
		_static_hWnd = this->GetSafeHwnd();
		_hParenthWnd = this->GetParent()->GetSafeHwnd();
		m_strTargetUrl = _T("http://test.shark.org");
		_deepth = 19;
		int i;
		CString str;
		CComboBox* ComboBox= (CComboBox*)GetDlgItem(IDC_COMBO_DEEP);
		for(i=1;i<=100;i++)
		{
			str.Format("%d", i);
			ComboBox->AddString(str);
		}
		m_spider.init();
		m_spider.setopt_owner(this);
		UpdateData(FALSE);

		
		m_SessionList.InsertColumn(0, "域名", LVCFMT_LEFT, 120);
		m_SessionList.InsertColumn(1, "COOKIE", LVCFMT_LEFT, 340);
		m_SessionList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

		httpproxy.initproxy();
		httpproxy.setopt_port(8090);
		httpproxy.setopt_owner(GetSafeHwnd());
		httpproxy.setopt_notify(NotifyProc);
		httpproxy.setopt_mode(httpproxy_mode_getcookie);
		httpproxy.startproxy();

		_first = false;
	}
	// TODO: 在此添加专用代码和/或调用基类
}

void CSiteSpiderView::OnBnClickedSpiderStart()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	string target(m_strTargetUrl.GetBuffer());
	m_spider.setopt_deepth(_deepth+1);
	m_spider.setopt_mode(mode_frombase);
	m_spider.setopt_baseurl(target);
	m_spider.setopt_notify(NotifyProc);
	m_spider.start();
}

void CSiteSpiderView::OnBnClickedSpiderStop()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_spider.is_running())
		m_spider.stop();
}

void CSiteSpiderView::OnRclickTreeResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//TV_DISPINFO* pTVDispinfo = (TV_DISPINFO*)pNMHDR;
	//HTREEITEM curhItem = pTVDispinfo->item.hItem;

	HTREEITEM  hSelectedItem = m_TreeCtrl.GetSelectedItem();

	CMenu	popup;
	popup.LoadMenu(IDR_MENU_TREEPOP);
	CMenu*	pM = popup.GetSubMenu(0);
	CPoint	p;
	GetCursorPos(&p);
	
	if(hSelectedItem == NULL)
	{
		int i=0;
		do
		{
			if(i!=2 && i!=5)
				pM->EnableMenuItem(i, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
			i++;
		}while(i<=6);
	}


	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);

	*pResult = 0;
}

CString CSiteSpiderView::GetTreeItemUrl(HTREEITEM hItem)
{
	int itemType;
	CString url, itemText;

	while(hItem != nullptr)
	{
		itemText = m_TreeCtrl.GetItemText(hItem);
		itemType = m_TreeCtrl.GetItemData(hItem) & 0x7F;
		switch(itemType)
		{
		case url_type_host:
		case url_type_dir:
			url = itemText + '/' + url;
			break;
		default:
			url = itemText + url;
		}
		hItem = m_TreeCtrl.GetParentItem(hItem);
	}
	return url;
}

void CSiteSpiderView::TraversalUrl(string preUrl, HTREEITEM hItem, url_type type, TRAV_NOTIFYPROC pNotify, bool notifyParent)
{
	int itemType;
	string url;
	CString itemText;
	bool notify = false, param = false;
	HTREEITEM hCurItem = m_TreeCtrl.GetChildItem(hItem);

	while(hCurItem!=nullptr)
	{
		itemType = m_TreeCtrl.GetItemData(hCurItem) & 0x7F;
		itemText = m_TreeCtrl.GetItemText(hCurItem);
		switch(itemType)
		{
		case url_type_host:
			url = itemText.GetBuffer() + '/';
			notify = notifyParent;
			break;
		case url_type_dir:
			url = preUrl + itemText.GetBuffer() + '/';
			notify = notifyParent;
			break;
		case url_type_param:
			url = preUrl + itemText.GetBuffer();
			param = true;
			break;
		case url_type_asp:
		case url_type_aspx:
		case url_type_jsp:
		case url_type_php:
		case url_type_html:
		case url_type_file:
		case url_type_pic:
		case url_type_js:
		case url_type_css:
		case url_type_unknow:
		case url_type_anchor:
		default:
			url = preUrl + itemText.GetBuffer();
		}
		
		HTREEITEM hChild = m_TreeCtrl.GetChildItem(hCurItem);
		if( hChild == nullptr || notify)
		{
			// Notify
			if(itemType == url_type_param || itemType == url_type_anchor)	//如果是参数或锚点，要获取真实的上级节点类型
			{
				if(type == url_type_param)
					itemType = m_TreeCtrl.GetItemData(m_TreeCtrl.GetParentItem(hItem)) & 0x7F;
				else
					itemType = type;
			}
			pNotify(url, (url_type)itemType, param);
		}
		
		if(hChild)
			TraversalUrl(url, hCurItem, (url_type)itemType, pNotify, notifyParent);

		hCurItem = m_TreeCtrl.GetNextSiblingItem(hCurItem);
	}
}

LRESULT CSiteSpiderView::ScanNotifyProc(string url, url_type type, bool param)
{
	pstruct_scan_item pscan_item = nullptr;
	switch(type)
	{
	case url_type_host:
	case url_type_dir:
	case url_type_param:
	case url_type_asp:
	case url_type_aspx:
	case url_type_jsp:
	case url_type_php:
	case url_type_html:
	case url_type_other:
	case url_type_unknow:
		pscan_item = new struct_scan_item(type, url, m_strCookie.GetBuffer());
		pscan_item->param = param;
		::SendMessage(_hParenthWnd, MSG_ADDSCAN_URL, NULL, (LPARAM)pscan_item);
	}
	return 0;
}

LRESULT CSiteSpiderView::ExportNotifyProc(string url, url_type type, bool param)
{
	_exportlist = _exportlist + "\r\n" + url ;
	return 0;
}

void CSiteSpiderView::OnBnClickedBtnSetSession()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	m_spider.setopt_cookie(m_strCookie.GetBuffer());
}

BOOL CSiteSpiderView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	// 重写PreTranslateMessage(MSG* pMsg), 否则无法响应Ctrl+V等消息
	if( IsDialogMessage( pMsg ) )
		return TRUE;
	else
		return CFormView::PreTranslateMessage(pMsg);
	//return CFormView::PreTranslateMessage(pMsg);
}

int CSiteSpiderView::SetTarget(CString target)
{
	m_strTargetUrl = target;
	UpdateData(FALSE);
	return 0;
}


void CSiteSpiderView::OnTreepopOpenUrl()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	if(hItem)
	{
		CString url = GetTreeItemUrl(hItem);
		ShellExecute(NULL, NULL,url, NULL, NULL, SW_SHOWNORMAL);
	}
}


void CSiteSpiderView::OnTreepopCopyUrl()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	if(hItem)
	{
		CString url = GetTreeItemUrl(hItem);
		if(OpenClipboard())
		{
			int len = url.GetLength();
			HANDLE hClip;
			char *pBuf;
			EmptyClipboard();
			hClip=GlobalAlloc(GMEM_MOVEABLE,len+1);
			pBuf=(char*)GlobalLock(hClip);
			strcpy_s(pBuf, len+1, url);
			GlobalUnlock(hClip);
			SetClipboardData(CF_TEXT,hClip);
			CloseClipboard();
		}
	}
}

void CSiteSpiderView::OnTreepopCheckNode()
{
	// TODO: 在此添加命令处理程序代码
	::PostMessage(_hParenthWnd,MSG_SWITCH_VIEW, WEAK_SCANNER_VIEW,0);

	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	if(hItem)
	{
		CString url = GetTreeItemUrl(hItem);
		int itemType = m_TreeCtrl.GetItemData(hItem) & 0x7F;
		
		bool param = false;

		if(itemType == url_type_param)
		{
			itemType = m_TreeCtrl.GetItemData(m_TreeCtrl.GetParentItem(hItem)) & 0x7F;
			param = true;
		}
		// Notify
		ScanNotifyProc(url.GetBuffer(), (url_type)itemType, param);

		if(m_TreeCtrl.GetChildItem(hItem) != nullptr)
		{
			TraversalUrl(url.GetBuffer(), hItem, (url_type)itemType, ScanNotifyProc, true);
		}
	}
}

void CSiteSpiderView::OnTreepopClearNode()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	if(hItem!=NULL)
	{
		m_TreeCtrl.DeleteItem(hItem);
	}
}

void CSiteSpiderView::OnTreepopClearTree()
{
	// TODO: 在此添加命令处理程序代码
	m_TreeCtrl.DeleteAllItems();
}

LRESULT CALLBACK CSiteSpiderView::NotifyProc(string& request, httpproxy_mode mode)
{
	if(mode == httpproxy_mode_getcookie)
	{
		//::MessageBox(0,request.c_str(),0,0);
		CString host, cookie;
		int pos1 = request.find("Host: "), pos2;
		if(pos1 != -1)
		{
			pos2 = request.find("\r\n", pos1);
			host = request.substr(pos1+6, pos2-pos1-6).c_str();
			pos1 = request.find("Cookie: ");
			if(pos1 != -1)
			{
				pos2 = request.find("\r\n", pos1);
				cookie = request.substr(pos1+8, pos2-pos1-8).c_str();

				int i=0;
				int count=m_SessionList.GetItemCount();
				while(i<count)
				{
					if(m_SessionList.GetItemText(i,1)==cookie)
					{
						break;
					}
					i++;
				}
				if(i==count)
				{
					m_SessionList.InsertItem(i, host);
					m_SessionList.SetItemText(i, 1, cookie);
				}
			}
		}
		
	}
	return 0;
}

void CSiteSpiderView::OnNMDblclkSessionList(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateData(TRUE);
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	int idx=phdr->iItem;
	m_strCookie =m_SessionList.GetItemText(idx, 1);
	UpdateData(FALSE);
	*pResult = 0;
}


void CSiteSpiderView::OnTreepopExportNode()
{
	// TODO: 在此添加命令处理程序代码
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	if(hItem)
	{
		CString url = GetTreeItemUrl(hItem);
		int itemType = m_TreeCtrl.GetItemData(hItem) & 0x7F;
		
		CString path;
		CFileDialog file(false,"file",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"文本文件|*.txt|列表文件|*.lst||",this);
		if(file.DoModal()==IDOK)
		{
			path=file.GetPathName();

			// Notify
			ExportNotifyProc(url.GetBuffer(), (url_type)itemType, false);

			if(m_TreeCtrl.GetChildItem(hItem) != nullptr)
			{
				TraversalUrl(url.GetBuffer(), hItem, (url_type)itemType, ExportNotifyProc, true);
			}

			CStdioFile stdfile;
			stdfile.Open(path,CFile::modeCreate|CFile::modeWrite);
			stdfile.WriteString(_exportlist.c_str());
			stdfile.Close();
			_exportlist.clear();
		}
	}
}


LRESULT CALLBACK CSiteSpiderView::NotifyProc(string url)
{
	int pos1,pos2;
	string root, path, name, param, anchor;
	if((pos1 = url.find("://"))==-1)
	{
		// 排除javascript、mailto等伪协议
		return 0;
	}
	pos1 = url.find('/', pos1+3);
	if(pos1==-1)
	{
		root = url;
		url = "";
	}
	else
	{
		root = url.substr(0, pos1);
		url = url.substr(pos1+1);
		pos2 = url.find('#');
		if(pos2!=-1)
		{
			anchor = url.substr(pos2);
			url = url.substr(0, pos2);
		}
		pos2 = url.find('?');
		if(pos2!=-1)
		{
			param = url.substr(pos2);
			url = url.substr(0, pos2);
		}
	}
	if(!url.empty())
	{
		int len = url.length();
		int pos = url.find_last_of('/');
		if(pos == -1)
		{
			name = url;
			path = "";
		}
		else
		{
			if(pos != len-1)
			{
				name = url.substr(pos + 1);
				path = url.substr(0, pos + 1);
			}
		}
	}

	UINT nMask = TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT;
	
	// 插入root节点
	HTREEITEM hItem=m_TreeCtrl.GetRootItem();
	while(hItem!=nullptr)
	{
		if(m_TreeCtrl.GetItemText(hItem)==root.c_str() && (m_TreeCtrl.GetItemData(hItem) & 0x7F) == url_type_host)
		{
			break;
		}
		hItem=m_TreeCtrl.GetNextSiblingItem(hItem);
	}
	if(hItem==nullptr)
	{
		hItem=m_TreeCtrl.InsertItem(nMask, root.c_str(), url_type_host, url_type_host,0, 0, url_type_host, TVI_ROOT, TVI_LAST );
	}

	// 插入path节点
	if(!path.empty())
	{
		int pos;
		int len;
		string dir;
		while((pos=path.find('/'))!=-1)
		{
			dir=path.substr(0, pos);
			len = path.length();
			path = path.substr(pos + 1);
			if(dir.empty())
				continue;
			HTREEITEM hSubItem=m_TreeCtrl.GetChildItem(hItem);
			while(hSubItem!=nullptr)
			{
				if(m_TreeCtrl.GetItemText(hSubItem)==dir.c_str() && (m_TreeCtrl.GetItemData(hSubItem) & 0x7F) == url_type_dir)
				{
					break;
				}
				hSubItem=m_TreeCtrl.GetNextSiblingItem(hSubItem);
			}
			if(hSubItem==nullptr)
			{
				hItem=m_TreeCtrl.InsertItem(nMask, dir.c_str(), url_type_dir, url_type_dir,0, 0, url_type_dir, hItem, TVI_FIRST );
			}
			else
			{
				hItem = hSubItem;
			}
		}
	}

	if(!name.empty())
	{
		HTREEITEM hSubItem=m_TreeCtrl.GetChildItem(hItem);
		while(hSubItem!=nullptr)
		{
			if(m_TreeCtrl.GetItemText(hSubItem)==name.c_str() && (m_TreeCtrl.GetItemData(hSubItem) & 0x7F) != url_type_dir)
			{
				break;
			}
			hSubItem=m_TreeCtrl.GetNextSiblingItem(hSubItem);
		}
		if(hSubItem==nullptr)
		{
			int type = url_type_unknow;
			int dot = name.find_last_of('.');
			if(dot!=-1)
			{
				string ext = name.substr(dot+1);
				if( strstr("shtml|htm|html", ext.c_str()))
					type = url_type_html;
				else if( strstr("jpeg|ico|bmp|png|gif|jpg", ext.c_str()))
					type = url_type_pic;
				else if(ext == "js")
					type = url_type_js;
				else if(ext == "css")
					type = url_type_css;
				else if(ext == "php")
					type = url_type_php;
				else if(ext == "asp")
					type = url_type_asp;
				else if(ext == "aspx")
					type = url_type_aspx;
				else if(ext == "jsp")
					type = url_type_jsp;
				else if( strstr("doc|docx|ppt|pptx|xls|xlsx|rar|zip|pdf|txt|exe", ext.c_str()))
					type = url_type_file;
				else
					type = url_type_other;
			}
			hItem=m_TreeCtrl.InsertItem(nMask, name.c_str(), type, type,0, 0, type, hItem, TVI_LAST );
		}
		else
		{
			hItem = hSubItem;
		}
	}

	if(!param.empty())
	{
		HTREEITEM hSubItem=m_TreeCtrl.GetChildItem(hItem);
		while(hSubItem!=nullptr)
		{
			if(m_TreeCtrl.GetItemText(hSubItem)==param.c_str() && (m_TreeCtrl.GetItemData(hSubItem) & 0x7F) == url_type_param)
			{
				break;
			}
			hSubItem=m_TreeCtrl.GetNextSiblingItem(hSubItem);
		}
		if(hSubItem==nullptr)
		{

			hItem=m_TreeCtrl.InsertItem(nMask, param.c_str(), url_type_param, url_type_param,0, 0, url_type_param, hItem, TVI_LAST );
		}
		else
		{
			hItem = hSubItem;
		}
	}
	if(!anchor.empty())
	{
		HTREEITEM hSubItem=m_TreeCtrl.GetChildItem(hItem);
		while(hSubItem!=nullptr)
		{
			if(m_TreeCtrl.GetItemText(hSubItem)==anchor.c_str() && (m_TreeCtrl.GetItemData(hSubItem) & 0x7F) == url_type_anchor)
			{
				break;
			}
			hSubItem=m_TreeCtrl.GetNextSiblingItem(hSubItem);
		}
		if(hSubItem==nullptr)
		{
			hItem = m_TreeCtrl.InsertItem(nMask, anchor.c_str(), url_type_anchor, url_type_anchor,0, 0, url_type_anchor, hItem, TVI_LAST );
		}
	}

	m_TreeCtrl.Expand(m_TreeCtrl.GetParentItem(hItem), TVE_EXPAND);

	LRESULT rtn = m_TreeCtrl.GetItemData(hItem);
	if(!(rtn & 0x80))			// 是否爬行过的标记
	{
		m_TreeCtrl.SetItemData(hItem, rtn | 0x80);			// 标记为爬行过(实际是在返回时才去爬行，即先标记后爬行),这样不是太好
															// 首先万一标记后却意外中断则实际未爬行，不过应该不会出现这种情况
															// 其次JPG\CSS等类型节点虽然后面不会爬行，但也会被标记为爬行过，感觉怪怪的
	}
	return rtn;
}