#include "stdafx.h"
#include "CodeAuditMdl.h"

#include <boost/regex.hpp>

#include <string>
#include <regex>

#define RULE_COUNT		21

char* ruleReg[] = {
	"\\b(include|require)(_once){0,1}(\\s{1,5}|\\s{0,5}\\().{0,60}\\$(?!.*(this->))\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"\\bpreg_replace\\(\\s{0,5}.*/[is]{0,2}e[is]{0,2}[\"']\\s{0,5},(.*\\$.*,|.*,.*\\$)",
	"\\bphpinfo\\s{0,5}\\(\\s{0,5}\\)",
	"\\bcall_user_func(_array){0,1}\\(\\s{0,5}\\$\\w{1,15}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"\\b(file_get_contents|fopen|readfile|fgets|fread|parse_ini_file|highlight_file|fgetss|show_source)\\s{0,5}\\(.{0,40}\\$\\w{1,15}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"\\b(system|passthru|pcntl_exec|shell_exec|escapeshellcmd|exec)\\s{0,10}\\(.{0,40}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"\\b(mb_){0,1}parse_str\\s{0,10}\\(.{0,40}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"\\${{0,1}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}\\s{0,4}=\\s{0,4}.{0,20}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"[\"'](HTTP_CLIENT_IP|HTTP_X_FORWARDED_FOR|HTTP_REFERER)[\"']",
	"\\b(unlink|copy|fwrite|file_put_contents|bzopen)\\s{0,10}\\(.{0,40}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"\\b(extract)\\s{0,5}\\(.{0,30}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}\\s{0,5},{0,1}\\s{0,5}(EXTR_OVERWRITE){0,1}\\s{0,5}\\)",
	"\\$[\\w]{1,15}(\\[.{1,10}\\]){0,1}\\(\\$_(POST|GET|REQUEST)\\[.{1,20}\\]",
	"^(?!.*\\baddslashes).{0,40}\\b((raw){0,1}urldecode|stripslashes)\\s{0,5}\\(.{0,60}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"`\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}`",
	"\\barray_map\\s{0,4}\\(\\s{0,4}.{0,20}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}\\s{0,4}.{0,20},",
	"\\bob_start\\s{0,4}\\(\\s{0,4}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"(select|SELECT)\\s{1,4}.{1,60}(FROM|from).{1,50}\\b(where|WHERE)\\s{1,3}.{1,30}=[\"\\s\\.]{0,10}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"(delete|DELETE)\\b.{1,50}\\b(where|WHERE)\\s{1,3}.{1,30}=[\"\\s\\.]{0,10}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}",
	"(insert|INSERT)\\s{0,3}(into|INTO)\\s{1,3}.{1,60}\\b(values|VALUES|Values)\\s{0,5}\\(.{0,50}(\\$\\w{1,15}\\s{0,3},|,\\s{0,3}\\$\\w{1,15}\\s{0,3}).{1,50}",
	"(update|UPDATE|Update)\\s{1,4}.{1,30}\\s{1,3}(set|SET|Set)\\s{1,3}\\w{1,15}\\s{0,3}=([\"\\s\\.]{0,10}\\$\\w{1,20}|.{1,50}\\b(where|WHERE)\\s{1,3}.{1,30}=[\"\\s\\.]{0,10}\\$\\w{1,20})",
	"\\b(eval|assert)\\s{0,10}\\(.{0,60}\\$\\w{1,20}((\\[[\"']|\\[)\\${0,1}[\\w\\[\\]\"']{0,30}){0,1}.*"
};

char* ruleDesc[] = {
	"�ļ����������д��ڱ���,���ܴ����ļ�����©��",
	"preg_replace��/eģʽ�����пɿر��������ܴ��ڴ���ִ��©��",
	"phpinfo()���������ܴ���������Ϣй¶©��",
	"call_user_func���������������������ܴ��ڴ���ִ��©��",
	"��ȡ�ļ������д��ڱ��������ܴ��������ļ���ȡ©��",
	"����ִ�к����д��ڱ��������ܴ�����������ִ��©��",
	"parse_str�����д��ڱ���,���ܴ��ڱ�������©��",
	"˫$$���ſ��ܴ��ڱ�������©��",
	"��ȡIP��ַ��ʽ��α�죬HTTP_REFERER��α�죬��������SQLע���©��",
	"�ļ����������д��ڱ��������ܴ��������ļ���ȡ/ɾ��/�޸�/д���©��",
	"extract�����д��ڱ��������ܴ��ڱ�������©��",
	"���ܴ��ڴ���ִ��©��,���ߴ˴��Ǻ���",
	"urldecode��������GPC,stripslashes��ȡ��GPCת���ַ�",
	"``�������а��������������ɿػᵼ������ִ��©��",
	"array_map��һ���������������������ɿؿ��ܻᵼ�´���ִ��©��",
	"ob_start�������������������ɿػᵼ�´���ִ��©��",
	"SQL���select�����������޵����ű��������ܴ���SQLע��©��",
	"SQL���delete�����������޵����ű��������ܴ���SQLע��©��",
	"SQL���insert�в�������޵����ű��������ܴ���SQLע��©��",
	"SQL���delete�����������޵����ű��������ܴ���SQLע��©��",
	"eval����assertc�����д��ڱ��������ܴ��ڴ���ִ��©��"
};

CTreeCtrl* CCodeAuditMdl::_projectTree;
CListCtrl* CCodeAuditMdl::_resultList;
CString    CCodeAuditMdl::_dirPath;

CCodeAuditMdl::CCodeAuditMdl(void)
{
}


CCodeAuditMdl::~CCodeAuditMdl(void)
{
}

void CCodeAuditMdl::InitModule(CTreeCtrl* projectTree, CListCtrl* resultList)
{
	_projectTree = projectTree;
	_resultList = resultList;
};

void CCodeAuditMdl::LoadProjectDir(CString dirPath)
{
	_dirPath = dirPath;
	_projectTree->DeleteAllItems();
	UINT nMask = TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT;
	int nImage=GetIconIndex(dirPath, GetFileAttributes(dirPath));
	HTREEITEM  hItem=_projectTree->InsertItem(nMask, "����Ŀ¼", nImage, nImage,0, 0, 0, TVI_ROOT, TVI_LAST );
	HANDLE thread = CreateThread(NULL,0,load_proc,hItem,0,NULL);
	CloseHandle(thread);

};

DWORD WINAPI CCodeAuditMdl::load_proc(LPVOID lpParam)
{
	HTREEITEM  hItem= (HTREEITEM)lpParam;
	LoadDir(_dirPath, hItem);
	return 0;
}


void CCodeAuditMdl::LoadDir(CString dirPath, HTREEITEM hParentItem)
{
	CString strTemp;
	if(dirPath.Right(1)!="\\")
		strTemp.Format("%s\\*.*",dirPath);
	else
		strTemp.Format("%s*.*",dirPath);
	CFileFind findFile;
	BOOL bfind=findFile.FindFile(strTemp);
	UINT nMask = TVIF_HANDLE|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_STATE|TVIF_TEXT;
	HTREEITEM  hItem;
	while(bfind)
	{
		bfind=findFile.FindNextFile();
		if(findFile.IsDots())
		{
			continue;
		}
		CString path = findFile.GetFilePath();
		int nImage=GetIconIndex(path, GetFileAttributes(path));
		if(findFile.IsDirectory())
		{
			hItem=_projectTree->InsertItem(nMask, findFile.GetFileName(), nImage, nImage,0, 0, 0, hParentItem, TVI_LAST );
			LoadDir(findFile.GetFilePath(), hItem);
		}
		else
		{
			hItem=_projectTree->InsertItem(nMask, findFile.GetFileName(), nImage, nImage,0, 0, 0, hParentItem, TVI_LAST );
		}
	}

}

int CCodeAuditMdl::GetIconIndex(LPCTSTR lpFileName, DWORD dwFileAttributes)
{
	SHFILEINFO	sfi;
	if (dwFileAttributes == INVALID_FILE_ATTRIBUTES)
		dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
	else
		dwFileAttributes |= FILE_ATTRIBUTE_NORMAL;

	SHGetFileInfo
		(
		lpFileName,
		dwFileAttributes, 
		&sfi,
		sizeof(SHFILEINFO), 
		SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES
		);

	return sfi.iIcon;
}

void CCodeAuditMdl::AuditProject()
{
	HANDLE thread = CreateThread(NULL,0,audit_proc,NULL,0,NULL);
	CloseHandle(thread);
}


DWORD WINAPI CCodeAuditMdl::audit_proc(LPVOID lpParam)
{
	AuditDir(_dirPath);
	return 0;
}

void CCodeAuditMdl::AuditDir(CString dirPath)
{
	CString strTemp;
	if(dirPath.Right(1)!="\\")
		strTemp.Format("%s\\*.*",dirPath);
	else
		strTemp.Format("%s*.*",dirPath);
	CFileFind findFile;
	BOOL bfind=findFile.FindFile(strTemp);
	while(bfind)
	{
		bfind=findFile.FindNextFile();
		if(findFile.IsDots())
		{
			continue;
		}
		if(findFile.IsDirectory())
		{
			AuditDir(findFile.GetFilePath());
		}
		else
		{
			CString fileName = findFile.GetFileName();
			int dot = fileName.ReverseFind('.');
			if(dot!=-1)
			{
				CString ext = fileName.Right(fileName.GetLength()-dot-1).MakeLower();
				if(ext == "php" || ext == "inc")		//ֻ���ָ����ʽ�ļ�
				{
					AuditFile(findFile.GetFilePath());
				}
			}
		}
	}
}

void CCodeAuditMdl::AuditFile(CString filePath)
{
	/*
	FILE * file;
	fopen_s(&file, filePath.GetBuffer(), "rb");
	char buffer[1024];
	std::string text;
	while(!feof(file))
	{
		fread(buffer, 1, 1024, file);
		text += buffer;
	}
	*/

	HANDLE hFile=CreateFile(filePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL,0);
	if (!hFile)				   
		return;

	BY_HANDLE_FILE_INFORMATION info;
	GetFileInformationByHandle(hFile, &info);

	if(info.nFileSizeLow < 4)
	{
		CloseHandle(hFile);
		return;
	}

	HANDLE hMapping=CreateFileMapping(hFile,NULL,PAGE_READONLY,info.nFileSizeHigh,info.nFileSizeLow,NULL);
	if(!hMapping)
	{
		CloseHandle(hFile);
		return;
	}
	LPVOID ImageBase=MapViewOfFile(hMapping,FILE_MAP_READ,0,0,info.nFileSizeLow);
    if(!ImageBase)
	{
		CloseHandle(hMapping);
		CloseHandle(hFile);
		return;
	}

	WORD dwHeader = *(WORD*)ImageBase;

	int nb1, nb2;
	WCHAR* wBuf;
	TCHAR* aBuf;
	std::string content;
	std::string split;
	switch(dwHeader)
	{
	case 0xBBEF:		// UTF-8				// EF BB BF
		if( ((TCHAR*)ImageBase)[2] == '\xbf')
		{
			nb1 = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)ImageBase + 3, info.nFileSizeLow-3, NULL, 0);
			wBuf = new WCHAR[nb1];
			MultiByteToWideChar(CP_UTF8, 0, (LPCCH)ImageBase + 3, info.nFileSizeLow-3, wBuf, nb1);
			// ���������Unicode���ԣ�������Ϳ���ֱ��ת����
			// �������Ƕ��ַ����ԣ�Ҫ��ת����ANSI
			nb2 = WideCharToMultiByte(CP_ACP, 0, wBuf, -1, NULL, 0, NULL, NULL);
			aBuf = new TCHAR[nb2];
			WideCharToMultiByte(CP_ACP, 0, wBuf, -1, aBuf, nb2, NULL, NULL);
			content = aBuf;
			split = "\n";
			delete [] wBuf;
			delete [] aBuf;
		}
		break;
	case 0xEFFF:		// Unicode
		break;
	case 0xFFFE:		// Unicode big endian
		break;
	default:			// ANSI
		content = (TCHAR*)ImageBase;
		split = "\n";
		break;
	}

	UnmapViewOfFile(ImageBase);
	CloseHandle(hMapping);
	CloseHandle(hFile);



	int pos1 = 0;
	int pos2 = content.find(split);
	std::string line;

	int nline = 1;

	while(1)
	{
		if(pos2==-1)
		{
			line = content.substr(pos1);
		}
		else 
		{
			line = content.substr(pos1, pos2-pos1);
			pos1 = pos2 + split.length();
		}


		int i;
		for(i = 0;i < RULE_COUNT; i++)
		{
			boost::cmatch mat;
			boost::regex reg(ruleReg[i]);
			bool r= boost::regex_search(line.c_str(), mat, reg);
			if(r) //���ƥ��ɹ�
			{
				CString itemText;
				int count = _resultList->GetItemCount();
				itemText.Format("%d", count+1);
				_resultList->InsertItem(count, itemText);
				itemText.Format("%s(line: %d)", filePath, nline);
				_resultList->SetItemText(count, 1,  itemText);
				_resultList->SetItemText(count, 2,  line.c_str());
				_resultList->SetItemText(count, 3,  ruleDesc[i]);
			}
		}
		if(pos2 == -1)
			break;
		nline ++;
		pos2 = content.find(split, pos1);
	}
}