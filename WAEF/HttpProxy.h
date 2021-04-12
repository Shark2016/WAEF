#pragma once

#include <string>
using namespace std;

#define	MAX_TUNNEL_COUNT	1000
#define BUFF_SIZE			1024
#define MSG_PROXY_NOTIFY	WM_USER+4096

enum httpproxy_error_code
{
	httpproxy_error_success,
	httpproxy_error_bindfail
};

enum httpproxy_mode
{
	httpproxy_mode_httpproxy,
	httpproxy_mode_httpdebug,
	httpproxy_mode_getcookie,
	httpproxy_mode_getrequest,

	httpproxy_mode_notifyresp,	//非模式，只是用来调试时发送响应到界面
};

typedef LRESULT (CALLBACK* PROXY_NOTIFYPROC)(string& data, httpproxy_mode mode);

typedef struct
{
	SOCKET	client;
	SOCKET	server;
	HANDLE	inThread;
	HANDLE	outThread;
	bool	connect;
	PVOID	pOption;
	string	host;
	int		port;
	string	proxy_request;
	string	url;
	string	request;
	string  response;
}Tunnel;

typedef struct
{
	SOCKET				socket;
	int					port;
	HWND				owner;
	httpproxy_mode		mode;
	int					tunnel_count;
	HANDLE				hMutex;
	Tunnel				tunnel[MAX_TUNNEL_COUNT];
	PROXY_NOTIFYPROC	pNotifyProc;
}ProxyOption;


class CHttpProxy
{
public:
	CHttpProxy(void);
	~CHttpProxy(void);

public:
	httpproxy_error_code initproxy();

	void setopt_port(int port) {_option.port = port;};
	void setopt_owner(HWND hOwner) {_option.owner = hOwner;};
	void setopt_mode(httpproxy_mode mode) { _option.mode = mode; } ;
	void setopt_notify(PROXY_NOTIFYPROC pNotifyProc = nullptr) { _option.pNotifyProc = pNotifyProc; } ;
	httpproxy_error_code startproxy();
	httpproxy_error_code stopproxy();


private:
	static DWORD WINAPI listen_proc(LPVOID lpParam);
	static DWORD WINAPI proxy_proc(LPVOID lpParam);
	static DWORD WINAPI forward_proc(LPVOID lpParam);

	static bool rebuildRequest(Tunnel* pTunnel);
	static SOCKET connectTarget(Tunnel* pTunnel);
	static unsigned long LookupAddress(const char* host);
	static BOOL ShutdownConnection(SOCKET sd);
	static bool IsRequest(string& req);

	ProxyOption		_option;


	HANDLE			_main_thread;
};