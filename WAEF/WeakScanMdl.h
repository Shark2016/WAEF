#pragma once

#include "ToolMdl.h"


#define MAX_SCAN_THREAD			10
#define MAX_WEAKNESS_COUNT		50

enum scanmdl_error
{
	scanmdl_error_success,
	scanmdl_error_weakfind,
	scanmdl_error_nullptr,
};

enum item_scan_state
{
	item_scan_state_waiting,
	item_scan_state_checking,
	item_scan_state_finish,
};

enum weakness_type
{
	weak_sqlinjection,
	weak_crosssitescript,
	weak_directorylist,
	weak_phperrorreport,
	weak_fileinclude,
	weak_codeinjection,
	weak_commandinjection,
	weak_sensitiveinfo_email,
	weak_sensitiveinfo_phpinfo,
	weak_pandingoracle,
};

enum weakness_level
{
	weakness_level_inform,
	weakness_level_low,
	weakness_level_medium,
	weakness_level_high,
};

enum target_method
{
	target_method_get,
	target_method_post
};
//
//enum url_type
//{
//	url_type_dir,
//	url_type_html,
//	url_type_asp,
//	url_type_aspx,
//	url_type_jsp,
//	url_type_php,
//	url_type_other
//};

enum policy_type
{
	policy_response_analysis,
	policy_sql_injection,
	policy_cross_site_script,
	policy_file_include,
	policy_code_injection,
	policy_command_injection
};

typedef struct _struct_schedule_item
{
	_struct_schedule_item(policy_type _type)
	{
		type = _type;
		pNext = nullptr;
	};
	policy_type					type;
	_struct_schedule_item*		pNext;
}struct_schedule_item, *pstruct_schedule_item;


typedef struct _struct_weakness_item
{
	_struct_weakness_item(weakness_type _type, weakness_level _level)
	{
		type  = _type;
		level = _level;
		pNext = nullptr;
	};
	weakness_type				type;
	weakness_level				level;
	string						payload;
	string						description;
	string						advice;
	_struct_weakness_item*		pNext;
}struct_weakness_item, *pstruct_weakness_item;


typedef struct _struct_scan_item
{
	_struct_scan_item(url_type _type, string _url, string _cookie)
	{
		type = _type;
		url = _url;
		cookie = _cookie;
		method = target_method_get;
		process = 0;
		schedule_count = 0;
		weakness_count = 0;
		pschedule_item = nullptr;
		pweakness_item = nullptr;
		state = item_scan_state_waiting;
	};
	_struct_scan_item(url_type _type, string _url, string _cookie, string _postdata)
	{
		type = _type;
		url = _url;
		cookie = _cookie;
		postdata = _postdata;
		method = target_method_post;
		process = 0;
		schedule_count = 0;
		weakness_count = 0;
		pschedule_item = nullptr;
		pweakness_item = nullptr;
		state = item_scan_state_waiting;
	};
	int							idx;
	target_method				method;
	url_type					type;
	string						url;
	string						cookie;
	bool						param;				// 是否带参数(GET方式下)
	string						postdata;
	item_scan_state				state;
	int							process;
	pstruct_schedule_item		pcurrent_item;		//当前检测进度
	int							schedule_count;
	pstruct_schedule_item		pschedule_item;
	int							weakness_count;
	pstruct_weakness_item		pweakness_item;
} struct_scan_item, *pstruct_scan_item;

class CWeakScanMdl
{
public:
	CWeakScanMdl(void);
	~CWeakScanMdl(void);

public:
	// setopt
	void setopt_thread(int thread_count = 1) { _thread_count = thread_count>MAX_SCAN_THREAD ? MAX_SCAN_THREAD:thread_count; }

public:
	scanmdl_error init(CListCtrl* scanlist, CListBox* loglist);
	scanmdl_error start();
	scanmdl_error stop();
	static scanmdl_error init_schedule(pstruct_scan_item pscan_item);
	static scanmdl_error update_item_state(pstruct_scan_item pscan_item);

protected:
	static DWORD WINAPI scan_proc(LPVOID lpParam);
	static scanmdl_error response_analysis(pstruct_scan_item pscan_item);
	static scanmdl_error check_sqlinjection(pstruct_scan_item pscan_item);
	static scanmdl_error check_crosssitescript(pstruct_scan_item pscan_item);
	static scanmdl_error check_fileinclude(pstruct_scan_item pscan_item);
	static scanmdl_error check_codeinjection(pstruct_scan_item pscan_item);
	static scanmdl_error check_commandinjection(pstruct_scan_item pscan_item);

private:
	static bool isnumric(string value);
	static string replace(string find, string replace, string source);
	static string urlencode(string source);
	static void addLog(CString log);

private:
	static int				_thread_count;
	static int				_item_count;
	static int				_current_item;
	static HANDLE			_thread[MAX_SCAN_THREAD];
	static CListCtrl*		_scanlist;
	static CListBox*		_loglist;
	static HANDLE			_mutex;
};

