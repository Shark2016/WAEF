// DeepScanView.cpp : ʵ���ļ�
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


// CDeepScanView ���

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


// CDeepScanView ��Ϣ�������

int CDeepScanView::SetTargetHost(CString targetHost)
{
	m_TargetHost = targetHost;
	UpdateData(FALSE);
	return 0;
}

void CDeepScanView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���
	if(m_First)
	{
		m_TargetHost="http://test.shark.org";
		UpdateData(FALSE);

		UINT nMask = TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT;
		m_ModuleTree.SetImageList(&m_ImgList,TVSIL_NORMAL); 
		m_ModuleTree.InsertItem(nMask, "Web����", 0, 0, 0, 0, 0, TVI_ROOT, TVI_LAST );
		m_ModuleTree.InsertItem(nMask, "WebӦ��", 0, 0, 0, 0, 0, TVI_ROOT, TVI_LAST );
				
		_deep_scan_mdl.InitModule(&m_ModuleTree, &m_deepInteractiveBox);

		m_First = false;
	}
}

void CDeepScanView::OnBnClickedDeepLoadDesc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	_deep_scan_mdl.LoadModuleConfig();
}


void CDeepScanView::OnBnClickedDeepExecute()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	_deep_scan_mdl.ExecuteModule();
}


BOOL CDeepScanView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if( IsDialogMessage( pMsg ) )
		return TRUE;
	else
		return CFormView::PreTranslateMessage(pMsg);
}
