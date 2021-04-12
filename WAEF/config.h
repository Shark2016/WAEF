#pragma once

enum Error_code 
{
	error_success,
	error_spider_stop,

	// network_error
	error_redirect, 
	error_notfound, 
	error_internet_error, 
	error_header_too_long,
	error_unknow_host,
	error_not_format_header,
	error_not_format_ip,

	// process_string_error
	error_not_find,

	// process_url_error
	error_invalid_url,
	
	
	// parse_error
	error_tag_without_value,
	error_not_target_tag,
	error_tag_no_link,


	// process_url_query_error
	error_url_exist,
	error_query_overflow
};
enum url_type
{
	url_type_host,			// ��������
	url_type_dir,			// Ŀ¼����
	url_type_php,			// php�ű�����(.php���������url)
	url_type_asp,			// asp�ű�����(.asp���������url)
	url_type_aspx,			// aspx�ű�����(.aspx���������url)
	url_type_jsp,			// jsp�ű�����(.jsp���������url)
	url_type_html,			// ��̬��ҳ����(.htm; .html; .shtml)
	url_type_pic,			// ͼƬ�ļ�����(.jpg; .jpeg; .png; bmp; .gif; .ico; etc.)
	url_type_js,			// js�ű�����(.js)
	url_type_css,			// css�ļ�����(.css)
	url_type_file,			// ��֪�ĵ��ļ�����(.doc; .docx; .ppt; pptx; .xls; .xlsx; .rar; .pdf; .txt; etc.)
	url_type_other,			// ������չ���ļ�����
	url_type_unknow,		// ����չ���ļ�����

	// �������ͽ���������������״��ʾ, ���ݵ�©��ɨ�����ʱ���ǰ�����������ʹ���
	url_type_param,			// ��̬�ű�GET��������(����������������״��ʾ)
	url_type_anchor			// #ê��
};
//
//enum node_type
//{
//	NODE_HOST,
//
//	NODE_DIR,
//
//	NODE_FILE,
//	NODE_HTML,
//	NODE_PIC,
//	NODE_JS,
//	NODE_CSS,
//	NODE_ASP,
//	NODE_ASPX,
//	NODE_JSP,
//	NODE_PHP,
//
//	NODE_DOC,
//
//	NODE_PARAM,
//	NODE_ANCHOR,
//
//};

enum Api_Type
{
	BING_API,
	BEST_API,
	AIZHAN_API,
	ZZSKY_API
};

enum message
{
	MSG_FIND_URL = WM_USER+0x100,

	MSG_SWITCH_VIEW,

	MSG_SET_SPIDER_TARGET,
	MSG_SET_DEEPSCAN_TARGET,
	MSG_SET_PLUGIN_TARGET,

	MSG_INIT_VIEW,

	MSG_ADDSCAN_URL,
	MSG_ADDREPORT_ITEM,

};

enum view
{
	INFO_COLLECT_VIEW,
	WEAK_SPIDER_VIEW,
	WEAK_SCANNER_VIEW,
	DEEP_SCANNER_VIEW,
	WEAK_REPORT_VIEW,
	WEAK_AUDIT_VIEW,
	WEAK_PLUGIN_VIEW,
	WEAK_DEBUG_VIEW,
	WEAK_ASSIS_VIEW,
}; 

#define VIEW_COUNT	9

typedef struct _struct_domain_item_param
{
	char domain[256];
	char ip_address[32];
	char title[1024];
}struct_domain_item_param, *pstruct_domain_item_param;

typedef struct _struct_header_item_param
{
	int idx;
	char state[256];
	char server[256];
	char powerby[256];
}struct_header_item_param, *pstruct_header_item_param;

typedef struct _struct_signature_item_param
{
	int idx;
	char hash[64];
}struct_signature_item_param, *pstruct_signature_item_param;

typedef struct _struct_server_info_param
{
	int idx;
	char host[256];
	char ip_address[32];
}struct_server_info_param, *pstruct_server_info_param;
