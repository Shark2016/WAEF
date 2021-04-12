#pragma once

#include "ToolMdl.h"
#include <vector>
#include <tinyxml.h>

enum infomdl_error_code
{
	infomdl_error_success,
	infomdl_error_finish,
	infomdl_error_xmlerror,

	infomdl_error_exists,

	infomdl_error_notformatip
};


enum signature_state
{
	signature_state_wait,		//�ȴ�ʶ��
	signature_state_success,	//ʶ��ɹ�
	signature_state_fail,		//ʶ��ʧ��
	
	signature_state_check,			//����ʶ��
	signature_state_refuse1,		//��������ʧ��1��
	signature_state_refuse2,		//��������ʧ��2��
	signature_state_refuse3,		//��������ʧ��3��
	signature_state_refuse4,		//��������ʧ��4��
	signature_state_refuse5,		//��������ʧ��5��
};

class CInfoCollMdl
{
public:
	CInfoCollMdl(void);
	~CInfoCollMdl(void);
	infomdl_error_code clear_list();
	infomdl_error_code get_info(Api_Type api_type, int threadnum = 5);
	infomdl_error_code get_info_cduan();
	infomdl_error_code get_server_info();

	static infomdl_error_code get_info_by_bing(LPVOID lpParam);
	static infomdl_error_code get_info_by_114best(LPVOID lpParam);
	static infomdl_error_code get_info_by_aizhan(LPVOID lpParam);
	static infomdl_error_code get_info_by_zzsky(LPVOID lpParam);
	static infomdl_error_code get_server_header(LPVOID lParam);
	static infomdl_error_code get_cms_signature(LPVOID lParam);

private:
	static TiXmlDocument *_pTiXmlDoc;
	static TiXmlElement  *_pTiXmlElem;

	static HANDLE _mutex_list;
	static HANDLE _mutex_bing;
	static HANDLE _mutex_best;
	static HANDLE _mutex_aizhan;
	static HANDLE _mutex_zzsky;
	static HANDLE _mutex_server;
	static HANDLE _mutex_signature;
	static HANDLE _mutex_query;
	static HANDLE _mutex_tixml;

	static CListCtrl* _infolist;
	static vector<string> _targetquery;

	static int _idx_bing;
	static int _idx_114best;
	static int _idx_aizhan;
	static int _idx_zzsky;

	static int _idx_server;
	static int _idx_signature;


public:
	infomdl_error_code init_targetquery(string& ip_address);
	infomdl_error_code static InsertItem(string& domain, string& ip_address, string& title);
	CString GetItemText(int nIndex, int nColumn);
	int export_list(CString path);
	int import_list(CString path);
	void init(CListCtrl* infolist){_infolist = infolist;};
};