// DeepScanView.cpp : 实现文件
//

#include "stdafx.h"
#include "WAEF.h"
#include "DeepScanView.h"


// CDeepScanView

IMPLEMENT_DYNCREATE(CDeepScanView, CFormView)

CDeepScanView::CDeepScanView()
	: CFormView(CDeepScanView::IDD)
	, m_TargetHost(_T(""))
	, m_First(true)
{
	CWinApp* pApp = AfxGetApp();
	//m_ImgList.Create(16,16,ILC_COLOR, 2, 2);
	//m_ImgList.Create(GetSystemMetrics(SM_CXSMICON),	GetSystemMetrics(SM_CYSMICON), ILC_COLOR24, 50, 50);
	m_ImgList.Create(16,16,ILC_COLOR24, 50, 50);
	m_ImgList.SetBkColor(GetSysColor(COLOR_WINDOW)); 
	int array_icon_id[] = {
		IDI_ICON_HOST,
		IDI_ICON_DIR,
		IDI_ICON_FILE,
		-1};
	int i=0;
	while(array_icon_id[i]!=-1)
	{
		m_ImgList.Add(pApp->LoadIcon(array_icon_id[i]));
		i++;
	}
}

CDeepScanView::~CDeepScanView()
{
}

void CDeepScanView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DEEP_TARGET_HOST, m_TargetHost);
	DDX_Control(pDX, IDC_DEEP_INTERACTIVE_BOX, m_deepInteractiveBox);
	DDX_Control(pDX, IDC_DEEP_MODULE_TREE, m_ModuleTree);
}

BEGIN_MESSAGE_MAP(CDeepScanView, CFormView)
	ON_BN_CLICKED(IDC_DEEP_LOAD_DESC, &CDeepScanView::OnBnClickedDeepLoadDesc)
	ON_BN_CLICKED(IDC_DEEP_PARAM_STR, &CDeepScanView::OnBnClickedDeepParamStr)
	ON_BN_CLICKED(IDC_DEEP_EXECUTE, &CDeepScanView::OnBnClickedDeepExecute)
END_MESSAGE_MAP()


// CDeepScanView 诊断

#ifdef _DEBUG
void CDeepScanView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CDeepScanView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CDeepScanView 消息处理程序

int CDeepScanView::SetTargetHost(CString targetHost)
{
	m_TargetHost = targetHost;
	UpdateData(FALSE);
	return 0;
}

void CDeepScanView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	if(m_First)
	{
		m_TargetHost="http://test.shark.org";
		UpdateData(FALSE);

		UINT nMask = TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT;
		m_ModuleTree.SetImageList(&m_ImgList,TVSIL_NORMAL); 
		m_ModuleTree.InsertItem(nMask, "Web容器", 0, 0, 0, 0, 0, TVI_ROOT, TVI_LAST );
		m_ModuleTree.InsertItem(nMask, "Web应用", 0, 0, 0, 0, 0, TVI_ROOT, TVI_LAST );
				
		_deep_scan_mdl.InitModule(&m_ModuleTree, &m_deepInteractiveBox);

		m_First = false;
	}
}

void CDeepScanView::OnBnClickedDeepLoadDesc()
{
	// TODO: 在此添加控件通知处理程序代码
	HTREEITEM hItem = m_ModuleTree.GetSelectedItem();
	CString moduleName = m_ModuleTree.GetItemText(hItem);
	if(moduleName.Find(".waef"))
	{
		_deep_scan_mdl.LoadWAEFModule(moduleName);
	}
	_deep_scan_mdl.LoadModuleDesc();
}

void CDeepScanView::OnBnClickedDeepParamStr()
{
	// TODO: 在此添加控件通知处理程序代码
	_deep_scan_mdl.LoadModuleConfig();
}


void CDeepScanView::OnBnClickedDeepExecute()
{
	// TODO: 在此添加控件通知处理程序代码
	_deep_scan_mdl.ExecuteModule();
}


BOOL CDeepScanView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if( IsDialogMessage( pMsg ) )
		return TRUE;
	else
		return CFormView::PreTranslateMessage(pMsg);
}
