#include "stdafx.h"
#include "InfoCollMdl.h"

CListCtrl* CInfoCollMdl::_infolist = nullptr;
HANDLE CInfoCollMdl::_mutex_list;

int CInfoCollMdl::_idx_bing;
HANDLE CInfoCollMdl::_mutex_bing;

int CInfoCollMdl::_idx_114best;
HANDLE CInfoCollMdl::_mutex_best;

int CInfoCollMdl::_idx_aizhan;
HANDLE CInfoCollMdl::_mutex_aizhan;

int CInfoCollMdl::_idx_zzsky;
HANDLE CInfoCollMdl::_mutex_zzsky;

int CInfoCollMdl::_idx_server;
HANDLE CInfoCollMdl::_mutex_server;

int CInfoCollMdl::_idx_signature;
HANDLE CInfoCollMdl::_mutex_signature;

vector<string> CInfoCollMdl::_targetquery;
HANDLE CInfoCollMdl::_mutex_query;

TiXmlDocument* CInfoCollMdl::_pTiXmlDoc;
TiXmlElement*  CInfoCollMdl::_pTiXmlElem;
HANDLE CInfoCollMdl::_mutex_tixml;


CInfoCollMdl::CInfoCollMdl(void)
{
	_mutex_list=CreateMutex(NULL,FALSE,NULL);
	_mutex_bing=CreateMutex(NULL,FALSE,NULL);
	_mutex_best=CreateMutex(NULL,FALSE,NULL);
	_mutex_aizhan=CreateMutex(NULL,FALSE,NULL);
	_mutex_zzsky=CreateMutex(NULL,FALSE,NULL);
	_mutex_server=CreateMutex(NULL,FALSE,NULL);
	_mutex_signature=CreateMutex(NULL,FALSE,NULL);
	_mutex_query=CreateMutex(NULL,FALSE,NULL);
	_mutex_tixml=CreateMutex(NULL,FALSE,NULL);
}


CInfoCollMdl::~CInfoCollMdl(void)
{
}

infomdl_error_code CInfoCollMdl::init_targetquery(string& ip_address)
{
	int pos1 = ip_address.find('.');
	int pos2 = ip_address.find('.',pos1+1);
	int pos3 = ip_address.find('.',pos2+1);
	if(pos1== -1 || pos2 == -1 || pos3 == -1)
		return infomdl_error_notformatip;
	string pre = ip_address.substr(0, pos3+1);
	string ip;
	int m4=atoi(ip_address.substr(pos3+1).c_str());
	int i;
	char tmp[5];
	WaitForSingleObject(_mutex_query,INFINITE);
	_targetquery.clear();
	_targetquery.push_back(ip_address);
	for(i=1;i<255;i++)
	{
		if(i!=m4)
		{
			sprintf_s(tmp, 5, "%d", i);
			ip = pre + tmp;
			_targetquery.push_back(ip);
		}
	}
	ReleaseMutex(_mutex_query);
	
	WaitForSingleObject(_mutex_bing,INFINITE);
	_idx_bing = 0;
	ReleaseMutex(_mutex_bing);
	WaitForSingleObject(_mutex_best,INFINITE);
	_idx_114best = 0;
	ReleaseMutex(_mutex_best);
	WaitForSingleObject(_mutex_aizhan,INFINITE);
	_idx_aizhan = 0;
	ReleaseMutex(_mutex_aizhan);
	WaitForSingleObject(_mutex_zzsky,INFINITE);
	_idx_zzsky = 0;
	ReleaseMutex(_mutex_zzsky);

	WaitForSingleObject(_mutex_server,INFINITE);
	_idx_server = 0;
	ReleaseMutex(_mutex_server);
	WaitForSingleObject(_mutex_signature,INFINITE);
	_idx_signature = 0;
	ReleaseMutex(_mutex_signature);

	return infomdl_error_success;
}

infomdl_error_code CInfoCollMdl::clear_list()
{
	WaitForSingleObject(_mutex_list,INFINITE);
	_infolist->DeleteAllItems();
	ReleaseMutex(_mutex_list);
	WaitForSingleObject(_mutex_server,INFINITE);
	_idx_server = 0;
	ReleaseMutex(_mutex_server);
	WaitForSingleObject(_mutex_signature,INFINITE);
	_idx_signature = 0;
	ReleaseMutex(_mutex_signature);
	return infomdl_error_success;
}

infomdl_error_code CInfoCollMdl::get_info(Api_Type api_type, int threadnum)
{
	LPTHREAD_START_ROUTINE proc = nullptr;
	switch(api_type)
	{
	case BING_API:
		proc = (LPTHREAD_START_ROUTINE)get_info_by_bing;
		break;
	case BEST_API:
		proc = (LPTHREAD_START_ROUTINE)get_info_by_114best;
		break;
	case AIZHAN_API:
		proc = (LPTHREAD_START_ROUTINE)get_info_by_aizhan;
		break;
	case ZZSKY_API:
		proc = (LPTHREAD_START_ROUTINE)get_info_by_zzsky;
		break;
	}
	if(proc == nullptr)
		return infomdl_error_success;
	HANDLE hThread = NULL;
	int i;
	for(i=0;i<threadnum;i++)
	{
		hThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)proc, 0, 0, NULL);
		CloseHandle(hThread);
	}
	return infomdl_error_success;
}

infomdl_error_code CInfoCollMdl::get_info_by_bing(LPVOID lpParam)
{
	string ip_address;
	string url;
	string buffer;
	string domain;
	string title;
	char format[128];
	CToolMdl tool;
	while(1)
	{
		WaitForSingleObject(_mutex_query,INFINITE);
		if(_targetquery.empty())
		{
			ReleaseMutex(_mutex_query);
			return infomdl_error_finish;
		}
		WaitForSingleObject(_mutex_bing,INFINITE);
		if(_idx_bing>=254)
		{
			ReleaseMutex(_mutex_bing);
			return infomdl_error_finish;
		}
		ip_address = _targetquery.at(_idx_bing++);
		ReleaseMutex(_mutex_bing);
		ReleaseMutex(_mutex_query);

		int n=1;
		do {
			sprintf_s(format, 128, "http://cn.bing.com/search?first=%d&count=50&q=ip:%s", n, ip_address.c_str());
			url = format;
			if(0!=tool.request_get(url, buffer))
			{
				int nPos=0;
				int nPos2=0;
				buffer = tool.utf8togb(buffer);
				do {
					if(error_success!=tool.sub_string(buffer, _T("<h3><a href=\""),_T("\" target="),domain, nPos))
					{
						break;
					}
					if(error_success!=tool.sub_string(buffer, _T("\">"),_T("</a></h3>"),title, nPos))
					{
						break;
					}
					nPos++;
					if((nPos2=domain.find('/', 7))!=-1)
					{
						domain=domain.substr(0, nPos2);
					}
					InsertItem(domain, ip_address, title);
				}while(1);
				n+=50;
				if(buffer.find(_T("下一页"))==-1)
				{
					break;
				}
			}
		}while(1);
	}
}

int CInfoCollMdl::export_list(CString path)
{
	WaitForSingleObject(_mutex_list, INFINITE);
	int i=0;
	int count=_infolist->GetItemCount();	
	CStdioFile stdfile;
	stdfile.Open(path,CFile::modeCreate|CFile::modeWrite);
	while(i<count)
	{
		stdfile.WriteString(_infolist->GetItemText(i++,0) + "\r\n");
	}	
	stdfile.Close();
	ReleaseMutex(_mutex_list);
	return i;
}

int CInfoCollMdl::import_list(CString path)
{
	WaitForSingleObject(_mutex_list, INFINITE);
	CString domain;
	CStdioFile stdfile;
	stdfile.Open(path,CFile::modeRead);
	BOOL b=stdfile.ReadString(domain);
	int cnt = _infolist->GetItemCount();
	int i = 0;
	while(b)
	{
		_infolist->InsertItem(cnt+i, domain);
		b=stdfile.ReadString(domain);
		i++;
	};
	stdfile.Close();
	ReleaseMutex(_mutex_list);
	return i;
}

infomdl_error_code CInfoCollMdl::get_info_cduan()
{
	/*
	int pos1 = ip_address.find('.');
	int pos2 = ip_address.find('.',pos1+1);
	int pos3 = ip_address.find('.',pos2+1);
	if(pos1== -1 || pos2 == -1 || pos3 == -1)
		return error_not_format_ip;
	int m1=atoi(ip_address.substr(0, pos1).c_str());
	int m2=atoi(ip_address.substr(pos1+1, pos2-pos1+1).c_str());
	int m3=atoi(ip_address.substr(pos2+1, pos3-pos2+1).c_str());
	int m4=atoi(ip_address.substr(pos3+1).c_str());
	int i;
	for(i=1;i<255;i++)
	{
		if(i!=m4)
		{
			char* ip_address = new char[32];
			sprintf_s(ip_address, 32, "%d.%d.%d.%d", m1, m2, m3, i);
			HANDLE hThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)get_info_by_bing, ip_address, 0, NULL);
			CloseHandle(hThread);
		}
	}
	return error_success;
	*/
	return infomdl_error_success;
}

infomdl_error_code CInfoCollMdl::get_info_by_114best(LPVOID lpParam)
{
	string ip_address;
	string url;
	string buffer;
	string domain;
	string title;
	CToolMdl tool;
	while(1)
	{
		WaitForSingleObject(_mutex_query,INFINITE);
		if(_targetquery.empty())
		{
			ReleaseMutex(_mutex_query);
			return infomdl_error_finish;
		}
		WaitForSingleObject(_mutex_best,INFINITE);
		if(_idx_114best>=254)
		{
			ReleaseMutex(_mutex_best);
			return infomdl_error_finish;
		}
		ip_address = _targetquery.at(_idx_114best++);
		ReleaseMutex(_mutex_best);
		ReleaseMutex(_mutex_query);

		url = "http://www.114best.com/ip/114.aspx?w=" + ip_address;
		
		if(0!=tool.request_get(url, buffer))
		{
			int nPos=0;
			int nPos2=0;
			buffer = tool.utf8togb(buffer);
			do {
				if(error_success!=tool.sub_string(buffer, _T("window.open('"),_T("')"),domain, nPos))
				{
					break;
				}
				if(error_success!=tool.sub_string(buffer, _T("</a><br>\r\n"),_T("<br><br>\r\n"),title, nPos))
				{
					break;
				}
				nPos++;
				if((nPos2=domain.find('/', 7))!=-1)
				{
					domain=domain.substr(0, nPos2);
				}
					
				InsertItem(domain, ip_address, title);
			}while(1);
		}
	}
	return infomdl_error_success;
}

infomdl_error_code CInfoCollMdl::get_info_by_aizhan(LPVOID lpParam)
{
	string ip_address;
	string url;
	string buffer;
	string domain;
	char format[128];
	CToolMdl tool;
	while(1)
	{
		
		WaitForSingleObject(_mutex_query,INFINITE);
		if(_targetquery.empty())
		{
			ReleaseMutex(_mutex_query);
			return infomdl_error_finish;
		}
		WaitForSingleObject(_mutex_aizhan,INFINITE);
		if(_idx_aizhan>=254)
		{
			ReleaseMutex(_mutex_aizhan);
			return infomdl_error_finish;
		}
		ip_address = _targetquery.at(_idx_aizhan++);
		ReleaseMutex(_mutex_aizhan);
		ReleaseMutex(_mutex_query);

		int n=1;

		do {
			sprintf_s(format, 128, "http://dns.aizhan.com/index.php?r=index/getress&q=%s&page=%d", ip_address.c_str(), n);
			url = format;
			if(0!=tool.request_get(url, buffer))
			{
				int nPos=0;
				int nPos2=0;
				do {
					if(error_success!=tool.sub_string(buffer, _T("domain\":\""),_T("\",\"pr\":\""),domain, nPos))
					{
						break;
					}
					domain = "http://"+domain;
					InsertItem(domain, ip_address, string(""));
					nPos++;
				}while(1);
				n++;
				if(buffer.find(_T("[]"))!=-1)
				{
					break;
				}
			}
		}while(1);
	}
	return infomdl_error_success;
}

infomdl_error_code CInfoCollMdl::get_info_by_zzsky(LPVOID lpParam)
{
	string redirctcurl;
	string ip_address;
	string url;
	string buffer;
	string domain;
	string title;
	CToolMdl tool;

	while(1)
	{
		WaitForSingleObject(_mutex_query,INFINITE);
		if(_targetquery.empty())
		{
			ReleaseMutex(_mutex_query);
			return infomdl_error_finish;
		}
		WaitForSingleObject(_mutex_zzsky,INFINITE);
		if(_idx_zzsky>=254)
		{
			ReleaseMutex(_mutex_zzsky);
			return infomdl_error_finish;
		}
		ip_address = _targetquery.at(_idx_zzsky++);
		ReleaseMutex(_mutex_zzsky);
		ReleaseMutex(_mutex_query);

		url = "http://www.zzsky.cn/tool/sameip/index.php?q=" + ip_address;
		if(0!=tool.request_get(url, buffer))
		{
			int nPos=0;
			if(error_success==tool.sub_string(buffer, _T("<a href=\""),_T("\">"),redirctcurl, nPos))
			{
				redirctcurl = "http://www.zzsky.cn" + redirctcurl;
				Sleep(3000);
				if(0!=tool.request_get(redirctcurl, buffer))
				{
					buffer = tool.utf8togb(buffer);
					int nPos=0;
					int nPos2=0;
					do {
						if(error_success!=tool.sub_string(buffer, _T("<h3><a href=\""),_T("\""),domain, nPos))
						{
							break;
						}
						if(error_success!=tool.sub_string(buffer, _T("target=\"_blank\">"),_T("</b></b>"),title, nPos))
						{
							break;
						}
						nPos++;
						if((nPos2=domain.find('/', 7))!=-1)
						{
							domain=domain.substr(0, nPos2);
						}
						InsertItem(domain, ip_address, title);
					}while(1);
				}
			}
		}
	}
	return infomdl_error_success;
}

infomdl_error_code CInfoCollMdl::get_server_info()
{
	HANDLE hThread;
	hThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)get_server_header, 0, 0, NULL);
	CloseHandle(hThread);
	hThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)get_cms_signature, 0, 0, NULL);
	CloseHandle(hThread);
	hThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)get_cms_signature, 0, 0, NULL);
	CloseHandle(hThread);
	hThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)get_cms_signature, 0, 0, NULL);
	CloseHandle(hThread);
	hThread=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)get_cms_signature, 0, 0, NULL);
	CloseHandle(hThread);
	return infomdl_error_success;
}

infomdl_error_code CInfoCollMdl::get_server_header(LPVOID lParam)
{
	int idx;
	string domain;
	while(1)
	{
		WaitForSingleObject(_mutex_list,INFINITE);
		int count=_infolist->GetItemCount();
		WaitForSingleObject(_mutex_server,INFINITE);
		if(_idx_server >=count)
		{
			ReleaseMutex(_mutex_server);
			ReleaseMutex(_mutex_list);
			return infomdl_error_finish;
		}
		idx = _idx_server++;
		ReleaseMutex(_mutex_server);
		domain = _infolist->GetItemText(idx, 0).GetBuffer();
		ReleaseMutex(_mutex_list);

		CToolMdl tool;
		string header, state, server, powerby;
		string serverinfo;
		if(toolmdl_error_success == tool.request_head(domain, header))
		{
			if(toolmdl_error_success == tool.parse_header(header, state, server, powerby))
			{
				serverinfo = server + " " + powerby;
				WaitForSingleObject(_mutex_list,INFINITE);
				_infolist->SetItemText(idx, 3, serverinfo.c_str());
				ReleaseMutex(_mutex_list);
			}
		}
	}
	return infomdl_error_success;
}

infomdl_error_code CInfoCollMdl::get_cms_signature(LPVOID lParam)
{
	int idx;
	int type;
	string url;
	string domain;
	int itemData;
	TiXmlElement *pTiXmlElem = nullptr, *pSubTiXmlElem = nullptr;
	while(1)
	{
		WaitForSingleObject(_mutex_list,INFINITE);
		int count=_infolist->GetItemCount();

		WaitForSingleObject(_mutex_signature,INFINITE);
		if(_idx_signature >=count)
		{
			ReleaseMutex(_mutex_signature);
			ReleaseMutex(_mutex_list);
			return infomdl_error_finish;
		}
		
		idx = _idx_signature;

		WaitForSingleObject(_mutex_tixml,INFINITE);
		if(_pTiXmlDoc == nullptr)
		{
			_pTiXmlDoc = new TiXmlDocument();
			if(!_pTiXmlDoc->LoadFile("common/signature.xml"))
			{
				delete _pTiXmlDoc;
				_pTiXmlDoc = nullptr;
				ReleaseMutex(_mutex_tixml);
				ReleaseMutex(_mutex_signature);
				ReleaseMutex(_mutex_list);
				return infomdl_error_xmlerror;
			}
			TiXmlHandle hDoc(_pTiXmlDoc);
			_pTiXmlElem = hDoc.FirstChild("path").ToElement();
			if(_pTiXmlElem == nullptr)
			{
				ReleaseMutex(_mutex_tixml);
				ReleaseMutex(_mutex_signature);
				ReleaseMutex(_mutex_list);
				return infomdl_error_xmlerror;
			}
		}
		
		itemData = _infolist->GetItemData(idx);
		if(itemData == signature_state_wait)
		{
			_infolist->SetItemData(idx, signature_state_check);
			_infolist->SetItemText(idx, 4, "正在识别");
		}

		if(_pTiXmlElem == nullptr)
		{
			if(itemData != signature_state_success)
			{
				_infolist->SetItemData(idx, signature_state_fail);
				_infolist->SetItemText(idx, 4, "未能识别");
			}
			TiXmlHandle hDoc(_pTiXmlDoc);
			_pTiXmlElem = hDoc.FirstChild("path").ToElement();
			if(_pTiXmlElem == nullptr)
			{
				ReleaseMutex(_mutex_tixml);
				ReleaseMutex(_mutex_signature);
				ReleaseMutex(_mutex_list);
				return infomdl_error_xmlerror;
			}
			_idx_signature++;
		}

		pTiXmlElem = _pTiXmlElem;

		url = pTiXmlElem->Attribute("path");
		pTiXmlElem->QueryIntAttribute("type", &type);
		
		_pTiXmlElem = _pTiXmlElem->NextSiblingElement();
		ReleaseMutex(_mutex_tixml);
		ReleaseMutex(_mutex_signature);

		domain = _infolist->GetItemText(idx, 0).GetBuffer();

		ReleaseMutex(_mutex_list);

		// 通过这种方式解决识别成功后继续识别的问题
		if(itemData == signature_state_success || itemData == signature_state_refuse5)
			continue;

		url = domain + url;

		CToolMdl tool;
		string content, hash;
		int retcode;
		if(type & 0x01)
		{
			retcode=tool.signature(url, hash);
			if(retcode == -1)
			{
				WaitForSingleObject(_mutex_list, INFINITE);
				_infolist->SetItemData(idx, ++itemData);
				if(itemData == signature_state_refuse5)
				{
					_infolist->SetItemText(idx, 4, "主机连接失败");
				}
				ReleaseMutex(_mutex_list);
				continue;
			}
			if(retcode !=200)
				continue;
		}
		if(type & 0x02)
		{
			retcode=tool.request_get(url, content);
			if(retcode == -1)
			{
				WaitForSingleObject(_mutex_list, INFINITE);
				_infolist->SetItemData(idx, ++itemData);
				if(itemData == signature_state_refuse5)
				{
					_infolist->SetItemText(idx, 4, "主机连接失败");
				}
				ReleaseMutex(_mutex_list);
				continue;
			}
			if(retcode !=200)
				continue;
		}
		
		
		string md5, keyword, start, stop, cmsname;
		int pos;
		bool success = false;

		WaitForSingleObject(_mutex_tixml,INFINITE);
		// 以下代码由于互斥变量次序问题会发生死锁，所以注释掉吧
		//if(retcode == -1)	// 主机连接不上，本来想放到上面请求后的，由于互斥变量次序问题放在这里吧
		//{
		//	WaitForSingleObject(_mutex_signature,INFINITE);
		//	if(idx == _idx_signature)
		//	{
		//		_pTiXmlElem = nullptr;
		//		ReleaseMutex(_mutex_signature);
		//		ReleaseMutex(_mutex_tixml);
		//		continue;
		//	}
		//	ReleaseMutex(_mutex_signature);
		//}
		pSubTiXmlElem = pTiXmlElem->FirstChild("sign")->ToElement();
		while(pSubTiXmlElem != nullptr)
		{
			int subtype = 0;
			if (pSubTiXmlElem->QueryIntAttribute("type", &subtype) == TIXML_SUCCESS) 
			{
				switch(subtype)
				{
				case 1:
					md5 = pSubTiXmlElem->Attribute("md5");
					if(md5 == hash)
					{
						success = true;
						cmsname = pSubTiXmlElem->GetText();
					}
					break;
				case 2:
					keyword = pSubTiXmlElem->Attribute("keyword");
					if(content.find(keyword)!=-1)
					{
						success = true;
						cmsname = pSubTiXmlElem->GetText();
					}
					break;
				case 3:
					start = pSubTiXmlElem->Attribute("start");
					stop = pSubTiXmlElem->Attribute("stop");
					pos = 0;
					if(toolmdl_error_success == tool.sub_string(content, start, stop, cmsname, pos))
					{
						cmsname = "模糊识别："+ cmsname;
						success = true;
					}
					break;
				default:
					break;
				}
			}
			// 以下代码由于互斥变量次序问题会发生死锁，所以注释掉吧，前面已经换了另一种方式避免重复识别
			//if(success)
			//{
			//	WaitForSingleObject(_mutex_signature,INFINITE);
			//	if(idx == _idx_signature)
			//	{
			//		_pTiXmlElem = nullptr;			// 指定为nullptr即可进入下一主机，不必再进行本次任务了  //多个线程识别都识别成功可能会造成问题
			//	}
			//	ReleaseMutex(_mutex_signature);
			//	break;
			//}
			pSubTiXmlElem = pSubTiXmlElem->NextSiblingElement("sign");
		}
		ReleaseMutex(_mutex_tixml);

		//if( idx == count - 1 && (success  || pSubTiXmlElem == nullptr))
		//{
		//	//这里也不对，因为有可能有的线程运行比较慢，不应该根据是否是最后一条记录
		//  //而应根据是否是最后一条读取操作，或者是最后一个运行中的线程
		//	delete _pTiXmlDoc;
		//	_pTiXmlDoc = nullptr;
		//}
		
		if(success)
		{
			WaitForSingleObject(_mutex_list,INFINITE);
			itemData = _infolist->GetItemData(idx);
			if(itemData != signature_state_success)
			{
				_infolist->SetItemData(idx, signature_state_success);
				_infolist->SetItemText(idx, 4, cmsname.c_str());
			}
			ReleaseMutex(_mutex_list);
		}
	}
	return infomdl_error_success;
}

CString CInfoCollMdl::GetItemText(int nIndex, int nColumn)
{
	CString itemText;
	WaitForSingleObject(_mutex_list,INFINITE);
	itemText = _infolist->GetItemText(nIndex, nColumn);
	ReleaseMutex(_mutex_list);
	return itemText;
}

infomdl_error_code CInfoCollMdl::InsertItem(string& domain, string& ip_address, string& title)
{
	WaitForSingleObject(_mutex_list,INFINITE);
	int i=0;
	int count=_infolist->GetItemCount();
	while(i<count)
	{
		if(_infolist->GetItemText(i,0)==domain.c_str())
		{
			ReleaseMutex(_mutex_list);
			return infomdl_error_exists;
		}
		i++;
	}
	if(i==count)
	{
		_infolist->InsertItem(i, domain.c_str());
		_infolist->SetItemText(i, 1, ip_address.c_str());
		_infolist->SetItemText(i, 2, title.c_str());
	}
	ReleaseMutex(_mutex_list);
	return infomdl_error_success;
}