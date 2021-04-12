#pragma once
class CCodeAuditMdl
{
public:
	CCodeAuditMdl(void);
	~CCodeAuditMdl(void);

public:
	void InitModule(CTreeCtrl* projectTree, CListCtrl* resultList);
	void LoadProjectDir(CString dirPath);
	void AuditProject();
	
	static DWORD WINAPI audit_proc(LPVOID lpParam);
	static void AuditDir(CString dirPath);
	static void AuditFile(CString filePath);

	static DWORD WINAPI load_proc(LPVOID lpParam);
	static int GetIconIndex(LPCTSTR lpFileName, DWORD dwFileAttributes);
	static void LoadDir(CString dirPath, HTREEITEM hParentItem);

private:
	static CTreeCtrl*		_projectTree;
	static CListCtrl*		_resultList;
	static CString			_dirPath;

};

