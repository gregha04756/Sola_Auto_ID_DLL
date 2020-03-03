// Auto_Discovery_Test.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Auto_Discovery_Test.h"

using namespace std;

#define MAX_LOADSTRING 100
#if 0
#define WM_APPAIDCALLBACK WM_APP + 0
#define WM_APPAIDSCANUPDCALLBACK WM_APP + 1
#define WM_APPAIDSCANDONECALLBACK WM_APP + 2
#endif

// Global Variables:
HINSTANCE hInst;								// current instance
HWND g_h_Wnd;
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
static void Auto_ID_Callback(DWORD dw_code);
static void Auto_ID_Scan_Upd_Callback(std::wstring *p);
static void Auto_ID_Scan_Done_Callback(void);
static void Dlg_End_Callback(void *p_v);
static void Scan_Done_Handler(void *p_v);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;
	DWORD dw_rc;
	INITCOMMONCONTROLSEX icc_e={0};

	icc_e.dwSize=sizeof(icc_e);
	icc_e.dwICC = ICC_STANDARD_CLASSES|ICC_INTERNET_CLASSES;
	InitCommonControlsEx(&icc_e);
	dw_rc = GetLastError();

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_AUTO_DISCOVERY_TEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AUTO_DISCOVERY_TEST));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AUTO_DISCOVERY_TEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_AUTO_DISCOVERY_TEST);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   g_h_Wnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!g_h_Wnd)
   {
      return FALSE;
   }

   ShowWindow(g_h_Wnd, nCmdShow);
   UpdateWindow(g_h_Wnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL b_r;
	int i_r;
	static int cx;
	static int cy;
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static CSola_Auto_ID_DLL *p_sid;
	static Ctrash81_Modeless_Dlg_DLL *p_aidd;
	TEXTMETRIC tm;
	std::list<uint8_t> *addr_list;
	std::list<uint8_t>::iterator addr_it;
	std::list<CSola_Auto_ID_DLL::SOLADEVICECOORDS>::iterator sdc_it;
	std::wstring wstr_addr;
	uint8_t ui8_addr;
	std::wstring *p_scan_upd;
	static CSola_Auto_ID_DLL::conn_type ct;
	static Ctrash81_Modeless_Dlg_DLL::LPCONNTYPEPARMS lp_ctp;
	static struct in_addr ia;
	INT_PTR ip_dbr;
	static std::string str_IP_Address;
	static std::wstring wstr_IP_Address;
	static std::list<wstring> *lp_ipwsl;
	std::list<wstring>::iterator wstr_it;
	TCHAR sz_IP_Address[100];
	PVOID p_v;
	static std::list<CSola_Auto_ID_DLL::SOLADEVICECOORDS> *p_SDCL;

	switch (message)
	{
	case WM_CREATE:
		p_SDCL = NULL;
		p_sid = NULL;
		p_v = SecureZeroMemory((PVOID)sz_IP_Address,sizeof(sz_IP_Address));
		str_IP_Address.clear();
		ia.S_un.S_addr = 0L;
		try
		{
			lp_ctp = reinterpret_cast<Ctrash81_Modeless_Dlg_DLL::LPCONNTYPEPARMS>(new Ctrash81_Modeless_Dlg_DLL::CONNTYPEPARMS);
			lp_ctp->ct = CSola_Auto_ID_DLL::conn_type::RTU;
			lp_ctp->p_lipddn = NULL;
		}
		catch (std::bad_alloc &ba)
		{
			ReportError(ba.what());
		}
		try
		{
			lp_ipwsl = reinterpret_cast<std::list<wstring>*>(new std::list<wstring>);
		}
		catch (std::bad_alloc &ba)
		{
			ReportError(ba.what());
		}
		p_aidd = NULL;
		cx = 10;
		cy = 0;
		break;
	case WM_APPAIDSCANDONECALLBACK:
		if (NULL != lParam)
		{
			if (NULL != p_SDCL)
			{
				delete p_SDCL;
			}
			p_SDCL = reinterpret_cast<list<CSola_Auto_ID_DLL::SOLADEVICECOORDS>*>(new list<CSola_Auto_ID_DLL::SOLADEVICECOORDS>);
			if (NULL != p_aidd->Get_SID()->Get_SDC_List())
			{
				for (sdc_it = p_aidd->Get_SID()->Get_SDC_List()->begin();sdc_it != p_aidd->Get_SID()->Get_SDC_List()->end();sdc_it++)
				{
					p_SDCL->push_back(*sdc_it);
				}
			}
		}
		b_r = InvalidateRect(hWnd,NULL,true);
		break;
	case WM_APPDLGENDING:
		if (NULL != p_aidd)
		{
			delete p_aidd;
			p_aidd = NULL;
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_AUTOID:
			if( NULL == p_aidd)
			{
				try
				{
					p_aidd = reinterpret_cast<Ctrash81_Modeless_Dlg_DLL*>(new Ctrash81_Modeless_Dlg_DLL(hWnd,hInst,szTitle));
				}
				catch(std::bad_alloc &ba)
				{
					ReportError(ba.what());
					break;
				}
				if (NULL != lp_ctp)
				{
					p_aidd->Set_Callback(Dlg_End_Callback);
					p_aidd->Set_Scan_Done_Handler(Scan_Done_Handler);
					b_r = p_aidd->Run(lp_ctp);
				}
			}
			break;
		case IDM_ABOUT:
			if (NULL != lp_ctp)
			{
				lp_ctp->ct = CSola_Auto_ID_DLL::conn_type::RTU;
				lp_ctp->p_lipddn = lp_ipwsl;
				ip_dbr = DialogBoxParam(hInst,MAKEINTRESOURCE(IDD_ABOUTBOX),hWnd,About,(LPARAM)lp_ctp);
				if (IDOK == ip_dbr)
				{
					cy = 0;
					b_r = InvalidateRect(hWnd,NULL,true);
				}
			}
			break;
		case IDM_EXIT:
			if (NULL != p_aidd)
			{
				delete p_aidd;
				p_aidd = NULL;
			}
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SIZE:
		cy = 0;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		b_r = GetTextMetrics(hdc,&tm);
		// TODO: Add any drawing code here...
		if (NULL != lp_ctp)
		{
			if ((CSola_Auto_ID_DLL::conn_type::TCP == lp_ctp->ct) && !lp_ctp->p_lipddn->empty())
			{
				cy += 10;
				for (wstr_it = lp_ctp->p_lipddn->begin(); wstr_it != lp_ctp->p_lipddn->end(); wstr_it++)
				{
					b_r = TextOut(hdc,cx,cy,wstr_it->c_str(),wstr_it->size());
					cy += tm.tmHeight+tm.tmAscent;
				}
			}
		}

		if (NULL != p_aidd)
		{
			if (!p_aidd->Get_SID()->Get_SDC_List()->empty())
			{
				cy += 10;
				for (sdc_it = p_SDCL->begin(); sdc_it != p_SDCL->end(); sdc_it++)
				{
					ui8_addr = sdc_it->ui8_addr;
					wstr_addr.assign(_T("Sola found on "));
					wstr_addr.append(sdc_it->Interface_Name);
					wstr_addr.append(std::wstring(_T(" at address ")));
					wstr_addr.append(std::to_wstring((LONGLONG)(sdc_it->ui8_addr)));
					b_r = TextOut(hdc,cx,cy,wstr_addr.c_str(),wstr_addr.size());
					cy += tm.tmHeight+tm.tmAscent;
				}
			}
		}

		EndPaint(hWnd, &ps);
		cy = 0;
		break;
	case WM_DESTROY:
		if (NULL != lp_ctp)
		{
			delete lp_ctp;
			lp_ctp = NULL;
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL b_r;
	DWORD dw_rc;
	int i_r;
	LRESULT l_r;
	int ip_cnt;
	static Ctrash81_Modeless_Dlg_DLL::LPCONNTYPEPARMS lpp;
	static HWND hwnd_ipc;
	DWORD dw_IP;
	static std::wstring wstr_IP_Address;
	static std::string str_IP_Address;
	TCHAR sz_IP_Address[100];
	static struct in_addr ia;
	PVOID p_v;
	static HWND hwnd_iplb;

	switch (message)
	{
	case WM_INITDIALOG:
		hwnd_iplb = GetDlgItem(hDlg,IDC_LBIPLIST);
		l_r = SendDlgItemMessage(hDlg,IDC_BTNRTU,BM_SETCHECK,(WPARAM)BST_CHECKED,(LPARAM)0);
		lpp = reinterpret_cast<Ctrash81_Modeless_Dlg_DLL::LPCONNTYPEPARMS>(lParam);
		hwnd_ipc = CreateWindow(
			WC_IPADDRESS, 
			L"", 
			WS_CHILD|WS_OVERLAPPED|WS_VISIBLE,
			70, 
			190,
			150, 
			20,  
			hDlg, 
			NULL, 
			hInst, 
			NULL);
		dw_rc = GetLastError();
		if (ERROR_SUCCESS != dw_rc)
		{
			ReportError(dw_rc);
			return (INT_PTR)TRUE;
		}
		if (NULL != hwnd_ipc)
		{
			b_r = EnableWindow(hwnd_ipc,false);
			b_r = EnableWindow(GetDlgItem(hDlg,IDC_BTNADDIP),false);
			b_r = EnableWindow(GetDlgItem(hDlg,IDC_LBIPLIST),false);
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDC_BTNRTU)
		{
			if (NULL != hwnd_ipc)
			{
				b_r = EnableWindow(hwnd_ipc,false);
				b_r = EnableWindow(GetDlgItem(hDlg,IDC_BTNADDIP),false);
				b_r = EnableWindow(GetDlgItem(hDlg,IDC_LBIPLIST),false);
			}
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_BTNTCP)
		{
			if (NULL != hwnd_ipc)
			{
				b_r = EnableWindow(hwnd_ipc,true);
				b_r = EnableWindow(GetDlgItem(hDlg,IDC_BTNADDIP),true);
				b_r = EnableWindow(GetDlgItem(hDlg,IDC_LBIPLIST),true);
				l_r = SendMessage(GetDlgItem(hDlg,IDC_LBIPLIST),CB_SETCURSEL,(WPARAM)0,(LPARAM)0);
			}
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_BTNADDIP)
		{
			l_r = SendMessage(GetDlgItem(hDlg,IDC_BTNRTU),BM_GETCHECK,(WPARAM)0,(LPARAM)0);
			lpp->ct = (l_r == BST_CHECKED) ? CSola_Auto_ID_DLL::conn_type::RTU : CSola_Auto_ID_DLL::conn_type::TCP;
			if (CSola_Auto_ID_DLL::conn_type::TCP == lpp->ct)
			{
				l_r = SendMessage(hwnd_ipc,IPM_GETADDRESS,(WPARAM)0,(LPARAM)&dw_IP);
				if (0L == dw_IP)
				{
					return (INT_PTR)TRUE;
				}
				p_v = SecureZeroMemory((PVOID)sz_IP_Address,sizeof(sz_IP_Address));
				ia.S_un.S_addr = ntohl(dw_IP);
				str_IP_Address.assign(inet_ntoa(ia));
				MultiByteToWideChar(
					CP_ACP,
					MB_PRECOMPOSED,
					str_IP_Address.c_str(),
					str_IP_Address.size(),
					sz_IP_Address,
					sizeof(sz_IP_Address)/sizeof(TCHAR));
				wstr_IP_Address.assign(sz_IP_Address);
				if (CB_ERR == (l_r = SendMessage(GetDlgItem(hDlg,IDC_LBIPLIST),CB_FINDSTRING,(WPARAM)-1,(LPARAM)wstr_IP_Address.c_str())))
				{
					l_r = SendMessage(GetDlgItem(hDlg,IDC_LBIPLIST),CB_ADDSTRING,(WPARAM)0,(LPARAM)wstr_IP_Address.c_str());
				}
				l_r = SendMessage(GetDlgItem(hDlg,IDC_LBIPLIST),CB_SETCURSEL,(WPARAM)0,(LPARAM)0);
			}
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDOK)
		{
			l_r = SendMessage(GetDlgItem(hDlg,IDC_BTNRTU),BM_GETCHECK,(WPARAM)0,(LPARAM)0);
			lpp->ct = (l_r == BST_CHECKED) ? CSola_Auto_ID_DLL::conn_type::RTU : CSola_Auto_ID_DLL::conn_type::TCP;
			if (CSola_Auto_ID_DLL::conn_type::TCP == lpp->ct)
			{
				if (0 == (ip_cnt = (int)SendMessage(GetDlgItem(hDlg,IDC_LBIPLIST),CB_GETCOUNT,(WPARAM)0,(LPARAM)0)))
				{
					i_r = MessageBox(hDlg,_T("Must enter at least one IP address"),szTitle,MB_OK);
					return (INT_PTR)TRUE;
				}
				for (i_r = 0; i_r < ip_cnt; i_r++)
				{
					l_r = SendMessage(GetDlgItem(hDlg,IDC_LBIPLIST),CB_GETLBTEXT,(WPARAM)i_r,(LPARAM)sz_IP_Address);
					lpp->p_lipddn->push_back(wstring(sz_IP_Address));
				}
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Auto_ID_Callback(DWORD dw_code)
{
	BOOL b_r;
	b_r = PostMessage(g_h_Wnd,WM_APPAIDCALLBACK,(WPARAM)0,(LPARAM)dw_code);
}

void Auto_ID_Scan_Upd_Callback(std::wstring *p)
{
	BOOL b_r;
	b_r = PostMessage(g_h_Wnd,WM_APPAIDSCANUPDCALLBACK,(WPARAM)0,(LPARAM)p);
}

void Auto_ID_Scan_Done_Callback(void)
{
}


void Dlg_End_Callback(void *p_v)
{
	PostMessage(g_h_Wnd,WM_APPDLGENDING,(WPARAM)0,(LPARAM)0);
}

void Scan_Done_Handler(void *p_v)
{
	BOOL b_r;
	b_r = PostMessage(g_h_Wnd,WM_APPAIDSCANDONECALLBACK,(WPARAM)0,(LPARAM)p_v);
}