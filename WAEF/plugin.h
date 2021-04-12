#pragma once

#include <windows.h>


typedef LRESULT (CALLBACK* WAEF_PLUGIN_NOTIFYPROC)(LPVOID Param);

enum error_plugin
{
	ERROR_WAEFPLGUIN_SUCCESS,
	ERROR_WAEFPLGUIN_VULNERABLE,
	ERROR_WAEFPLGUIN_NOTVULNERABLE,

	ERROR_WAEFPLGUIN_VULNERABLE_WAIT,
	ERROR_WAEFPLGUIN_CONTINUE,
	ERROR_WAEFPLGUIN_FINISH,

	ERROR_WAEFPLGUIN_CONFIG_ERROR
};

typedef struct st_init
{
	error_plugin			error_rtn;
	int						outcode;
	char					*inPtr;                   /* free pointer for function data */
	char					*outPtr;                   /* free pointer for function data */
	WAEF_PLUGIN_NOTIFYPROC	pNotify;
} WAEF_PLUGIN_INIT;


enum plugin_info
{
	plugin_name,
	plugin_desc,
	plugin_config
};


typedef error_plugin (_cdecl * LPFNDLLFUNC_WAEF_MODULE_FUNC)(WAEF_PLUGIN_INIT*);

bool GetPluginInfo(HINSTANCE hModule, plugin_info info, CString& Rtn);