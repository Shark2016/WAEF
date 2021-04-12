#pragma once

#include "ExploitModule.h"

#define SERVER_ALL						0x00000000
	
#define SERVER_IIS						0x00000100
#define SERVER_IIS_5_0					0x00000200
#define SERVER_IIS_6_0					0x00000300
#define SERVER_IIS_7_0					0x00000400
#define SERVER_IIS_7_5					0x00000500
#define SERVER_IIS_WAF					0x00000600

#define SERVER_APACHE					0x00001000

#define SERVER_TOMCAT					0x00002000
	
#define SERVER_ASP						0x00010000
#define SERVER_ASPX						0x00020000
#define SERVER_PHP						0x00100000
#define SERVER_PHPCGI					0x00110000
#define SERVER_PYTHON					0x00120000
#define SERVER_STRUCT					0x00200000

#define SERVER_WINDOWS					0x01000000
#define SERVER_LINUX					0x02000000

#define SERVER_OTHER					0x03000000

enum cms_type
{
	CMS_ALL							= 0x00,

	CMS_DISCUZ,
	CMS_DISCUZ_X2,
	CMS_DISCUZ_NT,

	CMS_DEDECMS,

	CMS_OTHER
};

class CDeepScanMdl
{
public:
	CDeepScanMdl(void);
	~CDeepScanMdl(void);

public:
	void InitModule(CTreeCtrl* moduleTree, CEdit* interactiveBox);
	void LoadModuleDir();
	HTREEITEM MakeCatgory(CString ModuleCatgory);
	void LoadWAEFModule(CString ModulePath);
	void LoadModuleDesc();
	void LoadModuleConfig();
	void ExecuteModule();


private:
	CTreeCtrl*		_moduleTree;
	static CEdit*	_interactiveBox;
	CExploitModule	_exploit;

	HINSTANCE		_hModule;
};

