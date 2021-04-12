// AssisToolView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "WAEF.h"
#include "AssisToolView.h"


// CAssisToolView

IMPLEMENT_DYNCREATE(CAssisToolView, CFormView)

CAssisToolView::CAssisToolView()
	: CFormView(CAssisToolView::IDD)
	, m_Src(_T(""))
	, m_Dst(_T(""))
	, m_First(true)
{

}

CAssisToolView::~CAssisToolView()
{
}

void CAssisToolView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ASSIS_SRC_STRING, m_Src);
	DDX_Text(pDX, IDC_ASSIS_DST_STRING, m_Dst);
	DDX_Control(pDX, IDC_ASSIS_CONVERT_TYPE, m_ConvertType);
}

BEGIN_MESSAGE_MAP(CAssisToolView, CFormView)
	ON_CBN_SELCHANGE(IDC_ASSIS_CONVERT_TYPE, &CAssisToolView::OnSelChangeAssisConvertType)
	ON_EN_CHANGE(IDC_ASSIS_SRC_STRING, &CAssisToolView::OnChangeAssisSrcString)
END_MESSAGE_MAP()


// CAssisToolView ���

#ifdef _DEBUG
void CAssisToolView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CAssisToolView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAssisToolView ��Ϣ�������


void CAssisToolView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���
	if(m_First)
	{
		m_ConvertType.AddString("String -> Hex");
		m_ConvertType.AddString("Hex -> String");
		m_ConvertType.AddString("String -> Ascii");
		m_ConvertType.AddString("Ascii -> String");
		m_ConvertType.AddString("Binary -> Hex");
		m_ConvertType.AddString("Hex -> Binary");
		m_ConvertType.AddString("Base64Encode");
		m_ConvertType.AddString("Base64Decode");
		m_ConvertType.AddString("UrlEncode");
		m_ConvertType.AddString("UrlDecode");
		m_ConvertType.AddString("Escape");
		m_ConvertType.AddString("Unescape");
		m_ConvertType.AddString("HASH");
		m_ConvertType.SetCurSel(0);
		m_First = false;
	}
}

void CAssisToolView::OnSelChangeAssisConvertType()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnChangeAssisSrcString();
}


void CAssisToolView::OnChangeAssisSrcString()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CFormView::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	switch(m_ConvertType.GetCurSel())
	{
	case 0:
		_assis_tool_mdl.ConvertStringToHex(m_Src, m_Dst);
		break;
	case 1:
		_assis_tool_mdl.ConvertHexToString(m_Src, m_Dst);
		break;
	case 2:
		_assis_tool_mdl.ConvertStringToAscii(m_Src, m_Dst);
		break;
	case 3:
		_assis_tool_mdl.ConvertAsciiToString(m_Src, m_Dst);
		break;
	case 4:
		_assis_tool_mdl.ConvertBinaryToHex(m_Src, m_Dst);
		break;
	case 5:
		_assis_tool_mdl.ConvertHexToBinary(m_Src, m_Dst);
		break;
	case 6:
		_assis_tool_mdl.Base64Encode(m_Src, m_Dst);
		break;
	case 7:
		_assis_tool_mdl.Base64Decode(m_Src, m_Dst);
		break;
	case 8:
		_assis_tool_mdl.UrlEncode(m_Src, m_Dst, true);
		break;
	case 9:
		_assis_tool_mdl.UrlDecode(m_Src, m_Dst);
		break;
	case 10:
		_assis_tool_mdl.Escape(m_Src, m_Dst);
		break;
	case 11:
		_assis_tool_mdl.Unescape(m_Src, m_Dst);
		break;
	case 12:
		_assis_tool_mdl.Hash(m_Src, m_Dst);
		break;

	}
	UpdateData(FALSE);
}


BOOL CAssisToolView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if( IsDialogMessage( pMsg ) )
		return TRUE;
	else
		return CFormView::PreTranslateMessage(pMsg);
}
