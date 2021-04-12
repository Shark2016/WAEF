
// WAEF.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "WAEF.h"
#include "MainFrm.h"

#include "WAEFSet.h"
#include "WAEFDoc.h"
#include "WAEFView.h"

#include "SkinH/SkinH.h"
#pragma comment(lib, "SkinH/SkinH.lib")

#include "SplashDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWAEFApp

BEGIN_MESSAGE_MAP(CWAEFApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CWAEFApp::OnAppAbout)
END_MESSAGE_MAP()


// CWAEFApp 构造

CWAEFApp::CWAEFApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则:
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("WAEF.AppID.NoVersion"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CWAEFApp 对象

CWAEFApp theApp;


// CWAEFApp 初始化

BOOL CWAEFApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	CoInitialize(NULL);

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// 使用 RichEdit 控件需要  AfxInitRichEdit2()	
	// AfxInitRichEdit2();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	LoadStdProfileSettings(0);  // 加载标准 INI 文件选项(包括 MRU)



	SkinH_Attach();
	
	//加皮肤(资源方式)
	HGLOBAL hRes;
	HRSRC hResInfo;
	HINSTANCE hinst = AfxGetInstanceHandle();
	hResInfo = FindResource( hinst, MAKEINTRESOURCE(IDR_SKIN1), "SKIN" );
	if (hResInfo != NULL)
	{
		hRes = LoadResource( hinst, hResInfo);
		if (hRes != NULL)
		{
			SkinH_AttachRes( (LPBYTE)hRes, SizeofResource(hinst,hResInfo), NULL, NULL, NULL, NULL );
			//SkinH_AdjustAero(200,200,255,10,0,0,-1,-1,-1);
			//SkinH_SetAero(TRUE);
			FreeResource(hRes);
		}
	}


	CSplashDlg m_pSplashDlg;
	m_pSplashDlg.DoModal();/*
	m_pSplashDlg.Create(IDD_SPLASH_DLG);
	m_pSplashDlg.ShowWindow(SW_SHOW); 
	DWORD dwStart = GetTickCount();
    DWORD dwEnd   = dwStart;
    do
    {
		MSG   msg;
		GetMessage(&msg,NULL,0,0);
		TranslateMessage(&msg); 
		DispatchMessage(&msg);
		dwEnd = GetTickCount()-dwStart;
	}while(dwEnd <2000);
	m_pSplashDlg.SendMessage(WM_CLOSE);
	m_pSplashDlg.FlashWindow(true);*/

	// 注册应用程序的文档模板。文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWAEFDoc),
		RUNTIME_CLASS(CMainFrame),       // 主 SDI 框架窗口
		RUNTIME_CLASS(CWAEFView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// 唯一的一个窗口已初始化，因此显示它并对其进行更新
	// m_pMainWnd->SetMenu(NULL); //去菜单
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

int CWAEFApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	AfxOleTerm(FALSE);

	CoUninitialize();

	return CWinApp::ExitInstance();
}

// CWAEFApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
//	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CWAEFApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
