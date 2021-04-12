#pragma once

#include <vector>
#include <string>
using namespace std;

//爬行模式
enum spider_mode {
	mode_frombase,	//只从指定的BaseUrl范围内爬行
	mode_fromhost,	//同时爬行本站范围
	mode_listall,	//爬行包括本站范围外的站点，但只列出域名，不继续爬行
	mode_fromall	//爬行包括本站范围外的站点
};

//爬行状态
enum spider_state {
	state_idle, 
	state_run, 
	state_stop
};
/*
enum spider_method {
	spider_method_get, 
	spider_method_post
};
*/
enum spider_error {
	spider_error_success,
	spider_error_finish,

	spider_error_stop,
	spider_error_running,
	spider_stop_running,

	// network_error
	spider_error_redirect, 
	spider_error_notfound, 
	spider_error_internet_error, 
	spider_error_header_too_long,
	spider_error_unknow_host,
	spider_error_not_format_header,
	spider_error_not_format_ip,

	// process_string_error
	spider_error_not_find,

	// process_url_error
	spider_error_invalid_url,
	
	
	// parse_error
	spider_error_tag_without_value,
	spider_error_not_target_tag,
	spider_error_tag_no_link,
	spider_error_emptyvalue,


	// process_url_query_error
	spider_error_urlexist
};

#define MAX_THREAD 5

typedef LRESULT (CALLBACK* NOTIFYPROC)(string url);

class NetSpider
{
public:
	NetSpider(void);
	~NetSpider(void);

	// setopt
	void setopt_deepth(int deepth = 0) { _deepth = deepth; } ;
	void setopt_owner(CWnd* owner = nullptr) { _owner = owner; } ;
	void setopt_baseurl(const string baseurl) { _baseurl = baseurl;} ;
	void setopt_download(bool download=false) { _download = download; } ;
	void setopt_mode(spider_mode mode = mode_frombase) { _mode = mode; } ;
	void setopt_notify(NOTIFYPROC pNotifyProc = nullptr) { _pNotifyProc = pNotifyProc; } ;
	void setopt_cookie(string cookie) { _cookie = cookie; } ;

	// getter

	// getstate
	bool is_running() { return _state==state_run; }

	// control
	spider_error init(bool forcestop = true);
	spider_error start();
	spider_error stop();

protected:
	static DWORD WINAPI spider_proc(LPVOID lpParam);
	spider_error static spider_querylist();
	long static request_get(const string& url, string& page_content);
	spider_error static request_post(const string& url, string& post_data, string& page_content);
	spider_error static parse_page(string content, const string& orig_url, int deepth);
	spider_error static parse_element(string& str_element, string& tag, string& value);
	spider_error static process_url(const string& orig_url, string value, string& url);
//	spider_error static addtolist(string url);

private:
	size_t static write_callback( void *ptr, size_t size, size_t nmemb, string &userp);
	string static makelower(string source);
	string static utf8togb(string source);
	void static getdir();
	void static gethost();

private:
	static CWnd*			_owner;
	static int				_deepth;
	static string			_baseurl;
	static string			_basedir;
	static string			_basehost;
	static string			_cookie;
	static bool				_download;
	static bool				_forcestop;
//	static bool				_stop;
	static spider_mode		_mode;
	static spider_state		_state;
	static NOTIFYPROC		_pNotifyProc;
	int						_thread_count;
	HANDLE					_thread[MAX_THREAD];
	
	static vector<string>	_querylist;
	static int				_querycount;
	static int				_queryidx;
	static HANDLE			_mutex;
};

