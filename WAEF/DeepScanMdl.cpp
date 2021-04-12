#include "stdafx.h"
#include "DeepScanMdl.h"

#include "plugin.h"

CEdit* CDeepScanMdl::_interactiveBox;

CDeepScanMdl::CDeepScanMdl(void)
{
}


CDeepScanMdl::~CDeepScanMdl(void)
{
}

void CDeepScanMdl::InitModule(CTreeCtrl* moduleTree, CEdit* interactiveBox)
{
	_hModule= NULL;
	_moduleTree = moduleTree;
	_interactiveBox = interactiveBox;
	_exploit.InitExploitModule(interactiveBox);
	LoadModuleDir();
};

void CDeepScanMdl::LoadModuleDir()
{
	////::GetCurrentDirectory(512,buf);				//不用该函数，不一定是程序目录
	//GetModuleFileName(NULL,buf,512);
	//PathRemoveFileSpec(buf);
	//SetCurrentDirectory(buf);

	
	UINT nMask = TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT;
	CFileFind fileFind;
	CString moduleCatgory, moduleName;
	BOOL find=fileFind.FindFile("module\\*.*");
	while(find)
	{
		find=fileFind.FindNextFile();
		if(fileFind.IsDirectory() || fileFind.IsDots())
			continue;
		moduleName=fileFind.GetFileName();
		int pos = moduleName.Find(".waef");
		if(pos!=-1)
		{
			if(_exploit.LoadModule("module\\" + moduleName))
			{
				moduleCatgory = _exploit.GetModuleCatgory();
				_moduleTree->InsertItem(nMask, moduleName, 2, 2, 0, 0, 0, MakeCatgory(moduleCatgory), TVI_LAST );
			}
		}
	}
}

HTREEITEM CDeepScanMdl::MakeCatgory(CString ModuleCatgory)
{
	CString Catgory;
	UINT nMask = TVIF_HANDLE|TVIF_IMAGE|TVIF_PARAM|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT;
	HTREEITEM hParent, hItem;
	hItem = hParent = _moduleTree->GetRootItem();
	int pos1 = ModuleCatgory.Find('/');
	while(pos1>0)
	{
		Catgory = ModuleCatgory.Left(pos1);
		while(hItem!=nullptr)
		{
			if(Catgory == _moduleTree->GetItemText(hItem))
			{
				break;
			}
			hItem = _moduleTree->GetNextSiblingItem(hItem);
		}
		if(hItem==nullptr)
			hItem=_moduleTree->InsertItem(nMask, Catgory, 1, 1, 0, 0, 0, hParent, TVI_LAST );
		hParent = hItem;
		hItem = _moduleTree->GetChildItem(hItem);
		ModuleCatgory.Delete(0, pos1+1);
		pos1 = ModuleCatgory.Find('/');
	}
	return  _moduleTree->InsertItem(nMask, ModuleCatgory, 1, 1, 0, 0, 0, hParent, TVI_LAST );
}


void CDeepScanMdl::LoadWAEFModule(CString ModuleName)
{
	if(_exploit.LoadModule("module\\" + ModuleName))
	{
		LoadModuleDesc();
	}
}


void CDeepScanMdl::LoadModuleDesc()
{
	_interactiveBox->SetWindowText(_exploit.GetModuleDesc());
}

void CDeepScanMdl::LoadModuleConfig()
{
	_interactiveBox->SetWindowText(_exploit.GetModuleConfig());
}

void CDeepScanMdl::ExecuteModule()
{
	CString moduleConfig;
	_interactiveBox->GetWindowText(moduleConfig);
	_exploit.SetConfig(moduleConfig.GetBuffer());
	_interactiveBox->SetWindowText("[-] 等待返回结果...\r\n");
	_exploit.StartExploit();
}