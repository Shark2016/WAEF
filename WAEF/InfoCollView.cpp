// InfoCollView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WAEF.h"
#include "InfoCollView.h"


// CInfoCollView

IMPLEMENT_DYNCREATE(CInfoCollView, CFormView)

CInfoCollView::CInfoCollView()
	: CFormView(CInfoCollView::IDD)
	, _domain(_T(""))
	, _ipaddress(_T(""))
{
	_first = true;
}

CInfoCollView::~CInfoCollView()
{
}

void CInfoCollView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INFO_LIST, _info_list);
	//  DDX_Control(pDX, IDC_EDIT_IP_ADDRESS, _ip_address);
	DDX_Text(pDX, IDC_EDIT_DOMAIN, _domain);
	DDX_Text(pDX, IDC_EDIT_IP_ADDRESS, _ipaddress);
}

BEGIN_MESSAGE_MAP(CInfoCollView, CFormView)
	ON_BN_CLICKED(IDC_BTN_GET_INFO, &CInfoCollView::OnBnClickedBtnGetInfo)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BTN_RESOLVE_IP, &CInfoCollView::OnBnClickedBtnResolveIp)
	ON_BN_CLICKED(IDC_BTN_SERVER_INFO, &CInfoCollView::OnBnClickedBtnServerInfo)
	ON_NOTIFY(NM_RCLICK, IDC_INFO_LIST, &CInfoCollView::OnRclickInfoList)
	ON_NOTIFY(NM_DBLCLK, IDC_INFO_LIST, &CInfoCollView::OnDblclkInfoList)
	ON_COMMAND(ID_INFOPOP_OPEN_SITE, &CInfoCollView::OnInfopopOpenSite)
	ON_COMMAND(ID_INFOPOP_SPIDER_SITE, &CInfoCollView::OnInfopopSpiderSite)
	ON_COMMAND(ID_INFOPOP_CLEAR_LIST, &CInfoCollView::OnInfopopClearList)
	ON_COMMAND(ID_INFOPOP_IMPORT_LIST, &CInfoCollView::OnInfopopImportList)
	ON_COMMAND(ID_INFOPOP_EXPORT_LIST, &CInfoCollView::OnInfopopExportList)
	ON_BN_CLICKED(IDC_BTN_IMPORT_LIST, &CInfoCollView::OnInfopopImportList)
	ON_BN_CLICKED(IDC_BTN_EXPORT_LIST, &CInfoCollView::OnInfopopExportList)
	ON_COMMAND(ID_COLLECT_DEEP_SCAN, &CInfoCollView::OnDeepScan)
	ON_COMMAND(ID_COLLECT_PLUGIN_SCAN, &CInfoCollView::OnCollectPluginScan)
END_MESSAGE_MAP()


// CInfoCollView ���

#ifdef _DEBUG
void CInfoCollView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CInfoCollView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CInfoCollView ��Ϣ�������


void CInfoCollView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���
	if(_first)		//��֪��ΪʲôҪ�������Σ����ԼӸ��ж�
	{
		_info_list.InsertColumn(0,_T("����"),LVCFMT_LEFT,180);
		_info_list.InsertColumn(1,_T("ip"),LVCFMT_LEFT,130);
		_info_list.InsertColumn(2,_T("վ�����"),LVCFMT_LEFT,240);
		_info_list.InsertColumn(3,_T("Server"),LVCFMT_LEFT,190);
		_info_list.InsertColumn(4,_T("CMSʶ��"),LVCFMT_LEFT,210);
	//	_info_list.SetItemState(0,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
		_info_list.SetExtendedStyle(LVS_EX_FULLROWSELECT);
		_info_coll_mdl.init(&_info_list);
		_domain = "www.baidu.com";
		UpdateData(FALSE);
		_first = false;
	}
}



void CInfoCollView::OnBnClickedBtnGetInfo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	string domain = _domain.GetBuffer();
	string ip_address = _ipaddress.GetBuffer();
	if(domain.find("://")==-1)
	{
		domain = "http://" + domain;
	}
	int pos=domain.find("://");
	if((pos=domain.find("/", pos+3))!=-1)
	{
		domain=domain.substr(0,pos);
	}
	_info_coll_mdl.InsertItem(domain, ip_address, string(""));
	_info_coll_mdl.init_targetquery(ip_address);
	_info_coll_mdl.get_info(BING_API);		// Bing ��ʱʧЧ
//	_info_coll_mdl.get_info(BEST_API);		// 114best, ������ʱ������
//	_info_coll_mdl.get_info(AIZHAN_API);	// ���ݲ���׼ȷ,ͬipÿ���ύ��������
//	_info_coll_mdl.get_info(ZZSKY_API);		// zzsky,��Ҫ������ת����
}


void CInfoCollView::OnBnClickedBtnResolveIp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	UpdateData(TRUE);

	string domain=_domain.GetBuffer();
	int pos1,pos2;
	if((pos1=domain.find("//"))!=-1)
	{
		if((pos2=domain.find("/",pos1+2))!=-1)
		{
			domain=domain.substr(pos1+2, pos2-pos1-2);
		}
		else
		{
			domain=domain.substr(pos1+2);
		}
	}
	CToolMdl tool;

	char host_name[256]={0};
	char ip_address[32]={0};
	size_t size= strlen(host_name);
	strcpy_s(host_name, 256, domain.c_str()); 

	tool.resolve_ip(host_name, ip_address, 32);
	_ipaddress = ip_address;
	UpdateData(FALSE);
}

void CInfoCollView::OnBnClickedBtnServerInfo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	_info_coll_mdl.get_server_info();
}

BOOL CInfoCollView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if( IsDialogMessage( pMsg ) )
		return TRUE;
	else
		return CFormView::PreTranslateMessage(pMsg);
}

void CInfoCollView::OnDblclkInfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int idx=phdr->iItem;
	CString Target =_info_coll_mdl.GetItemText(idx, 0);
	ShellExecute(NULL, NULL,Target, NULL, NULL, SW_SHOWNORMAL);

	*pResult = 0;
}

void CInfoCollView::OnRclickInfoList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CMenu	popup;
	CMenu*	pM;
	popup.LoadMenu(IDR_MENU_INFOPOP);
	pM = popup.GetSubMenu(0);

	CPoint	p;
	GetCursorPos(&p);

	int nItem=phdr->iItem;
	if(nItem==-1)
	{
		pM->EnableMenuItem(0, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		pM->EnableMenuItem(1, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		pM->EnableMenuItem(2, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		pM->EnableMenuItem(3, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
	}

	pM->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);

	*pResult = 0;
}


void CInfoCollView::OnInfopopOpenSite()
{
	// TODO: �ڴ���������������
	int nIndex=_info_list.GetSelectionMark();
	CString Target = _info_list.GetItemText(nIndex,0);
	ShellExecute(NULL, NULL,Target, NULL, NULL, SW_SHOWNORMAL);
}


void CInfoCollView::OnInfopopSpiderSite()
{
	// TODO: �ڴ���������������
	int nIndex=_info_list.GetSelectionMark();
	CString Target = _info_list.GetItemText(nIndex,0);
	HWND hParent = this->GetParent()->GetSafeHwnd();
	::SendMessage(hParent,MSG_SET_SPIDER_TARGET, 0,(LPARAM)Target.GetBuffer());
}

void CInfoCollView::OnDeepScan()
{
	// TODO: �ڴ���������������
	int nIndex=_info_list.GetSelectionMark();
	CString Target = _info_list.GetItemText(nIndex,0);
	HWND hParent = this->GetParent()->GetSafeHwnd();
	::SendMessage(hParent,MSG_SET_DEEPSCAN_TARGET, 0,(LPARAM)Target.GetBuffer());
}



void CInfoCollView::OnInfopopClearList()
{
	// TODO: �ڴ���������������
	_info_coll_mdl.clear_list();
}



void CInfoCollView::OnInfopopImportList()
{
	// TODO: �ڴ���������������
	CString path;
	CFileDialog file(true,"file",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"�ı��ļ�|*.txt|�б��ļ�|*.lst||",this);
	if(file.DoModal()==IDOK)
	{
		path=file.GetPathName();
		_info_coll_mdl.import_list(path);
	}
}


void CInfoCollView::OnInfopopExportList()
{
	// TODO: �ڴ���������������
	CString path;
	CFileDialog file(false,"file",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"�ı��ļ�|*.txt|�б��ļ�|*.lst||",this);
	if(file.DoModal()==IDOK)
	{
		path=file.GetPathName();
		_info_coll_mdl.export_list(path);
	}
}

void CInfoCollView::OnCollectPluginScan()
{
	// TODO: �ڴ���������������
	int nIndex=_info_list.GetSelectionMark();
	CString Target = _info_list.GetItemText(nIndex,0);
	HWND hParent = this->GetParent()->GetSafeHwnd();
	::SendMessage(hParent,MSG_SET_PLUGIN_TARGET, 0,(LPARAM)Target.GetBuffer());
}
