#include "stdafx.h"
#include "WeakScanMdl.h"

char* state_description[] = {
	"等待检测...",
	"正在检测...",
	"检测完毕",
};

char* process_description[] = {
	"□□□□□□□□□□",
	"■□□□□□□□□□",
	"■■□□□□□□□□",
	"■■■□□□□□□□",
	"■■■■□□□□□□",
	"■■■■■□□□□□",
	"■■■■■■□□□□",
	"■■■■■■■□□□",
	"■■■■■■■■□□",
	"■■■■■■■■■□",
	"■■■■■■■■■■"
};

int				CWeakScanMdl::_thread_count;
int				CWeakScanMdl::_item_count;
int				CWeakScanMdl::_current_item;
HANDLE			CWeakScanMdl::_thread[MAX_SCAN_THREAD];
CListCtrl*		CWeakScanMdl::_scanlist;
CListBox*		CWeakScanMdl::_loglist;
HANDLE			CWeakScanMdl::_mutex;

CWeakScanMdl::CWeakScanMdl(void)
{
}


CWeakScanMdl::~CWeakScanMdl(void)
{
}

scanmdl_error CWeakScanMdl::init(CListCtrl* scanlist, CListBox* loglist)
{
	_scanlist = scanlist;
	_loglist = loglist;
	return scanmdl_error_success;
}

scanmdl_error CWeakScanMdl::start()
{
	_current_item = 0;
	_item_count = _scanlist->GetItemCount();
	int i;
	for(i=0;i<_thread_count;i++)
	{
		_thread[i] = CreateThread(NULL,0,scan_proc,NULL,0,NULL);
	}

	return scanmdl_error_success;
}

scanmdl_error CWeakScanMdl::stop()
{
	return scanmdl_error_success;
}

scanmdl_error CWeakScanMdl::init_schedule(pstruct_scan_item pscan_item)
{
	pstruct_schedule_item pfirst=nullptr, pcurrent = nullptr;
	pfirst = pcurrent = new struct_schedule_item(policy_response_analysis);
	pscan_item->schedule_count+=1;

	if(pscan_item->param)
	{
		pcurrent->pNext = new struct_schedule_item(policy_cross_site_script);
		pcurrent = pcurrent->pNext;
		pcurrent->pNext = new struct_schedule_item(policy_sql_injection);
		pcurrent = pcurrent->pNext;
		pscan_item->schedule_count+=2;
		switch(pscan_item->type)
		{
		case url_type_php:
			pcurrent->pNext = new struct_schedule_item(policy_file_include);
			pcurrent = pcurrent->pNext;
			pcurrent->pNext = new struct_schedule_item(policy_code_injection);
			pcurrent = pcurrent->pNext;
			pcurrent->pNext = new struct_schedule_item(policy_command_injection);
			pcurrent = pcurrent->pNext;
			pscan_item->schedule_count+=3;
			break;
		case url_type_asp:
		case url_type_aspx:
		case url_type_jsp:
			break;
		}
	}
	if(pscan_item->method == target_method_post)
	{/*
		pcurrent->pNext = new struct_schedule_item(policy_cross_site_script);
		pcurrent = pcurrent->pNext;*/
		pcurrent->pNext = new struct_schedule_item(policy_sql_injection);
		pcurrent = pcurrent->pNext;
		pscan_item->schedule_count+=1;/*
		switch(pscan_item->type)
		{
		case url_type_php:
			pcurrent->pNext = new struct_schedule_item(policy_file_include);
			pcurrent = pcurrent->pNext;
			pcurrent->pNext = new struct_schedule_item(policy_code_injection);
			pcurrent = pcurrent->pNext;
			pcurrent->pNext = new struct_schedule_item(policy_command_injection);
			pcurrent = pcurrent->pNext;
			pscan_item->schedule_count+=3;
			break;
		case url_type_asp:
		case url_type_aspx:
		case url_type_jsp:
			break;
		}*/
	}

	pscan_item->pcurrent_item = pscan_item->pschedule_item = pfirst;
	return scanmdl_error_success;
}

scanmdl_error CWeakScanMdl::update_item_state(pstruct_scan_item pscan_item)
{
	int idx = pscan_item->idx;
	char weakness_count[10];
	sprintf_s(weakness_count, "%d", pscan_item->weakness_count);
	int process = pscan_item->schedule_count ? (int)(pscan_item->process / pscan_item->schedule_count +0.5 ) * 10 : 10;
	WaitForSingleObject(_mutex,INFINITE);
	_scanlist->SetItemText(idx, 1, process_description[process]);
	_scanlist->SetItemText(idx, 2, state_description[pscan_item->state]);
	_scanlist->SetItemText(idx, 3, weakness_count);
	ReleaseMutex(_mutex);
	return scanmdl_error_success;
}

DWORD WINAPI CWeakScanMdl::scan_proc(LPVOID lpParam)
{
	int current_item;
	pstruct_scan_item pscan_item = nullptr;
	while(true)
	{
		WaitForSingleObject(_mutex,INFINITE);
		if(_current_item >= _item_count)
		{
			ReleaseMutex(_mutex);
			break;
		}
		current_item = _current_item++;
		pscan_item = (pstruct_scan_item)_scanlist->GetItemData(current_item);
		ReleaseMutex(_mutex);
		if(pscan_item->state == item_scan_state_finish)
			continue;
		pscan_item->state = item_scan_state_checking;
		update_item_state(pscan_item);
		while(pscan_item->pcurrent_item!=nullptr)
		{
			scanmdl_error result = scanmdl_error_success;
			switch(pscan_item->pcurrent_item->type)
			{
			case policy_response_analysis:
				result = response_analysis(pscan_item);
				break;
			case policy_sql_injection:
				result = check_sqlinjection(pscan_item);
				break;
			case policy_cross_site_script:
				result = check_crosssitescript(pscan_item);
				break;
			case policy_file_include:
				result = check_fileinclude(pscan_item);
				break;
			case policy_code_injection:
				result = check_codeinjection(pscan_item);
				break;
			case policy_command_injection:
				result = check_commandinjection(pscan_item);
				break;
			}
			if(scanmdl_error_success != result)
			{
				update_item_state(pscan_item);
			}
			pscan_item->pcurrent_item = pscan_item->pcurrent_item->pNext;
			pscan_item->process+=1;
		}
		pscan_item->state = item_scan_state_finish;
		update_item_state(pscan_item);
	}
	return 0;
}

scanmdl_error CWeakScanMdl::response_analysis(pstruct_scan_item pscan_item)
{
	CToolMdl toolmdl;
	string content;
	toolmdl.request_get(pscan_item->url, content);
	scanmdl_error result = scanmdl_error_success;
	pstruct_weakness_item pfirst=nullptr, pcurrent = nullptr;

	// 常规分析
	int pos1;
	if((pos1 = content.find("mailto:"))!=-1)
	{
		int pos2 = content.find("\"", pos1+7);
		string email = content.substr(pos1+7, pos2-pos1-7);
		if(pcurrent==nullptr)
		{
			pfirst = pcurrent = new struct_weakness_item(weak_sensitiveinfo_email, weakness_level_inform);
		}
		else
		{
			pcurrent-> pNext = new struct_weakness_item(weak_sensitiveinfo_email, weakness_level_inform);
			pcurrent = pcurrent->pNext;
		}
		pcurrent->payload = pscan_item->url;
		pcurrent->description = "发现敏感信息—email地址 " + email + ", 当心被恶意利用。";
		pcurrent->advice = "注意隐私保护";
		
		pscan_item->weakness_count++;
		result = scanmdl_error_weakfind;
	}

	if((content.find("<form"))!=-1)
	{
		CString log;
		log.Format("%s 发现form表单，需要进一步抓取POST数据进行检测", pscan_item->url.c_str());
		addLog(log);
	}


	// 特殊类型特殊分析
	switch(pscan_item->type)
	{
	case url_type_host:
	case url_type_dir:
		if(content.find("<title>Index of")!=-1)
		{	
			if(pcurrent==nullptr)
			{
				pfirst = pcurrent = new struct_weakness_item(weak_directorylist, weakness_level_low);
			}
			else
			{
				pcurrent-> pNext = new struct_weakness_item(weak_directorylist, weakness_level_low);
				pcurrent = pcurrent->pNext;
			}
			pcurrent->payload = pscan_item->url;
			pcurrent->description = "路径"+ pscan_item->url + "存在列目录问题，可能泄露服务器敏感信息。";
			pcurrent->advice = "修改apache配置文件httpd.conf，将Options Indexes FollowSymLinks修改成Options FollowSymLinks并重启apache服务";
			pscan_item->weakness_count++;
			result = scanmdl_error_weakfind;
		}
		break;
	case url_type_html:
		break;
	case url_type_asp:
		break;
	case url_type_aspx:
		if(content.find("<script src=\"/WebResource.axd?d=")!=-1 || content.find("<script src=\"/ScriptResource.axd?d=")!=-1)
		{
			if(pcurrent==nullptr)
			{
				pfirst = pcurrent = new struct_weakness_item(weak_pandingoracle, weakness_level_high);
			}
			else
			{
				pcurrent-> pNext = new struct_weakness_item(weak_pandingoracle, weakness_level_high);
				pcurrent = pcurrent->pNext;
			}
			pcurrent->payload = pscan_item->url;
			pcurrent->description = "服务器ASP.NET框架存在padding oracle漏洞(MS10-070)";
			pcurrent->advice = "在web.config中创建&lt;customErrors&gt;配置项如下，将成为页面重定向到指定页面:<br />"
				"<pre>&lt;configuration&gt;<br />"
				"   &lt;system.web&gt;<br />"
				"     &lt;customErrors mode=\"On\" redirectMode=\"ResponseRewrite\" defaultRedirect=\"~/error.aspx\" /&gt;<br />"
				"   &lt;/system.web&gt;<br />"
				"&lt;/configuration&gt;<br /></pre>";;
			pscan_item->weakness_count++;
			result = scanmdl_error_weakfind;
		}
		break;
	case url_type_jsp:
		break;
	case url_type_php:
		if(content.find("<b>Notice</b>:")!=-1 || content.find("<b>Warning</b>:")!=-1 || content.find("<b>Error</b>:")!=-1)
		{
			if(pcurrent==nullptr)
			{
				pfirst = pcurrent = new struct_weakness_item(weak_phperrorreport, weakness_level_medium);
			}
			else
			{
				pcurrent-> pNext = new struct_weakness_item(weak_phperrorreport, weakness_level_medium);
				pcurrent = pcurrent->pNext;
			}
			pcurrent->payload = pscan_item->url;
			pcurrent->description = "发现php报错信息，可能泄露物理路径或用来报错注入。";
			pcurrent->advice = "修改php.ini, 将display_errors = On改成display_errors = Off并重启apache服务";
			pscan_item->weakness_count++;
			result = scanmdl_error_weakfind;
		}
		if(content.find("<title>phpinfo()</title>")!=-1)
		{
			if(pcurrent==nullptr)
			{
				pfirst = pcurrent = new struct_weakness_item(weak_sensitiveinfo_phpinfo, weakness_level_medium);
			}
			else
			{
				pcurrent-> pNext = new struct_weakness_item(weak_sensitiveinfo_phpinfo, weakness_level_medium);
				pcurrent = pcurrent->pNext;
			}
			pcurrent->payload = pscan_item->url;
			pcurrent->description = "发现敏感信息—phpinfo()，可泄露服务器环境配置等重要信息。";
			pcurrent->advice = "删除相关页面或phpinfo()调用代码";
			pscan_item->weakness_count++;
			result = scanmdl_error_weakfind;
		}
		break;
	case url_type_other:
		break;
	}

	pcurrent = pscan_item->pweakness_item;
	if(pcurrent==nullptr)
	{
		pscan_item->pweakness_item = pfirst;
	}
	else
	{
		while(pcurrent->pNext!=nullptr)
		{
			pcurrent = pcurrent->pNext;
		}
		pcurrent->pNext = pfirst;
	}
	return result;
}

scanmdl_error CWeakScanMdl::check_crosssitescript(pstruct_scan_item pscan_item)
{
	scanmdl_error result = scanmdl_error_success;
	pstruct_weakness_item pfirst=nullptr, pcurrent = nullptr;

	CToolMdl toolmdl;

	string target = pscan_item->url;
	string payload;
	
	string content;
	string url, preurl, param, value, posturl;
	int istart, ipos1, ipos2;
	ipos1 = istart = pscan_item->url.find('?');
	if(istart == -1)
	{
		return scanmdl_error_success;
	}
	preurl = target.substr(0, istart+1);
	while(true)
	{
		ipos2 = target.find('=', ipos1+1);
		if(ipos2 == -1)
		{
			return scanmdl_error_success;
		}
		param = target.substr(ipos1+1, ipos2-ipos1-1);
		ipos1 = target.find('&', ipos2);
		if(ipos1 == -1)
		{
			value = target.substr(ipos2+1);
			posturl = "";
		}
		else
		{
			value = target.substr(ipos2+1, ipos1-ipos2-1);
			posturl = target.substr(ipos1);
		}

		payload = "7d543e<script>alert(1)</script>7d543e";
		url = urlencode(preurl + param + '=' + value + payload + posturl);
		toolmdl.request_get(url, content);

		if(content.find(payload)!=-1)
		{
			if(pcurrent==nullptr)
			{
				pfirst = pcurrent = new struct_weakness_item(weak_crosssitescript, weakness_level_high);
			}
			else
			{
				pcurrent-> pNext = new struct_weakness_item(weak_crosssitescript, weakness_level_high);
				pcurrent = pcurrent->pNext;
			}
			pcurrent->payload = url;
			pcurrent->description = "参数 " + param +" 存在反射型XSS漏洞。";
			pcurrent->advice = "注意用户输入参数的处理和编码";
			pscan_item->weakness_count++;
			result = scanmdl_error_weakfind;

		}
		if(ipos1 == -1)
			break;
	}

	pcurrent = pscan_item->pweakness_item;
	if(pcurrent==nullptr)
	{
		pscan_item->pweakness_item = pfirst;
	}
	else
	{
		while(pcurrent->pNext!=nullptr)
		{
			pcurrent = pcurrent->pNext;
		}
		pcurrent->pNext = pfirst;
	}

	return result;
}

scanmdl_error CWeakScanMdl::check_sqlinjection(pstruct_scan_item pscan_item)
{
	scanmdl_error result = scanmdl_error_success;
	pstruct_weakness_item pfirst=nullptr, pcurrent = nullptr;
	
	CToolMdl toolmdl;
	string target = pscan_item->url;

	if(pscan_item->method == target_method_get)
	{
		string url, preurl, param, name, value, posturl;
		string payload1, payload2;
		string content;
		size_t content_length1, content_length2;
		int ipos1, ipos2, ipos3;
		ipos1 = target.find('?');
		if(ipos1 == -1)
		{
			return scanmdl_error_success;
		}	
		do
		{
			ipos2 = target.find('&', ipos1+1);
			if(ipos2==-1)
			{
				preurl = target.substr(0, ipos1+1);
				param = target.substr(ipos1+1);
				posturl = "";
			}
			else
			{
				preurl = target.substr(0, ipos1+1);
				param = target.substr(ipos1+1, ipos2-ipos1-1);
				posturl = target.substr(ipos2);
			}
			ipos3 = param.find('=');
			if(ipos3 > 0 )
			{
				name = param.substr(0, ipos3);
				value = param.substr(ipos3+1);


				payload1 = "'";			// 首先测试下单引号报错

				url = urlencode(preurl + name + '=' + value + payload1 + posturl);
				toolmdl.request_get(url, content, pscan_item->cookie);
				if(content.find("Microsoft JET Database Engine")!=-1 || content.find("<b>Warning</b>:  mysql")!=-1) 
				{
					if(pcurrent==nullptr)
					{
						pfirst = pcurrent = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
					}
					else
					{
						pcurrent-> pNext = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
						pcurrent = pcurrent->pNext;
					}
					pcurrent->description = "参数 " + name +" 发现数据库报错，可能存在注入漏洞。";
					pcurrent->advice = "注意用户输入参数的处理和过滤";
					pcurrent->payload = url;
					pscan_item->weakness_count++;
					result = scanmdl_error_weakfind;

					// 发现漏洞就不继续测试了，检测下一参数吧
					ipos1=ipos2;
					continue;
				}

				if(isnumric(value))
				{
					payload1 = " and 1=1-- -";
					payload2 = " and 1=2-- -";

					url = urlencode(preurl + name + '=' + value + payload1 + posturl);
					toolmdl.request_get(url, content, pscan_item->cookie);
					content_length1 = content.length();
					url = urlencode(preurl + name + '=' + value + payload2 + posturl);
					toolmdl.request_get(url, content, pscan_item->cookie);
					content_length2 = content.length();
					if(content_length1 - content_length2 > 10) 
					{
						if(pcurrent==nullptr)
						{
							pfirst = pcurrent = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
						}
						else
						{
							pcurrent-> pNext = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
							pcurrent = pcurrent->pNext;
						}
						pcurrent->payload = url;
						pcurrent->description = "参数 " + name +" 存在数字型SQL注入漏洞。";
						pcurrent->advice = "注意用户输入参数的处理和过滤";
						pscan_item->weakness_count++;
						result = scanmdl_error_weakfind;

						// 发现漏洞就不继续测试了，检测下一参数吧
						ipos1=ipos2;
						continue;
					}
				}
				payload1 = "' and 1=1-- -";
				payload2 = "' and 1=2-- -";

				url = urlencode(preurl + name + '=' + value + payload1 + posturl);
				toolmdl.request_get(url, content, pscan_item->cookie);
				content_length1 = content.length();
				url = urlencode(preurl + name + '=' + value + payload2 + posturl);
				toolmdl.request_get(url, content, pscan_item->cookie);
				content_length2 = content.length();
				if(content_length1 - content_length2 > 10) 
				{
					if(pcurrent==nullptr)
					{
						pfirst = pcurrent = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
					}
					else
					{
						pcurrent-> pNext = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
						pcurrent = pcurrent->pNext;
					}
					pcurrent->description = "参数 " + name +" 存在字符型SQL注入漏洞。";
					pcurrent->advice = "注意用户输入参数的处理和过滤";
					pcurrent->payload = url;
					pscan_item->weakness_count++;
					result = scanmdl_error_weakfind;

					// 发现漏洞就不继续测试了，检测下一参数吧
					ipos1=ipos2;
					continue;
				}

			}
			ipos1=ipos2;
		}while(ipos2!=-1);
	}
	else if(pscan_item->method==target_method_post)
	{
		string origindata, data, predata, param, name, value, postdata;
		string payload1, payload2;
		string content;
		size_t content_length1, content_length2;
		int ipos1=0, ipos2, ipos3;
		origindata = pscan_item->postdata;
		do
		{
			ipos2 = origindata.find('&', ipos1);
			if(ipos2==-1)
			{
				predata = origindata.substr(0, ipos1);
				param = origindata.substr(ipos1);
				postdata = "";
			}
			else
			{
				predata = origindata.substr(0, ipos1);
				param = origindata.substr(ipos1, ipos2-ipos1);
				postdata = origindata.substr(ipos2);
			}
			if(param.substr(0,2)!="__")			// 这里可以对一些参数过滤, 去除不需要检测的参数，如ASPX的乱七八糟的__LASTFOCUS参数等等
			{
				ipos3 = param.find('=');
				if(ipos3 > 0 )
				{
					name = param.substr(0, ipos3);
					value = param.substr(ipos3+1);


					if(isnumric(value))
					{
						payload1 = " and 1=1-- -";
						payload2 = " and 1=2-- -";

						data = urlencode(predata + name + '=' + value + payload1 + postdata);
						toolmdl.request_post(target, data, content, pscan_item->cookie);
						content_length1 = content.length();
						data = urlencode(predata + name + '=' + value + payload2 + postdata);
						toolmdl.request_post(target, data, content, pscan_item->cookie);
						content_length2 = content.length();
						if(content_length1 - content_length2 > 10) 
						{
							if(pcurrent==nullptr)
							{
								pfirst = pcurrent = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
							}
							else
							{
								pcurrent-> pNext = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
								pcurrent = pcurrent->pNext;
							}
							pcurrent->payload = data;
							pcurrent->description = "POST参数 " + name +" 存在数字型SQL注入漏洞。";
							pcurrent->advice = "注意用户输入参数的处理和过滤";
							pscan_item->weakness_count++;
							result = scanmdl_error_weakfind;

							// 发现漏洞就不继续测试了，检测下一参数吧
							ipos1=ipos2;
							continue;
						}
					}
					payload1 = "' and 1=1-- -";
					payload2 = "' and 1=2-- -";

					data = urlencode(predata + name + '=' + value + payload1 + postdata);
					toolmdl.request_post(target, data, content, pscan_item->cookie);
					content_length1 = content.length();
					data = urlencode(predata + name + '=' + value + payload2 + postdata);
					toolmdl.request_post(target, data, content, pscan_item->cookie);
					content_length2 = content.length();
					if(content_length1 - content_length2 > 10) 
					{
						if(pcurrent==nullptr)
						{
							pfirst = pcurrent = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
						}
						else
						{
							pcurrent-> pNext = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
							pcurrent = pcurrent->pNext;
						}
						pcurrent->description = "POST参数 " + name +" 存在字符型SQL注入漏洞。";
						pcurrent->advice = "注意用户输入参数的处理和过滤";
						pcurrent->payload = data;
						pscan_item->weakness_count++;
						result = scanmdl_error_weakfind;

						// 发现漏洞就不继续测试了，检测下一参数吧
						ipos1=ipos2;
						continue;
					}

					payload1 = "%' and 1=1 and '%'='";
					payload2 = "%' and 1=2 and '%'='";

					data = urlencode(predata + name + '=' + value + payload1 + postdata);
					toolmdl.request_post(target, data, content, pscan_item->cookie);
					content_length1 = content.length();
					data = urlencode(predata + name + '=' + value + payload2 + postdata);
					toolmdl.request_post(target, data, content, pscan_item->cookie);
					content_length2 = content.length();
					if(content_length1 - content_length2 > 10) 
					{
						if(pcurrent==nullptr)
						{
							pfirst = pcurrent = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
						}
						else
						{
							pcurrent-> pNext = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
							pcurrent = pcurrent->pNext;
						}
						pcurrent->description = "POST参数 " + name +" 存在搜索型SQL注入漏洞。";
						pcurrent->advice = "注意用户输入参数的处理和过滤";
						pcurrent->payload = data;
						pscan_item->weakness_count++;
						result = scanmdl_error_weakfind;

						// 发现漏洞就不继续测试了，检测下一参数吧
						ipos1=ipos2;
						continue;
					}

					// 登录表单万能登录测试
					payload1 = "' or 1=1-- -";
					data = urlencode(predata + name + '=' + value + payload1 + postdata);
					toolmdl.request_post(target, data, content, pscan_item->cookie);
					content_length1 = content.length();
					toolmdl.request_post(target, pscan_item->postdata, content, pscan_item->cookie);
					content_length2 = content.length();
					if(content_length1 - content_length2 > 10) 
					{
						if(pcurrent==nullptr)
						{
							pfirst = pcurrent = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
						}
						else
						{
							pcurrent-> pNext = new struct_weakness_item(weak_sqlinjection, weakness_level_high);
							pcurrent = pcurrent->pNext;
						}
						pcurrent->description = "POST参数 " + name +" 存在SQL注入漏洞。";
						pcurrent->advice = "注意用户输入参数的处理和过滤";
						pcurrent->payload = data;
						pscan_item->weakness_count++;
						result = scanmdl_error_weakfind;

						// 发现漏洞就不继续测试了，检测下一参数吧
						ipos1=ipos2;
						continue;
					}
				}
			}
			ipos1=ipos2+1;
		}while(ipos2!=-1);
	}

	pcurrent = pscan_item->pweakness_item;
	if(pcurrent==nullptr)
	{
		pscan_item->pweakness_item = pfirst;
	}
	else
	{
		while(pcurrent->pNext!=nullptr)
		{
			pcurrent = pcurrent->pNext;
		}
		pcurrent->pNext = pfirst;
	}
	return result;
}

scanmdl_error CWeakScanMdl::check_fileinclude(pstruct_scan_item pscan_item)
{
	scanmdl_error result = scanmdl_error_success;
	pstruct_weakness_item pfirst=nullptr, pcurrent = nullptr;

	char* payloadlist[] = {
		"../../../etc/passwd%00",
		"../../../../etc/passwd%00"
	};

	char* signlist[] = {
		"root:x:",
		"root:x:"
	};


	CToolMdl toolmdl;

	string target = pscan_item->url;
	string payload;
	
	string content;
	string url, preurl, param, name, value, posturl;
	int ipos1, ipos2, ipos3;
	ipos1 = target.find('?');
	if(ipos1 == -1)
	{
		return scanmdl_error_success;
	}
	do
	{
		ipos2 = target.find('&', ipos1+1);
		if(ipos2==-1)
		{
			preurl = target.substr(0, ipos1+1);
			param = target.substr(ipos1+1);
			posturl = "";
		}
		else
		{
			preurl = target.substr(0, ipos1+1);
			param = target.substr(ipos1+1, ipos2-ipos1-1);
			posturl = target.substr(ipos2);
		}
		ipos3 = param.find('=');
		if(ipos3 > 0 )
		{
			name = param.substr(0, ipos3);
			value = param.substr(ipos3+1);

			int i;
			for(i=0;i<sizeof(payloadlist)/4;i++)
			{
				payload = payloadlist[i];
				url = urlencode(preurl + name + '=' + payload + posturl);
				toolmdl.request_get(url, content);

				if(content.find(signlist[i])!=-1)
				{
					if(pcurrent==nullptr)
					{
						pfirst = pcurrent = new struct_weakness_item(weak_fileinclude, weakness_level_high);
					}
					else
					{
						pcurrent-> pNext = new struct_weakness_item(weak_fileinclude, weakness_level_high);
						pcurrent = pcurrent->pNext;
					}
					pcurrent->payload = url;
					pcurrent->description = "参数 " + param +" 存在本地文件包含漏洞。";
					pcurrent->advice = "注意用户输入参数的处理和过滤";
					pscan_item->weakness_count++;
					result = scanmdl_error_weakfind;
					break;
				}
			}
		}
		ipos1=ipos2;
	}while(ipos2!=-1);
	
	pcurrent = pscan_item->pweakness_item;
	if(pcurrent==nullptr)
	{
		pscan_item->pweakness_item = pfirst;
	}
	else
	{
		while(pcurrent->pNext!=nullptr)
		{
			pcurrent = pcurrent->pNext;
		}
		pcurrent->pNext = pfirst;
	}

	return result;
}

scanmdl_error CWeakScanMdl::check_codeinjection(pstruct_scan_item pscan_item)
{
	scanmdl_error result = scanmdl_error_success;
	pstruct_weakness_item pfirst=nullptr, pcurrent = nullptr;

	char* payloadlist[] = {
		"{${phpinfo()}};"
	};

	char* signlist[] = {
		"<title>phpinfo()</title>"
	};
	CToolMdl toolmdl;

	string target = pscan_item->url;
	string payload;
	
	string content;
	string url, preurl, param, name, value, posturl;
	int ipos1, ipos2, ipos3;
	ipos1 = target.find('?');
	if(ipos1 == -1)
	{
		return scanmdl_error_success;
	}
	do
	{
		ipos2 = target.find('&', ipos1+1);
		if(ipos2==-1)
		{
			preurl = target.substr(0, ipos1+1);
			param = target.substr(ipos1+1);
			posturl = "";
		}
		else
		{
			preurl = target.substr(0, ipos1+1);
			param = target.substr(ipos1+1, ipos2-ipos1-1);
			posturl = target.substr(ipos2);
		}
		ipos3 = param.find('=');
		if(ipos3 > 0 )
		{
			name = param.substr(0, ipos3);
			value = param.substr(ipos3+1);

			int i;
			for(i=0;i<sizeof(payloadlist)/4;i++)
			{
				payload = payloadlist[i];
				url = urlencode(preurl + name + '=' + value + payload + posturl);
				toolmdl.request_get(url, content);

				if(content.find(signlist[i])!=-1)
				{
					if(pcurrent==nullptr)
					{
						pfirst = pcurrent = new struct_weakness_item(weak_codeinjection, weakness_level_high);
					}
					else
					{
						pcurrent-> pNext = new struct_weakness_item(weak_codeinjection, weakness_level_high);
						pcurrent = pcurrent->pNext;
					}
					pcurrent->payload = url;
					pcurrent->description = "参数 " + param +" 存在php代码注入漏洞。";
					pcurrent->advice = "注意用户输入参数的处理和过滤";
					pscan_item->weakness_count++;
					result = scanmdl_error_weakfind;
					break;
				}
			}
		}
		ipos1=ipos2;
	}while(ipos2!=-1);

	pcurrent = pscan_item->pweakness_item;
	if(pcurrent==nullptr)
	{
		pscan_item->pweakness_item = pfirst;
	}
	else
	{
		while(pcurrent->pNext!=nullptr)
		{
			pcurrent = pcurrent->pNext;
		}
		pcurrent->pNext = pfirst;
	}

	return result;
}

scanmdl_error CWeakScanMdl::check_commandinjection(pstruct_scan_item pscan_item)
{
	scanmdl_error result = scanmdl_error_success;
	pstruct_weakness_item pfirst=nullptr, pcurrent = nullptr;

	char* payloadlist[] = {
		"| cat /etc/passwd"
	};

	char* signlist[] = {
		"root:x:"
	};
	CToolMdl toolmdl;

	string target = pscan_item->url;
	string payload;
	
	string content;
	string url, preurl, param, name, value, posturl;
	int ipos1, ipos2, ipos3;
	ipos1 = target.find('?');
	if(ipos1 == -1)
	{
		return scanmdl_error_success;
	}
	do
	{
		ipos2 = target.find('&', ipos1+1);
		if(ipos2==-1)
		{
			preurl = target.substr(0, ipos1+1);
			param = target.substr(ipos1+1);
			posturl = "";
		}
		else
		{
			preurl = target.substr(0, ipos1+1);
			param = target.substr(ipos1+1, ipos2-ipos1-1);
			posturl = target.substr(ipos2);
		}
		ipos3 = param.find('=');
		if(ipos3 > 0 )
		{
			name = param.substr(0, ipos3);
			value = param.substr(ipos3+1);

			int i;
			for(i=0;i<sizeof(payloadlist)/4;i++)
			{
				payload = payloadlist[i];
				url = urlencode(preurl + name + '=' + value + payload + posturl);
				toolmdl.request_get(url, content);

				if(content.find(signlist[i])!=-1)
				{
					if(pcurrent==nullptr)
					{
						pfirst = pcurrent = new struct_weakness_item(weak_commandinjection, weakness_level_high);
					}
					else
					{
						pcurrent-> pNext = new struct_weakness_item(weak_commandinjection, weakness_level_high);
						pcurrent = pcurrent->pNext;
					}
					pcurrent->payload = url;
					pcurrent->description = "参数 " + param +" 存在命令注入漏洞。";
					pcurrent->advice = "注意用户输入参数的处理和过滤";
					pscan_item->weakness_count++;
					result = scanmdl_error_weakfind;
					break;
				}
			}
		}
		ipos1=ipos2;
	}while(ipos2!=-1);

	pcurrent = pscan_item->pweakness_item;
	if(pcurrent==nullptr)
	{
		pscan_item->pweakness_item = pfirst;
	}
	else
	{
		while(pcurrent->pNext!=nullptr)
		{
			pcurrent = pcurrent->pNext;
		}
		pcurrent->pNext = pfirst;
	}

	return result;
}


bool CWeakScanMdl::isnumric(string value)
{
	int i;
	int len = value.length();
	for(i=0;i<len;i++)
	{
		if(!isdigit(value.at(i)))
			return false;
	}
	return true;
}

string CWeakScanMdl::replace(string find, string replace, string source)
{
	int pos = source.find(find);
	int lenfind = find.length();
	while(pos!=-1)
	{
		source.replace(pos, lenfind, replace);
		pos = source.find(find, pos);
	}
	return source;
}

string CWeakScanMdl::urlencode(string source)
{
	int i;
	string dest;
	int len = source.length();
	char x;
	for(i=0;i<len;i++)
	{
		x = source.at(i);
		switch (x)
		{
		case ' ':
			dest+="%20";
			break;
		case '<':
			dest+="%3c";
			break;
		case '>':
			dest+="%3e";
			break;
		case '"':
			dest+="%22";
			break;
		case '\'':
			dest+="%27";
			break;
		default:
			dest+=x;
			break;
		}
	}
	return dest;
	
}

void CWeakScanMdl::addLog(CString log)
{
	CString outLog;
	SYSTEMTIME sys; 
	GetLocalTime( &sys );
	outLog.Format("%4d-%02d-%02d %02d:%02d:%02d %s", sys.wYear,sys.wMonth,sys.wDay,sys.wHour,sys.wMinute, sys.wSecond, log);
	_loglist->AddString(outLog);
}