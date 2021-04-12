#include "stdafx.h"
#include "ToolMdl.h"

#include "md5.h"


#define USER_AGENT "Mozillia"
#define BUFF_SIZE 2048

CToolMdl::CToolMdl(void)
{
}


CToolMdl::~CToolMdl(void)
{
}

size_t CToolMdl::write_callback( void *ptr, size_t size, size_t nmemb, string &userp)
{
	long sizes = size * nmemb;
    string temp((char*)ptr, sizes);
    userp += temp; 
    return sizes;
}

long CToolMdl::request_get(const string& url, string& content, string cookie)
{
	CURLcode res;
	CURL *curl = nullptr;
	long retcode = 0;
	curl = curl_easy_init();
	if(curl)
	{
		content.clear();
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); 
		if(!cookie.empty())
		{
			//res = curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie.txt"); //这种方式失败，可能是cookie写入格式没弄清
			res = curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str());
		}
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
		//curl_easy_setopt(curl, CURLOPT_PROXY, "localhost:8080"); 
		res = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
		curl_easy_cleanup(curl);
		if(res == CURLE_COULDNT_CONNECT)
		{
			retcode=-1;
		}
	}
	return retcode;
}

long CToolMdl::request_post(const string& url, string& post_data, string& content, string cookie)
{
	CURLcode res;
	CURL *curl = nullptr;
	long retcode = 0;
	curl = curl_easy_init();
	if(curl)
	{
		content.clear();
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); 
		if(!cookie.empty())
		{
			//res = curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie.txt"); //这种方式失败，可能是cookie写入格式没弄清
			res = curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str());
		}
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		//curl_easy_setopt(curl, CURLOPT_PROXY, "localhost:8080"); 
		res = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
		curl_easy_cleanup(curl);
		if(res == CURLE_COULDNT_CONNECT)
		{
			retcode=-1;
		}
	}
	return retcode;
}

toolmdl_error CToolMdl::request_head(const string& url, string& header)
{
	CURLcode res;
	CURL *curl = nullptr;
	curl = curl_easy_init();
	if(curl)
	{
		header.clear();
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); 
		curl_easy_setopt(curl, CURLOPT_NOBODY ,1L );
		res = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_callback);
		res = curl_easy_setopt(curl, CURLOPT_WRITEHEADER, &header);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return toolmdl_error_success;
}

size_t CToolMdl::signature_callback( void *ptr, size_t size, size_t nmemb, void *userp)
{
	long sizes = size * nmemb;
	CryptHashObject* hasher = (CryptHashObject*)userp;
	// Hash this chunk of data
	hasher->HashData(ptr, sizes);
	//userp = hash_update(ptr, sizes);
    return sizes;
}

long CToolMdl::signature(const string& url, string& signature)
{
	CURLcode res;
	CURL *curl = nullptr;
	long retcode=0;
	curl = curl_easy_init();
	if(curl)
	{
		
		// Create the algorithm provider for SHA-1 hashing
		CryptAlgorithmProvider md5;

		// Create the hash object for the particular hashing
		CryptHashObject hasher(md5);

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); 
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, signature_callback);
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &hasher);
		//curl_easy_setopt(curl, CURLOPT_PROXY, "localhost:8080"); 

		res = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
		curl_easy_cleanup(curl);
		
		// Finalize hashing
		hasher.FinishHash();

		// Return hash digest
		signature = hasher.HashDigest();
		//strcpy_s(signature, 64, hasher.HashDigest().c_str());

		if(res == CURLE_COULDNT_CONNECT)
		{
			retcode=-1;
		}
	}
	return retcode;
}

toolmdl_error CToolMdl::parse_header(const string& header, string& state, string& server, string& powerby)
{
	//解析Header
	int pos1 , pos2, pos3;
	pos1 = header.find(_T("\r\n"));
	if(pos1==-1)
	{
		return toolmdl_error_notstandardheader;
	}
	state = header.substr(0, pos1);
	string item;
	while(header.at(pos1+2)!='\r')
	{
		pos2 = header.find(_T("\r\n"),pos1+2);
		item = header.substr(pos1+2, pos2-pos1-1);
		pos3 = item.find(':');
		if(pos3!=-1)
		{
			if(item.substr(0, pos3)=="Server")
			{
				server=item.substr(pos3+2, item.length()-pos3-2);
			}
			else if(item.substr(0,pos3)=="X-Powered-By")
			{
				powerby=item.substr(pos3+2, item.length()-pos3-2);
			}
		}
		pos1 = pos2;
	}
	return toolmdl_error_success;
}
	
toolmdl_error CToolMdl::sub_string(const string& content, const string& start, const string& stop, string& rtn, int& startpos)
{
	int pos;
	int len=start.length();
	if((startpos=content.find(start, startpos))==-1)
	{
		return toolmdl_error_notfind;
	}
	if((pos=content.find(stop, startpos+len))==-1)
	{
		return toolmdl_error_notfind;
	}
	rtn = content.substr(startpos+len, pos-startpos-len);
	return toolmdl_error_success;
}

string CToolMdl::utf8togb(string source)
{
	int len = source.length();
	wchar_t *szUnic=new wchar_t[len];
	char *szBuf=new char[len];
	MultiByteToWideChar(CP_UTF8, 0, source.c_str(), -1, szUnic, len);
	WideCharToMultiByte(CP_ACP, 0, szUnic, -1, szBuf, len, NULL, NULL);
	string rtn = szBuf;
	delete [] szUnic;
	delete [] szBuf;
	return rtn;
}

toolmdl_error CToolMdl::resolve_ip(char* host_name, char* ip_address, const size_t size_ip_address)
{
	HOSTENT *host_entry;
	host_entry=gethostbyname(host_name);
	if(host_entry==nullptr)
	{
		return toolmdl_error_unknowhost;
	}
	
	sprintf_s(ip_address, size_ip_address, "%d.%d.%d.%d",
		(host_entry->h_addr_list[0][0]&0x00ff),
		(host_entry->h_addr_list[0][1]&0x00ff),
		(host_entry->h_addr_list[0][2]&0x00ff),
		(host_entry->h_addr_list[0][3]&0x00ff));
	
	return toolmdl_error_success;
}
/*
Error_code CToolMdl::parse_url(const CString& url, CString& protocol, char* host_name,const size_t size_host_name, char* ip_address,const size_t size_ip_address, int& port, CString& path)
{
	int pos1, pos2;
	CString domain;
	pos1=url.Find("://");
	if(pos1==-1)
	{
		return error_invalid_url;
	}
	protocol = url.Mid(0, pos1+3);
	pos2=url.Find('/', pos1+3);
	if(pos2==-1)
	{
		domain=url.Right(url.GetLength()-pos1-3);
		path = "/";
	}
	else
	{
		path = url.Mid(pos2, url.GetLength()-pos2);
		domain=url.Mid(pos1+3, pos2-pos1-3);
	}
	if((pos1=domain.Find(':'))>0)
	{
		port = atoi(domain.Right(pos1).GetBuffer());
		domain=domain.Left(pos1-1);
	}
	else
	{
		port = 80;
	}

	size_t size= strlen(host_name);
	strcpy_s(host_name, size_host_name, domain.GetBuffer()); 
	HOSTENT *host_entry;
	host_entry=gethostbyname(host_name);
	if(host_entry==nullptr)
	{
		return error_unknow_host;
	}
	
	sprintf_s(ip_address, size_ip_address, "%d.%d.%d.%d",
		(host_entry->h_addr_list[0][0]&0x00ff),
		(host_entry->h_addr_list[0][1]&0x00ff),
		(host_entry->h_addr_list[0][2]&0x00ff),
		(host_entry->h_addr_list[0][3]&0x00ff));

	return error_success;
}
*/