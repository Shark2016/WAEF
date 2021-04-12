#include "stdafx.h"
#include "WeakPluginMdl.h"

#include "plugin.h"

//CListCtrl*		CWeakPluginMdl::_pluginList;
CString			CWeakPluginMdl::_target;
CListCtrl*		CWeakPluginMdl::_resultList;
CEdit*			CWeakPluginMdl::_interactiveBox;

CWeakPluginMdl::CWeakPluginMdl(void)
{
}


CWeakPluginMdl::~CWeakPluginMdl(void)
{
}

void CWeakPluginMdl::InitModule(CListCtrl* pluginList, CEdit* interactiveBox, CListCtrl* resultList)
{
	_hPlugin= NULL;
	_pluginList = pluginList;
	_interactiveBox = interactiveBox;
	_resultList = resultList;
	LoadPluginDir("plugin");
};

void CWeakPluginMdl::SetTarget(CString target)
{
	_target = target;
}


void CWeakPluginMdl::LoadPluginDir(CString PluginPath)
{
	////::GetCurrentDirectory(512,buf);				//不用该函数，不一定是程序目录
	//GetModuleFileName(NULL,buf,512);
	//PathRemoveFileSpec(buf);
	//SetCurrentDirectory(buf);

	
	CFileFind fileFind;
	CString pluginIndex, pluginName;
	int idx = _pluginList->GetItemCount();
	BOOL find=fileFind.FindFile(PluginPath +"\\*.*");
	while(find)
	{
		find=fileFind.FindNextFile();
		pluginName=fileFind.GetFileName();
		if(fileFind.IsDots() || fileFind.IsDirectory())
			continue;
		else
		{
			int pos = pluginName.Find(".waef");
			if(pos!=-1)
			{
				pluginIndex.Format("%d", idx+1);
				_pluginList->InsertItem(idx, pluginIndex);
				_pluginList->SetItemText(idx, 2, PluginPath + "\\" + pluginName);
				HINSTANCE hPlugin = LoadLibrary(PluginPath + "\\" + pluginName);
				if(hPlugin!=NULL)
				{
					if(GetPluginInfo(hPlugin, plugin_name, pluginName))
					{
						_pluginList->SetItemText(idx, 1, pluginName);
					}
					FreeLibrary(hPlugin);
				}
				idx++;
			}
		}
	}

}


void CWeakPluginMdl::LoadWAEFPlugin(int nIndex)
{
	CString ModulePath = _pluginList->GetItemText(nIndex, 2);
	if(_hPlugin != NULL)
	{
		FreeLibrary(_hPlugin);
	}
	_hPlugin = LoadLibrary(ModulePath);
	LoadPluginDesc();
}

void CWeakPluginMdl::LoadPluginDesc()
{
	if(_hPlugin == NULL)
	{
		_interactiveBox->SetWindowText("[!] 当前未加载任何插件");
		return;
	}
	CString pluginDesc;
	GetPluginInfo(_hPlugin, plugin_desc, pluginDesc);
	_interactiveBox->SetWindowText(pluginDesc);
}

void CWeakPluginMdl::LoadPluginConfig()
{
	if(_hPlugin == NULL)
	{
		_interactiveBox->SetWindowText("[!] 当前未加载任何插件");
		return;
	}
	CString pluginConfig;
	GetPluginInfo(_hPlugin, plugin_config, pluginConfig);
	pluginConfig.Replace("<%TARGET%>", _target);
	_interactiveBox->SetWindowText(pluginConfig);
}

void CWeakPluginMdl::ExecutePlugin()
{
	if(_hPlugin == NULL)
	{
		_interactiveBox->SetWindowText("[!] 当前未加载任何插件");
		return;
	}
	CString moduleConfig, moduleOutput;
	_interactiveBox->GetWindowText(moduleConfig);
		
	WAEF_PLUGIN_INIT* initid = new WAEF_PLUGIN_INIT;
	size_t size = moduleConfig.GetLength()+1;
	initid->outPtr = nullptr;
	initid->inPtr=(char *)malloc(size);
	initid->pNotify = NotifyProc;
	strcpy_s(initid->inPtr, size, moduleConfig.GetBuffer());

	LPFNDLLFUNC_WAEF_MODULE_FUNC lpfnWAEFPluginFunc;
	lpfnWAEFPluginFunc = (LPFNDLLFUNC_WAEF_MODULE_FUNC)GetProcAddress(_hPlugin, "WAEF_ExecutePlugin");
	if (!lpfnWAEFPluginFunc)
		return;
	if(ERROR_WAEFPLGUIN_SUCCESS == lpfnWAEFPluginFunc(initid))
	{
		_interactiveBox->SetWindowText("[-] 等待返回结果...");
	}
}

LRESULT CALLBACK CWeakPluginMdl::NotifyProc(LPVOID Param)
{
	WAEF_PLUGIN_INIT* initid=(WAEF_PLUGIN_INIT*)Param;
	
	CString moduleOutput;
	int nCnt;
	switch(initid->error_rtn)
	{
	case ERROR_WAEFPLGUIN_NOTVULNERABLE:
		moduleOutput.Format("[+] 404 - %s", initid->outPtr);
		_interactiveBox->SetWindowText(moduleOutput);
		break;
	case ERROR_WAEFPLGUIN_VULNERABLE:
		moduleOutput.Format("[+] %d - %s", initid->outcode, initid->outPtr);
		_interactiveBox->SetWindowText(moduleOutput);
		nCnt = _resultList->GetItemCount();
		moduleOutput.Format("%d", nCnt);
		_resultList->InsertItem(nCnt, moduleOutput);
		moduleOutput.Format("%d : %s", initid->outcode, initid->outPtr);
		_resultList->SetItemText(nCnt, 1, moduleOutput);
		break;
	case ERROR_WAEFPLGUIN_FINISH:
		_interactiveBox->SetWindowText("[+] 模块执行完毕");
		break;
	case ERROR_WAEFPLGUIN_CONFIG_ERROR:
		_interactiveBox->SetWindowText("[!] 配置参数错误, 请先获取参数格式再填充参数字串!");
		break;
	default:
		_interactiveBox->SetWindowText("[!] 未知错误!");
		break;
	}

	if(initid->outPtr)
	{
		free(initid->outPtr);
		initid->outPtr=nullptr;
	}

	if(initid->error_rtn !=ERROR_WAEFPLGUIN_NOTVULNERABLE && initid->error_rtn != ERROR_WAEFPLGUIN_VULNERABLE)
	{
		if(initid->inPtr)
			free(initid->inPtr);
		delete initid;
	}

	return 0;
}