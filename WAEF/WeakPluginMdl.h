#pragma once
class CWeakPluginMdl
{
public:
	CWeakPluginMdl(void);
	~CWeakPluginMdl(void);

public:
	void InitModule(CListCtrl* pluginList, CEdit* interactiveBox, CListCtrl* resultList);
	void SetTarget(CString target);
	void LoadPluginDir(CString ModulePath);
	void LoadWAEFPlugin(int nIndex);
	void LoadPluginDesc();
	void LoadPluginConfig();
	void ExecutePlugin();
//	void setNotify(WAEF_PLUGIN_NOTIFYPROC pNotify, WAEF_PLUGIN_INIT* initid) {initid->pNotify = pNotify;};

	static LRESULT CALLBACK NotifyProc(LPVOID Param);

private:
	static CString			_target;
	CListCtrl*				_pluginList;
	static CListCtrl*		_resultList;
	static CEdit*			_interactiveBox;

	HINSTANCE		_hPlugin;
};

