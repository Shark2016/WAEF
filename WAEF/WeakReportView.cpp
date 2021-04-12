// ScanResultView.cpp : 实现文件
//

#include "stdafx.h"
#include "WAEF.h"
#include "WeakReportView.h"

char* weakness_type_desc[] = {
	"SQL注入",
	"XSS",
	"列目录",
	"PHP报错",
	"本地或远程文件包含",
	"PHP代码注入",
	"命令注入",
	"敏感信息泄露",
	"敏感信息泄露",
	"ASP.NET Padding Oracle漏洞",
};

char* weakness_level_desc[] = {
	"提醒",
	"低危",
	"中危",
	"高危"
};

char* report_color[] = {
	"green",
	"blue",
	"orange",
	"red"
};


// CWeakReportView

IMPLEMENT_DYNCREATE(CWeakReportView, CFormView)

CWeakReportView::CWeakReportView()
	: CFormView(CWeakReportView::IDD)
{
	CWinApp* pApp = AfxGetApp();
	//m_ImgList.Create(16,16,ILC_COLOR, 2, 2);
	//m_ImgList.Create(GetSystemMetrics(SM_CXSMICON),	GetSystemMetrics(SM_CYSMICON), ILC_COLOR24, 50, 50);
	m_ImgList.Create(16,16,ILC_COLOR24, 50, 50);
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

		IDI_ICON_PARAM,

		IDI_ICON_GREEN,
		IDI_ICON_BLUE,
		IDI_ICON_ORANGE, 
		IDI_ICON_RED,
		-1};
	int i=0;
	while(array_icon_id[i]!=-1)
	{
		m_ImgList.Add(pApp->LoadIcon(array_icon_id[i]));
		i++;
	}
	_first = true;
}

CWeakReportView::~CWeakReportView()
{
}

void CWeakReportView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_REPORT, m_TreeCtrl);
//	DDX_Text(pDX, IDC_WEAKNESS_DESC, m_WeakDesc);
}

BEGIN_MESSAGE_MAP(CWeakReportView, CFormView)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_REPORT, &CWeakReportView::OnSelchangedTreeReport)
	ON_BN_CLICKED(IDC_BUTTON1, &CWeakReportView::OnBnClickedButton1)
END_MESSAGE_MAP()


// CWeakReportView 诊断

#ifdef _DEBUG
void CWeakReportView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CWeakReportView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CWeakReportView 消息处理程序

void CWeakReportView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	if(_first)
	{
		m_TreeCtrl.SetImageList(&m_ImgList,TVSIL_NORMAL); 

		
		_htmlView = new CWAEFHtmlView();
		_htmlView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,CRect(515,40,960,315), this, 0, NULL);
		
		_first=false;

		CStdioFile stdfile;
		CString str;
		DWORD dwRead;
		if(stdfile.Open("Report\\Template\\ReportA.tpl",CFile::modeRead))
		{
			do
			{
				dwRead = stdfile.ReadString(str);
				m_ReportTemplateA+= str+"\r\n";
			}while(dwRead > 0);
			stdfile.Close();
		}
		else 
		{
			AfxMessageBox("警告：报告模板丢失，请重新安装程序");
			return;
		}
		
		if(stdfile.Open("Report\\Template\\ReportB.tpl",CFile::modeRead))
		{
			do
			{
				dwRead = stdfile.ReadString(str);
				m_ReportTemplateB+= str+"\r\n";
			}while(dwRead > 0);
			stdfile.Close();
		}
		else 
		{
			AfxMessageBox("警告：报告模板丢失，请重新安装程序");
			return;
		}
		
		CString strReport = "file://E:\\Program\\VC++\\WAEF\\WAEF(20140601 完善+皮肤)\\WAEF\\WAEF\\";
		char szPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, szPath);
		m_ReportPath = "file://";
		m_ReportPath += szPath;
		m_ReportPath += "\\";
	}
}

int CWeakReportView::AddReportItem(CString url, pstruct_scan_item pscan_item)
{
	int pos1;
	pos1 = url.Find('/', 7);
	if(pos1==-1)
	{
		return 0;
	}
	
	int len = url.GetLength();
	CString root, path, type;
	HTREEITEM hRootItem, hTypeItem, hWeakItem;
	root = url.Left(pos1+1);
	path = (pos1+1 == len)?"/":url.Right(len-pos1);

	
	UINT nMask = TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT;
	// 插入root节点
	hRootItem=m_TreeCtrl.GetRootItem();
	while(hRootItem!=nullptr)
	{
		if(m_TreeCtrl.GetItemText(hRootItem)==root)
		{
			break;
		}
		hRootItem=m_TreeCtrl.GetNextSiblingItem(hRootItem);
	}
	if(hRootItem==nullptr)
	{
		hRootItem=m_TreeCtrl.InsertItem(nMask, root, url_type_host, url_type_host,0, 0, 0, TVI_ROOT, TVI_LAST );
		m_TreeCtrl.SetItemData(hRootItem, (DWORD_PTR)new struct_report_item(root.GetBuffer()));
	}

	pstruct_weakness_item pweakness_item = pscan_item->pweakness_item;

	while(pweakness_item!=nullptr)
	{

		type = weakness_type_desc[pweakness_item->type];
		//weakness_level level = pweakness_item->level;

		hTypeItem=m_TreeCtrl.GetChildItem(hRootItem);
		while(hTypeItem!=nullptr)
		{
			if(m_TreeCtrl.GetItemText(hTypeItem)==type)
			{
				break;
			}
			hTypeItem=m_TreeCtrl.GetNextSiblingItem(hTypeItem);
		}
		if(hTypeItem==nullptr)
		{
			int level = pweakness_item->level + 13;
			hTypeItem=m_TreeCtrl.InsertItem(nMask, type, level, level,0, 0, 0, hRootItem, TVI_LAST );
		}

		hWeakItem=m_TreeCtrl.GetChildItem(hTypeItem);
		while(hWeakItem!=nullptr)
		{
			if(m_TreeCtrl.GetItemText(hWeakItem)==path)
			{
				break;
			}
			hWeakItem=m_TreeCtrl.GetNextSiblingItem(hWeakItem);
		}
		if(hWeakItem==nullptr)
		{
			pstruct_report_item preport_item = new struct_report_item(pweakness_item);
			GenerateReport(preport_item);
			hWeakItem=m_TreeCtrl.InsertItem(nMask, path, pscan_item->type, pscan_item->type,0, 0, (DWORD_PTR)preport_item, hTypeItem, TVI_LAST );
			pstruct_report_item proot_report_item = (pstruct_report_item)m_TreeCtrl.GetItemData(hRootItem);
			switch(pweakness_item->level)
			{
			case weakness_level_low:
				proot_report_item->low_count++;
				break;
			case weakness_level_medium:
				proot_report_item->medium_count++;
				break;
			case weakness_level_high:
				proot_report_item->high_count++;
				break;
			case weakness_level_inform:
				proot_report_item->info_count++;
				break;
			}
		}
		pweakness_item = pweakness_item->pNext;
	}
	return 0;
}

void CWeakReportView::GenerateReport(pstruct_report_item preport_item)
{
	CString content;
	if(preport_item->type == report_host)
	{
		content = m_ReportTemplateA;
		char count[10];
		sprintf_s(count, 10, "%d", preport_item->high_count);
		content.Replace("{%VUL_HIGH%}", count); 
		sprintf_s(count, 10, "%d", preport_item->medium_count);
		content.Replace("{%VUL_MEDIUM%}", count); 
		sprintf_s(count, 10, "%d", preport_item->low_count);
		content.Replace("{%VUL_LOW%}", count); 
		sprintf_s(count, 10, "%d", preport_item->info_count);
		content.Replace("{%VUL_INFO%}", count); 
		sprintf_s(count, 10, "%d", preport_item->high_count +  preport_item->medium_count + preport_item->low_count + preport_item->info_count);
		content.Replace("{%VUL_COUNT%}", count); 
	}
	else
	{
		content = m_ReportTemplateB;
		content.Replace("{%VUL_COLOR%}", report_color[preport_item->pweakness_item->level]);
		content.Replace("{%VUL_NAME%}", weakness_type_desc[preport_item->pweakness_item->type]); 
		content.Replace("{%VUL_PATH%}", preport_item->pweakness_item->payload.c_str()); 
		content.Replace("{%VUL_LEVEL%}", weakness_level_desc[preport_item->pweakness_item->level]); 
		content.Replace("{%VUL_DESC%}", preport_item->pweakness_item->description.c_str()); 
		content.Replace("{%VUL_FIX%}", preport_item->pweakness_item->advice.c_str()); 
	}
	SYSTEMTIME sys; 
	GetLocalTime( &sys );
	char report[256];
	sprintf_s(report, 256, 
		"Report\\%4d%02d%02d%02d%02d%02d%03d.html",
		sys.wYear,
		sys.wMonth,
		sys.wDay,
		sys.wHour,
		sys.wMinute,
		sys.wSecond,
		sys.wMilliseconds
		);
	CStdioFile stdfile;
	stdfile.Open(report, CFile::modeCreate|CFile::modeWrite);
	stdfile.WriteString(content);
	stdfile.Close();
	preport_item->reportfile = report;
}

void CWeakReportView::OnSelchangedTreeReport(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM hRootItem = m_TreeCtrl.GetRootItem();
	HTREEITEM hCurItem = m_TreeCtrl.GetSelectedItem();
	pstruct_report_item preport_item = (pstruct_report_item)m_TreeCtrl.GetItemData(hCurItem);
	if(hRootItem==hCurItem)
	{
		if(preport_item != nullptr)
		{
			GenerateReport(preport_item);
			_htmlView->Navigate2(m_ReportPath + preport_item->reportfile.c_str()); 
		}
	}
	else 
	{
		if(preport_item != nullptr)
		{
			_htmlView->Navigate2(m_ReportPath + preport_item->reportfile.c_str()); 
		}
	}

	*pResult = 0;
}


BOOL CWeakReportView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	// 重写PreTranslateMessage(MSG* pMsg), 否则无法响应Ctrl+V等消息
	if( IsDialogMessage( pMsg ) )
		return TRUE;
	else
		return CFormView::PreTranslateMessage(pMsg);
	//return CFormView::PreTranslateMessage(pMsg);
}


void CWeakReportView::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
}
