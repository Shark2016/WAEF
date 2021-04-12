#include "stdafx.h"
#include "NetSpider.h"

#include <curl/curl.h>


CWnd*				NetSpider::_owner;
int					NetSpider::_deepth;
spider_mode			NetSpider::_mode;
//bool				NetSpider::_stop;
string				NetSpider::_baseurl;
string				NetSpider::_basedir;
string				NetSpider::_basehost;
string				NetSpider::_cookie;
spider_state		NetSpider::_state;
NOTIFYPROC			NetSpider::_pNotifyProc;

vector<string>		NetSpider::_querylist;
int					NetSpider::_querycount;
int					NetSpider::_queryidx;

HANDLE				NetSpider::_mutex;

NetSpider::NetSpider(void)
{
	curl_global_init(CURL_GLOBAL_ALL);
	_mutex=CreateMutex(NULL,FALSE,NULL);
}

NetSpider::~NetSpider(void)
{
	if(is_running())
	{
		stop();
	}
	curl_global_cleanup();
}

spider_error NetSpider::init(bool forcestop)
{
	_state = state_idle;
	WaitForSingleObject(_mutex, INFINITE);
	_querycount = 0;
	_queryidx = 0;
	_querylist.clear();
	ReleaseMutex(_mutex);
	return spider_error_success;
	/*
	if(forcestop)
	{
		if(is_running())
		{
			stop();
			return spider_stop_running;
		}
		else
		{
			_state=state_idle;
			return spider_error_success;
		}
	}
	else
	{
		if(is_running())
		{
			return spider_error_running;
		}
		else
		{
			_state=state_idle;
			return spider_error_success;
		}
	}
	*/
}

spider_error NetSpider::start()
{
	if(is_running())
	{
		stop();
	}
	_state = state_run;
	WaitForSingleObject(_mutex, INFINITE);
	getdir();
	gethost(); 
	_querycount = 0;
	_queryidx = 0;
	_querylist.clear();
	_pNotifyProc(_baseurl);
	_querylist.push_back(_baseurl);
	_querycount++;
	ReleaseMutex(_mutex);
	_thread[0] = CreateThread(NULL,0,spider_proc,NULL,0,NULL);
	return spider_error_success;
}

spider_error NetSpider::stop()
{
	//_stop = true;
	//WaitForSingleObject(_mutex, INFINITE);		加了这行有时会锁死
	TerminateThread(_thread[0],0);
	//ReleaseMutex(_mutex);
	WaitForSingleObject(_thread[0],INFINITE);
	CloseHandle(_thread[0]);
	_state = state_idle;
	return spider_error_success;
}

DWORD WINAPI NetSpider::spider_proc(LPVOID lpParam)
{
	spider_querylist();
	MessageBox(_owner->GetSafeHwnd(), "爬行完毕","提示",0);
	_state = state_idle;
	return 0;
}

spider_error NetSpider::spider_querylist()
{
	string url;
	string buffer;
	while(1)
	{
		WaitForSingleObject(_mutex, INFINITE);
		if(_queryidx >=_querycount)
		{
			ReleaseMutex(_mutex);
			/*
			if(还有线程没有退出)  //不对，这样永远都不会退出了
				continue;
			*/
			return spider_error_finish;
		}
		url = _querylist.at(_queryidx++);
		ReleaseMutex(_mutex);
		request_get(url, buffer);
		parse_page(buffer, url, 0);
	}
	return spider_error_success;
}

size_t NetSpider::write_callback( void *ptr, size_t size, size_t nmemb, string &userp)
{
	long sizes = size * nmemb;
    string temp((char*)ptr, sizes);
    userp += temp; 
    return sizes;
}

long NetSpider::request_get(const string& url, string& content)
{
	CURLcode res;
	CURL *curl = nullptr;
	curl = curl_easy_init();
	long retcode = 0;
	if(curl)
	{
		content.clear();
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); 
		res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
		if(!_cookie.empty())
		{
			//res = curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookie.txt"); //这种方式失败，可能是cookie写入格式没弄清
			res = curl_easy_setopt(curl, CURLOPT_COOKIE, _cookie.c_str());
		}
		//curl_easy_setopt(curl, CURLOPT_PROXY, "localhost:8080"); 
		//curl_easy_setopt(curl, CURLOPT_PROXY, "localhost"); 
		//curl_easy_setopt(curl, CURLOPT_PROXYPORT, "8080");
		//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		res = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode);
		curl_easy_cleanup(curl);
	}
	return retcode;
}

spider_error NetSpider::parse_page(string content, const string& orig_url, int deepth)
{
//	if(_stop)
//		return error_spider_stop;
	int rtn=0;
	int pos1 = 0, pos2 = 0;
	while(true)
	{
		pos1 = content.find('<', pos2);
		if(pos1==-1)
		{
			break;
		}
		pos2 = content.find('>', pos1);
		if(pos2==-1)
		{
			break;
		}
		string str_element = content.substr(pos1,pos2-pos1+1);
		if(str_element=="<script>")			// 跳过内嵌的<script></script>，不再分析里面的内容
		{
			pos1=content.find("</script>", pos2);
			if(pos1==-1)
				break;
			else
			{
				pos2 = pos1 + 5;
				continue;
			}
		}
		if(str_element.find("赵又廷")!=-1)
			int mmm=0;
		if(str_element.at(1)!='/' && str_element.at(1)!='!')	// 排除 </  <!--
		{
			string tag, value;
			if(error_success==parse_element(str_element, tag, value))
			{
				if(tag=="meta")
				{
					if(value.find("charset=utf-8"))		//<meta content="text/html; charset=gbk" />  <meta charset="utf-8" />
					{
						content=utf8togb(content);
					}
				}
				else
				{
					string url;
					if(error_success==process_url(orig_url, value, url))
					{
						//最终再处理一下一些异常的url，主要是<script></script>中夹杂的一些标签
						//先临时这么处理，好的处理方案是<script></script>之间的内容直接跳过
						//更完美的方式就是代理工具了吧，直接截取请求相应
						//--------------
						/*
						if((pos1=url.find('\''))>0)
						{
							url=url.substr(0, pos1);
						}
						if((pos1=url.find('"'))>0)
						{
							url=url.substr(0, pos1);
						}
						*/
						//if(url!="http://")	//还是由于<script></script>中夹杂的一些标签，临时这么处理一下吧
						//{
						//	if(spider_error_success == addtolist(url))
						//		_pNotifyProc(url);
						//}
						if(deepth<_deepth)
						{
							int nodetype;
							//string page_content;
							WaitForSingleObject(_mutex, INFINITE);
							switch (_mode)
							{
							case mode_frombase:
								if(url.find(_basedir)==0)
								{
									nodetype = _pNotifyProc(url);
									switch(nodetype & 0x7F)
									{
									case url_type_pic:
									case url_type_js:
									case url_type_css:
									case url_type_file:
									case url_type_anchor:
										break;
									case url_type_host:
									case url_type_dir:
									case url_type_php:
									case url_type_asp:
									case url_type_aspx:
									case url_type_jsp:
									case url_type_html:
									case url_type_other:
									case url_type_unknow:
									case url_type_param:
										if(!(nodetype & 0x80))	//没有爬过就爬!
										{
											//request_get(url, page_content);
											//parse_page(page_content, url, deepth+1);
											_querylist.push_back(url);
											_querycount++;
										}
										break;
									}
									
								}
								break;
							case mode_fromhost:
								if(url.find(_basehost)==0)
								{
									nodetype = _pNotifyProc(url);
									switch(nodetype & 0x7F)
									{
									case url_type_pic:
									case url_type_js:
									case url_type_css:
									case url_type_file:
									case url_type_anchor:
										break;
									case url_type_host:
									case url_type_dir:
									case url_type_php:
									case url_type_asp:
									case url_type_aspx:
									case url_type_jsp:
									case url_type_html:
									case url_type_other:
									case url_type_unknow:
									case url_type_param:
										if(!(nodetype & 0x80))	//没有爬过就爬!
										{
											WaitForSingleObject(_mutex, INFINITE);
											//request_get(url, page_content);
											//parse_page(page_content, url, deepth+1);
											_querylist.push_back(url);
											_querycount++;
											ReleaseMutex(_mutex);
										}
										break;
									}
									
								}
								break;
							case mode_listall:
								nodetype = _pNotifyProc(url);
								if(url.find(_basehost)==0)
								{
									switch(nodetype & 0x7F)
									{
									case url_type_pic:
									case url_type_js:
									case url_type_css:
									case url_type_file:
									case url_type_anchor:
										break;
									case url_type_host:
									case url_type_dir:
									case url_type_php:
									case url_type_asp:
									case url_type_aspx:
									case url_type_jsp:
									case url_type_html:
									case url_type_other:
									case url_type_unknow:
									case url_type_param:
										if(!(nodetype & 0x80))	//没有爬过就爬!
										{
											WaitForSingleObject(_mutex, INFINITE);
											//request_get(url, page_content);
											//parse_page(page_content, url, deepth+1);
											_querylist.push_back(url);
											_querycount++;
											ReleaseMutex(_mutex);
										}
										break;
									}
									
								}
								break;
							case mode_fromall:
								nodetype = _pNotifyProc(url);
								switch(nodetype & 0x7F)
								{
								case url_type_pic:
								case url_type_js:
								case url_type_css:
								case url_type_file:
								case url_type_anchor:
									break;
								case url_type_host:
								case url_type_dir:
								case url_type_php:
								case url_type_asp:
								case url_type_aspx:
								case url_type_jsp:
								case url_type_html:
								case url_type_other:
								case url_type_unknow:
								case url_type_param:
									if(!(nodetype & 0x80))	//没有爬过就爬!
									{
										WaitForSingleObject(_mutex, INFINITE);
										//request_get(url, page_content);
										//parse_page(page_content, url, deepth+1);
										_querylist.push_back(url);
										_querycount++;
										ReleaseMutex(_mutex);
									}
									break;
								}
								break;
							default:
								break;
							}
							ReleaseMutex(_mutex);
						}
					}
				}
			}
		}
	}
	return spider_error_success;
}

spider_error NetSpider::process_url(const string& orig_url, string value, string& url)
{
	if(value.empty())
		return spider_error_emptyvalue;
	if(value.find("://")!=-1 || value.substr(0, 7) == "mailto:" ||  value.substr(0, 11) == "javascript:")
	{
		url=value;
	}
	else if(value.substr(0, 2)=="//")
	{
		url="http:";
		url+=value;
	}
	else
	{
		string baseurl;
		string hosturl;
		int right_pos = orig_url.find('?');
		right_pos = (right_pos==-1)? orig_url.find_last_of('/'):orig_url.find_last_of('/', right_pos);
		if((orig_url.substr(0, 7)=="http://" && right_pos==6) ||
			(orig_url.substr(0, 8)=="https://" && right_pos==7))		//  类似 http://www.baidu.com
		{
			baseurl = hosturl = orig_url;
		}
		else
		{
			baseurl = orig_url.substr(0, right_pos);
			hosturl = orig_url.substr(0, orig_url.find('/', orig_url.find("://")+3));
		}

		char x=value.at(0);
		switch(x)
		{
		case '#':
			if(value.length()==1)
				return spider_error_emptyvalue;
			url = orig_url+value;
			break;
		case '/':
			url = hosturl + value;
			break;
		case '.':
			while(x=='.')
			{
				if(value.substr(0,2)=="./")
				{
					value = value.substr(2);
				}
				else if(value.substr(0,3)=="../")
				{
					value = value.substr(3);
					baseurl = baseurl.substr(0, baseurl.find_last_of('/'));
				}
				if(value.empty())
					break;
				x=value.at(0);
			}
			url = baseurl + '/' + value;
			break;
		default:
			url = baseurl + '/' + value;
		}
	}
	return spider_error_success;
}

spider_error NetSpider::parse_element(string& str_element, string& tag, string& value)
{
	int pos=0;
	//CString str_element_lower = str_element.MakeLower();
	string str_element_lower = makelower(str_element);
	if((pos=str_element_lower.find(' '))==-1)
		return spider_error_tag_without_value;
	tag = str_element_lower.substr(1,pos-1);
	string value_name;
	if(tag=="a")
	{
		value_name="href";
	}
	else if(tag=="iframe")
	{
		value_name="src";
	}
	else if(tag=="link")
	{
		value_name="href";
	}
	else if(tag=="script")
	{
		value_name="src";
	}
	//else if(tag=="form")		// 爬虫就不再爬表单提交页了
	//{
	//	value_name="action";
	//}
	else if(tag=="meta")
	{
		value_name="content";
	}
	else
	{
		return spider_error_not_target_tag;
	}
	if((pos=str_element_lower.find(value_name),pos)==-1)
		return spider_error_tag_no_link;
	pos=str_element.find('=',pos);
	if(pos==-1)
		return spider_error_tag_no_link;
	while(str_element.at(pos)==' ')
	{
		pos++;
	}
	char x=str_element.at(pos+1);
	if(x=='"')
	{
		value=str_element.substr(pos+2,str_element.find('"',pos+2)-pos-2);
	}
	else if(x=='\'')
	{
		value=str_element.substr(pos+2,str_element.find('\'',pos+2)-pos-2);
	}
	else
	{
		int pos2 = str_element.find(' ',pos+1);
		pos2 = (pos2==-1)?str_element.find('>',pos+1):pos2;
		value=str_element.substr(pos+1,pos2-pos-1);
	}
	//value.Trim();
	if(!value.empty())
	{
		int first = value.find_first_not_of(" ");
		if(first == -1)
		{
			value = "";
		}
		else
		{
			int last = value.find_last_not_of(" ");
			value = value.substr(first, last - first + 1);
		}
	}
	return spider_error_success;
}

string NetSpider::makelower(string source)
{
	string rtn;
	int i=0;
	int len = source.length();
	char c;
	while(i<len)
	{
		c = source.at(i);
		if(c>=0x41 && c<=0x5A)
		{
			rtn += c+0x20;
		}
		else
		{
			rtn += c;
		}
		i++;
	}
	return rtn;
}

string NetSpider::utf8togb(string source)
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

void NetSpider::getdir()
{
	int pos = _baseurl.find_last_of('/');
	if(pos >7  && pos != -1)
	{
		_basedir=_baseurl.substr(0, pos+1);
	}
	else
	{
		_basedir=_baseurl + "/";
	}
}

void NetSpider::gethost()
{
	int pos = _baseurl.find('/', 7);
	if(pos != -1)
	{
		_basehost=_baseurl.substr(0, pos);
	}
	else
	{
		_basehost=_baseurl + "/";
	}
}

/*
spider_error NetSpider::addtolist(string url)
{
	int i=0;
	int size = _urllist.size();
	while(i< size)
	{
		if(_urllist.at(i) == url)
			return spider_error_urlexist;
		i++;
	}
	_urllist.push_back(url);
	return spider_error_success;
}
*/