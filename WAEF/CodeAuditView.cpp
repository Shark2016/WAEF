// CodeAuditView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WAEF.h"
#include "CodeAuditView.h"


#include <string>
#include <regex>

// CCodeAuditView

IMPLEMENT_DYNCREATE(CCodeAuditView, CFormView)

CCodeAuditView::CCodeAuditView()
	: CFormView(CCodeAuditView::IDD)
	, m_First(true)
{

}

CCodeAuditView::~CCodeAuditView()
{
}

void CCodeAuditView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AUDIT_PROJECT_TREE, m_ProjectTree);
	DDX_Control(pDX, IDC_AUDIT_RESULT_LIST, m_ResultList);
}

BEGIN_MESSAGE_MAP(CCodeAuditView, CFormView)
	ON_BN_CLICKED(IDC_AUDIT_LOAD_PROJECT, &CCodeAuditView::OnBnClickedAuditLoadProject)
	ON_BN_CLICKED(IDC_AUDIT_START_AUDIT, &CCodeAuditView::OnBnClickedAuditStartAudit)
	ON_NOTIFY(NM_DBLCLK, IDC_AUDIT_RESULT_LIST, &CCodeAuditView::OnNMDblclkAuditResultList)
	ON_NOTIFY(NM_DBLCLK, IDC_AUDIT_RESULT_LIST, &CCodeAuditView::OnNMDblclkAuditResultList)
END_MESSAGE_MAP()


// CCodeAuditView ���

#ifdef _DEBUG
void CCodeAuditView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CCodeAuditView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CCodeAuditView ��Ϣ�������


void CCodeAuditView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���
	if(m_First)
	{
		SHFILEINFO	sfi;
		HIMAGELIST hImageList = (HIMAGELIST)SHGetFileInfo
			(
			NULL,
			0,
			&sfi,
			sizeof(SHFILEINFO),
			SHGFI_SMALLICON | SHGFI_SYSICONINDEX
			);
		CImageList*  m_pImageList_Small = CImageList::FromHandle(hImageList);
		m_ProjectTree.SetImageList(m_pImageList_Small, LVSIL_NORMAL);
		_codeAuditMdl.InitModule(&m_ProjectTree, &m_ResultList);
		m_ResultList.InsertColumn(0, "ID", LVCFMT_LEFT, 30);
		m_ResultList.InsertColumn(1, "λ��", LVCFMT_LEFT, 100);
		m_ResultList.InsertColumn(2, "����", LVCFMT_LEFT, 100);
		m_ResultList.InsertColumn(3, "˵��", LVCFMT_LEFT, 100);
		m_ResultList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

		m_First = false;
	}

}

void CCodeAuditView::OnBnClickedAuditLoadProject()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO bi;
	char buffer[MAX_PATH];
	ZeroMemory(buffer,MAX_PATH);
	bi.hwndOwner=GetSafeHwnd();
	bi.pidlRoot=NULL;
	bi.pszDisplayName=buffer;
	bi.lpszTitle="ѡ��һ���ļ���";
	bi.ulFlags=BIF_EDITBOX;
	bi.lpfn=NULL;
	bi.lParam=0;
	bi.iImage=0;
	LPITEMIDLIST pList=NULL;
	if((pList=SHBrowseForFolder(&bi))!=NULL)
	{
		char path[MAX_PATH];
		ZeroMemory(path,MAX_PATH);
		SHGetPathFromIDList(pList,path);
		_codeAuditMdl.LoadProjectDir(path);
	}
}

bool is_email_valid(const std::string& email)
{
   const std::regex pattern("(\\w+)(\\.|_)?(\\w*)@(\\w+)(\\.(\\w+))+");
   return std::regex_match(email, pattern);
}

void CCodeAuditView::OnBnClickedAuditStartAudit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	_codeAuditMdl.AuditProject();
}

void CCodeAuditView::OnNMDblclkAuditResultList(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int idx=phdr->iItem;
	CString vulPos = m_ResultList.GetItemText(idx, 1);
	int pos = vulPos.Find("(line: ");
	CString path = vulPos.Left(pos);
	pos = atoi(vulPos.Right(vulPos.GetLength()-pos-7));

	CString editPath = "D:\\Program Files\\Notepad++\\notepad++.exe";
	CString editParam;
	editParam.Format("-n%d %s", pos, path);

	ShellExecute(NULL, NULL,editPath, editParam, NULL, SW_SHOWNORMAL);
	*pResult = 0;
}