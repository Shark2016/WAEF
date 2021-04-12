#include "stdafx.h"

#include "plugin.h"


bool GetPluginInfo(HINSTANCE hModule, plugin_info info, CString& Rtn)
{
	CString FunctionName;
	switch(info)
	{
	case plugin_name:
		FunctionName="WAEF_GetPluginName";
		break;
	case plugin_desc:
		FunctionName="WAEF_GetPluginDesc";
		break;
	case plugin_config:
		FunctionName="WAEF_GetPluginConfig";
		break;
	default:
		return false;
	}

	WAEF_PLUGIN_INIT initid;
	LPFNDLLFUNC_WAEF_MODULE_FUNC lpfnWAEFGetModuleName;

	lpfnWAEFGetModuleName = (LPFNDLLFUNC_WAEF_MODULE_FUNC)GetProcAddress(hModule, FunctionName);
	if (!lpfnWAEFGetModuleName)
		return false;
	if(ERROR_WAEFPLGUIN_SUCCESS != lpfnWAEFGetModuleName(&initid))
		return false;
	Rtn = initid.outPtr;
	free(initid.outPtr);
	return true;
}