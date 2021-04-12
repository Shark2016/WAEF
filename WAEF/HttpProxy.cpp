#include "stdafx.h"
#include "HttpProxy.h"

CHttpProxy::CHttpProxy(void)
{
}


CHttpProxy::~CHttpProxy(void)
{
}


httpproxy_error_code CHttpProxy::initproxy()
{
	_option.port = 8080;
	_option.tunnel_count = 0;
	WSAData wsaDat;
	WSAStartup(MAKEWORD(1,1),&wsaDat);

	_option.hMutex = CreateMutex(NULL,FALSE,NULL);
	return httpproxy_error_success;
}

httpproxy_error_code CHttpProxy::startproxy()
{
	_main_thread = CreateThread(NULL, 0, listen_proc, (LPVOID)&_option, 0, NULL);
	return httpproxy_error_success;
}

httpproxy_error_code CHttpProxy::stopproxy()
{
	closesocket(_option.socket);
	int i;
	for(i=0;i<_option.tunnel_count;i++)
	{
		closesocket(_option.tunnel[i].client);
		closesocket(_option.tunnel[i].server);
	}
	return httpproxy_error_success;
}

DWORD WINAPI CHttpProxy::listen_proc(LPVOID lpParam)
{
	ProxyOption* pOption = (ProxyOption*)lpParam;
	pOption->socket =socket(AF_INET,SOCK_STREAM,0);
	
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
	addrSrv.sin_family=AF_INET;
	addrSrv.sin_port=htons(pOption->port);
 
	if(SOCKET_ERROR == bind(pOption->socket,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR)))
		return 0;

	listen(pOption->socket,5);
 
	SOCKADDR_IN addrClient;	
	int len=sizeof(SOCKADDR);

	SOCKET sockConn;

	do
	{
		sockConn=accept(pOption->socket,(SOCKADDR*)&addrClient,&len);
		Tunnel* pTunnel = &pOption->tunnel[pOption->tunnel_count++];
		pTunnel->pOption = lpParam;
		pTunnel->client = sockConn;
		pTunnel->connect = false;
		pTunnel->inThread = CreateThread(NULL, 0, proxy_proc, (LPVOID)pTunnel, 0, NULL);
	}while(sockConn!=INVALID_SOCKET);
	closesocket(pOption->socket);
	return 0;
}

bool CHttpProxy::rebuildRequest(Tunnel* pTunnel)//string& real_request, string& host, int& port)
{
	int pos1 = pTunnel->proxy_request.find(" ");
	int pos2 = pTunnel->proxy_request.find(" ", pos1+1);
	string url = pTunnel->proxy_request.substr(pos1+1, pos2-pos1-1);
	int pos3 = url.find('/', 7) + pos1+1;
	pTunnel->request = pTunnel->proxy_request.substr(0, pos1+1) + pTunnel->proxy_request.substr(pos3);

	pos1 = pTunnel->request.find("Host: ");
	pos2 = pTunnel->request.find("\r\n", pos1);
	pTunnel->host = pTunnel->request.substr(pos1 + 6, pos2 - pos1 -6);
	pos3 = pTunnel->host.find(':');
	pTunnel->port = 80;
	if(pos3!=-1)
	{
		pTunnel->port = atoi(pTunnel->host.substr(0, pos3).c_str());
		pTunnel->host = pTunnel->host.substr(0, pos3);
	}
	return true;
}

unsigned long CHttpProxy::LookupAddress(const char* host)
{
	unsigned long addr = inet_addr(host);
	HOSTENT *phe;

	if (addr == INADDR_NONE)
	{
		phe = gethostbyname(host);
		if (!phe)
			return INADDR_NONE;
		addr = *((unsigned long *) phe->h_addr_list[0]);
	}

	return addr;
}

SOCKET CHttpProxy::connectTarget(Tunnel* pTunnel)
{
	SOCKADDR_IN sin;
	SOCKET sd;
	unsigned long addr;

	if ((addr = LookupAddress(pTunnel->host.c_str())) == INADDR_NONE)
		return INVALID_SOCKET;

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) != INVALID_SOCKET)
	{
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = addr;
		sin.sin_port = htons((unsigned short) pTunnel->port);
		if (connect(sd, (struct sockaddr FAR *) &sin, sizeof(sin)) == SOCKET_ERROR)
			return INVALID_SOCKET;
	}
	pTunnel->connect = true;
	return sd;
}

DWORD WINAPI CHttpProxy::proxy_proc(LPVOID lpParam)
{
	Tunnel* pTunnel =  (Tunnel*)lpParam;
	ProxyOption* pOption = (ProxyOption*)pTunnel->pOption;
	
	char recvBuf[BUFF_SIZE];
	int retval;
	do
	{
		ZeroMemory(recvBuf, sizeof(recvBuf));
		retval = recv(pTunnel->client,recvBuf,BUFF_SIZE,0);
		if(retval > 0)
		{
			pTunnel->proxy_request = string(recvBuf, retval);
			if(IsRequest(pTunnel->proxy_request))
			{
				if(rebuildRequest(pTunnel))
				{
					if(!pTunnel->connect && pOption->mode != httpproxy_mode_httpdebug)
					{
						pTunnel->server = connectTarget(pTunnel);
						pTunnel->outThread = CreateThread(NULL, 0, forward_proc, (LPVOID)pTunnel, 0, NULL);
					}
				}
			}
			else
			{
				pTunnel->request += string(recvBuf, retval);
			}
			
			int nSent = 0, nTmp;
			int len;
			const char* sendBuf;
			if(pOption->mode == httpproxy_mode_httpdebug)
			{
				if(retval==BUFF_SIZE)		// 一个数据包是否完整，暂时这么判断吧，有问题
					continue;
				else
				{
					WaitForSingleObject(pOption->hMutex, INFINITE);
					SendMessage(pOption->owner, MSG_PROXY_NOTIFY, 0, 0);
					int pos1 = pTunnel->request.find("Accept-Encoding: ");
					if(pos1 != -1)
					{
						int pos2 = pTunnel->request.find("\r\n", pos1);
						pTunnel->request = pTunnel->request.substr(0, pos1) + pTunnel->request.substr(pos2+2);
					}
					pOption->pNotifyProc(pTunnel->request, pOption->mode);
					ReleaseMutex(pOption->hMutex);
					
					if(!pTunnel->connect)
					{
						pTunnel->server = connectTarget(pTunnel);
						pTunnel->outThread = CreateThread(NULL, 0, forward_proc, (LPVOID)pTunnel, 0, NULL);
					}

					sendBuf = pTunnel->request.c_str();
					len = pTunnel->request.length();
				}
			}
			else
			{
				sendBuf = recvBuf;
				len = retval;
			}

			while (nSent < len)
			{
				nTmp = send(pTunnel->server, sendBuf, len - nSent, 0);
				if (nTmp > 0)
					nSent += nTmp;
				else if (nTmp == SOCKET_ERROR)
					break;
				else
					break;
			}
		}
		else if(retval == SOCKET_ERROR)
		{
			break;
		}
	} while(retval !=0);

	return 0;
}


DWORD WINAPI CHttpProxy::forward_proc(LPVOID lpParam)
{
	Tunnel* pTunnel =  (Tunnel*)lpParam;
	ProxyOption* pOption=(ProxyOption*)pTunnel->pOption;

	char buf[BUFF_SIZE];
	int nRead, nSent, nTmp;
	int packet = 0;
	do
	{
		ZeroMemory(buf, sizeof(buf));
		nRead = recv(pTunnel->server, buf, BUFF_SIZE, 0);

		if (nRead > 0)
		{

			if(buf[0] == 'H' && buf[1] == 'T' && buf[2] == 'T' && buf[3] == 'P' && buf[4] == '/' && buf[5] == '1')	// "HTTP/1"开头
			{
				pTunnel->response = string(buf, nRead);
			}
			else
			{
				pTunnel->response += string(buf, nRead);
			}
		
			switch(pOption->mode)
			{
			case httpproxy_mode_httpdebug:
				pOption->pNotifyProc(pTunnel->response, httpproxy_mode_notifyresp);
				break;
			case httpproxy_mode_getcookie:
			case httpproxy_mode_getrequest:
				pOption->pNotifyProc(pTunnel->request, pOption->mode);
				break;
			case httpproxy_mode_httpproxy:
			default:
				break;
			}

			nSent = 0;
			while (nSent < nRead)
			{
				nTmp = send(pTunnel->client, buf + nSent, nRead - nSent, 0);
				if (nTmp > 0)
					nSent += nTmp;
				else if (nTmp == SOCKET_ERROR)
					break;
				else
					break;
			}
		}
		else if (nRead == SOCKET_ERROR)
			break;
	} while (nRead != 0);

	ShutdownConnection(pTunnel->client);
	ShutdownConnection(pTunnel->server);

	return 0;
}

BOOL CHttpProxy::ShutdownConnection(SOCKET sd)
{
	char buf[BUFF_SIZE];
	int nNew;

	if (shutdown(sd, SD_SEND) == SOCKET_ERROR)
		return FALSE;

	while (1)
	{
		nNew = recv(sd, buf, BUFF_SIZE, 0);
		if (nNew == SOCKET_ERROR)
			return FALSE;
		else if (nNew != 0)
			continue;
		else
			break;
	}

	if (closesocket(sd) == SOCKET_ERROR)
		return FALSE;

	return TRUE;
}

bool CHttpProxy::IsRequest(string& req)
{
	string method = req.substr(0, 4);
	if(method == "GET " || method == "POST" || method == "CONN")
		return true;
	else
		return false;
}
