// Sola_Auto_ID_DLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Sola_Auto_ID_DLL.h"

// This is the constructor of a class that has been exported.
// see Sola_Auto_ID_DLL.h for the class definition
CSola_Auto_ID_DLL::CSola_Auto_ID_DLL(uint8_t const la,
	uint8_t const ha):m_ui8_Low_Range_MB_Addr(la),m_ui8_High_Range_MB_Addr(ha)
{
	m_connection = RTU;
	if ((m_ui8_High_Range_MB_Addr < m_ui8_Low_Range_MB_Addr) ||
		(0 == m_ui8_High_Range_MB_Addr) ||
		(0 == m_ui8_Low_Range_MB_Addr))
	{
		this->m_ui8_High_Range_MB_Addr = 8;
		this->m_ui8_Low_Range_MB_Addr = 1;
	}
	m_pcf = NULL;
	m_sucfp = NULL;
	m_sdcfp = NULL;
	m_p_SDCL = NULL;
	m_h_Work_Thread = NULL;
	m_hdup_Work_Thread = NULL;
	m_pstr_IP = NULL;
	m_pwstr_IP = NULL;
	m_p_lipddn = NULL;
	m_p_to_owner = NULL;
	m_st_ip_len = 0;
	m_int_Port = 0;
	m_dw_Work_Thread_ID = (DWORD)0;
	try
	{
		m_p_SDCL = dynamic_cast<std::list<SOLADEVICECOORDS>*>(new std::list<SOLADEVICECOORDS>);
	}
	catch (std::bad_alloc &ba)
	{
		throw ba;
	}
	m_h_Cancel_Event = CreateEvent(NULL,true,false,NULL);
}

CSola_Auto_ID_DLL::CSola_Auto_ID_DLL(
	uint8_t const la,
	uint8_t const ha,
	std::list<wstring> const *lip,
	int const port):m_ui8_Low_Range_MB_Addr(la),m_ui8_High_Range_MB_Addr(ha),m_int_Port(port)
{
	m_connection = TCP;
	if ((m_ui8_High_Range_MB_Addr < m_ui8_Low_Range_MB_Addr) ||
		(0 == m_ui8_High_Range_MB_Addr) ||
		(0 == m_ui8_Low_Range_MB_Addr))
	{
		this->m_ui8_High_Range_MB_Addr = 8;
		this->m_ui8_Low_Range_MB_Addr = 1;
	}
	m_pcf = NULL;
	m_sucfp = NULL;
	m_sdcfp = NULL;
	m_p_SDCL = NULL;
	m_h_Work_Thread = NULL;
	m_dw_Work_Thread_ID = 0UL;
	m_hdup_Work_Thread = NULL;
	m_pwstr_IP = NULL;
	m_pstr_IP = NULL;
	m_p_to_owner = NULL;
	m_st_ip_len = 0;
	m_int_Port = (0 == m_int_Port) ? STD_MODBUS_TCP_PORT : m_int_Port;
/*	hres_r = StringCchLength(ip,MAX_IP_LENGTH,&m_st_ip_len);*/
	try
	{
		m_p_lipddn = dynamic_cast<std::list<wstring>*>(new std::list<wstring>);
	}
	catch (std::bad_alloc &ba)
	{
		throw ba;
	}
	try
	{
		*m_p_lipddn = *lip;
	}
	catch (std::exception &exc)
	{
		throw exc;
	}
	try
	{
		m_p_SDCL = dynamic_cast<std::list<SOLADEVICECOORDS>*>(new std::list<SOLADEVICECOORDS>);
	}
	catch (std::bad_alloc &ba)
	{
		throw ba;
	}
	m_h_Cancel_Event = CreateEvent(NULL,true,false,NULL);
}

CSola_Auto_ID_DLL::~CSola_Auto_ID_DLL()
{
	BOOL b_r;
	DWORD dw_wr;
	DWORD dw_rc;
	if (NULL != m_h_Cancel_Event)
	{
		SetEvent(m_h_Cancel_Event);
	}
	if (NULL != m_hdup_Work_Thread)
	{
		dw_wr = WaitForSingleObject(m_hdup_Work_Thread,5000);
		dw_rc = GetLastError();
	}
	if (NULL != m_hdup_Work_Thread)
	{
		b_r = CloseHandle(m_hdup_Work_Thread);
		m_hdup_Work_Thread = NULL;
	}
	if (NULL != m_h_Work_Thread)
	{
		b_r = CloseHandle(m_h_Work_Thread);
		m_h_Work_Thread = NULL;
	}
	if (m_pstr_IP != NULL)
	{
		delete m_pstr_IP;
		m_pstr_IP = NULL;
	}
	if (m_pwstr_IP != NULL)
	{
		delete m_pwstr_IP;
		m_pwstr_IP = NULL;
	}
	if (m_p_SDCL != NULL)
	{
		delete m_p_SDCL;
		m_p_SDCL = NULL;
	}
	if (NULL != m_p_lipddn)
	{
		delete m_p_lipddn;
		m_p_lipddn = NULL;
	}
	if (NULL != m_h_Cancel_Event)
	{
		b_r = CloseHandle(m_h_Cancel_Event);
		m_h_Cancel_Event = NULL;
	}
}

void CSola_Auto_ID_DLL::Set_Pointer_To_Owner(void *p)
{
	m_p_to_owner = p;
}


void CSola_Auto_ID_DLL::Set_Callback_Func_Ptr(void* p)
{
	m_pcf = (func_ptr)p;
}

void CSola_Auto_ID_DLL::Set_Scan_Upd_Callback_Func_Ptr(void* p)
{
	m_sucfp = (scan_upd_callback_func_ptr)p;
}

void CSola_Auto_ID_DLL::Set_Scan_Done_Callback_Func_Ptr(void* p)
{
	m_sdcfp = (scan_done_callback_func_ptr)p;
}

HANDLE CSola_Auto_ID_DLL::Start_Auto_Discovery(void)
{
	BOOL b_r;
	DWORD dw_r;
	if ((NULL != m_h_Cancel_Event) && (NULL == m_h_Work_Thread))
	{
		m_h_Work_Thread = CreateThread(
			NULL,
			0,
			(LPTHREAD_START_ROUTINE)Work_Thread,
			reinterpret_cast<LPVOID>(this),
			CREATE_SUSPENDED,
			&m_dw_Work_Thread_ID);
		if (NULL != m_h_Work_Thread)
		{
			b_r = DuplicateHandle(
				GetCurrentProcess(),
				m_h_Work_Thread,
				GetCurrentProcess(),
				&m_hdup_Work_Thread,
				NULL,
				true,
				DUPLICATE_SAME_ACCESS);
		}
		if ((NULL != m_h_Work_Thread) &&(NULL != m_hdup_Work_Thread))
		{
			dw_r = ResumeThread(m_h_Work_Thread);
		}
	}
	return m_h_Work_Thread;
}

DWORD WINAPI CSola_Auto_ID_DLL::Work_Thread(LPARAM l_p)
{
	BOOL b_Cancel;
	char ch_IP[100];
	int i_r;
	int ret;
	PVOID p_v;
	modbus_param_t mb_param;
	uint16_t *tab_rp_registers;
	uint8_t ui8_addr;
	std::string str_COM_Port;
	std::wstring wstr_COM_Port;
	std::wstring *p_wstr_upd;
	int i_COM_Number;
	DWORD dw_rc;
	DWORD dw_wr;
	SOLADEVICECOORDS sdc;
	CSola_Auto_ID_DLL *p_this;
	std::list<wstring>::iterator lip_it;
	p_this = reinterpret_cast<CSola_Auto_ID_DLL*>(l_p);
	dw_rc = ERROR_SUCCESS;
	b_Cancel = false;
	p_wstr_upd = reinterpret_cast<std::wstring*>(new std::wstring);
	tab_rp_registers = (uint16_t*)new uint16_t[1];
	if (NULL == p_this->Get_IP_List()) /* If no IP address provided, scan serial COM ports */
	{
		for (i_COM_Number = 1; !b_Cancel && (i_COM_Number <= i_Max_COM_Port); i_COM_Number++) /* Cycle through COM ports 1 to i_Max_COM_Port */
		{
			str_COM_Port.assign(str_COM_Prefix+std::to_string(((LONGLONG)i_COM_Number)));
			wstr_COM_Port.assign(wstr_COM_Prefix+std::to_wstring(((LONGLONG)i_COM_Number)));
			if (NULL != p_this->Get_Scan_Upd_Callback_Func_Ptr())
			{
				p_wstr_upd->assign(_T("Scanning ")+wstr_COM_Port);
				p_this->Get_Scan_Upd_Callback_Func_Ptr()(p_this->m_p_to_owner,p_wstr_upd);
			}
			modbus_init_rtu(&mb_param,str_COM_Port.c_str(),38400,"none",8,1);
			if ((ret = modbus_connect(&mb_param)) != 0)
			{
				if (NULL != p_this->Get_Callback_Func_Ptr())
				{
					p_this->Get_Callback_Func_Ptr()(p_this->m_p_to_owner,(DWORD)ret);
				}
				b_Cancel = true;
			}
/* For each available COM port, cycle through prescribed Modbus addresses
   scanning for connected Sola devices */
			if (NULL != mb_param.h_COM)
			{
				p_v = SecureZeroMemory((PVOID)tab_rp_registers,sizeof(uint16_t));
				for(ui8_addr = p_this->Get_Low_Address(); !b_Cancel && (ui8_addr <= p_this->Get_High_Address()); ui8_addr++)
				{
					if (NULL != p_this->Get_Scan_Upd_Callback_Func_Ptr())
					{
						p_wstr_upd->assign(_T("Scanning ")+wstr_COM_Port+_T(" address ")+to_wstring((LONGLONG)ui8_addr));
						p_this->Get_Scan_Upd_Callback_Func_Ptr()(p_this->m_p_to_owner,p_wstr_upd);
					}
					i_r = read_input_registers(
						&mb_param,
						ui8_addr,
						PRODUCT_TYPE_REGISTER_ADDR,
						1,
						tab_rp_registers);
					if ((NULL != p_this->Get_SDC_List()) && (1 == i_r))
					{
						if ((ui16_Residential_Sola_Type_Code == tab_rp_registers[0]) ||
							(ui16_Commercial_Sola_Type_Code == tab_rp_registers[0]))
						{
							sdc.ct = p_this->Get_Conn_Type();
							sdc.Interface_Name = wstr_COM_Port;
							sdc.ui8_addr = ui8_addr;
							p_this->Get_SDC_List()->push_back(sdc); /* Found a Sola on wstr_COM_Port at ui8_addr */
						}
					}
					dw_wr = WaitForSingleObject(p_this->m_h_Cancel_Event,500);
					b_Cancel = (WAIT_FAILED == dw_wr);
					b_Cancel = (!b_Cancel && (WAIT_OBJECT_0 == dw_wr)) ? true : false;
					b_Cancel = (!b_Cancel && (WAIT_TIMEOUT == dw_wr)) ? false : b_Cancel;
#if _DEBUG
					if (b_Cancel)
					{
						b_Cancel = b_Cancel;
					}
#endif
				}
				modbus_close(&mb_param);
			}
		}
	}
	if (NULL != p_this->Get_IP_List()) /* If IP address provided, scan via Modbus TCP gateway */
	{
		if (!p_this->Get_IP_List()->empty())
		{
			for (lip_it = p_this->Get_IP_List()->begin();!b_Cancel && (lip_it != p_this->Get_IP_List()->end()); lip_it++)
			{

				if (NULL != p_this->Get_Scan_Upd_Callback_Func_Ptr())
				{
					p_wstr_upd->assign(_T("Connecting please standby.."));
					p_this->Get_Scan_Upd_Callback_Func_Ptr()(p_this->m_p_to_owner,p_wstr_upd);
				}
				p_v = SecureZeroMemory((PVOID)ch_IP,sizeof(ch_IP));
				i_r = WideCharToMultiByte(
					CP_ACP,
					WC_COMPOSITECHECK,
					lip_it->c_str(),
					-1,
					ch_IP,
					sizeof(ch_IP),
					NULL,
					NULL);
				modbus_init_tcp(&mb_param,ch_IP,p_this->Get_Server_Port());
				modbus_set_debug(&mb_param,TRUE);
				if ((i_r = modbus_connect(&mb_param)) != 0)
				{
					if (NULL != p_this->Get_Callback_Func_Ptr())
					{
						p_this->Get_Callback_Func_Ptr()(p_this->m_p_to_owner,(DWORD)i_r);
					}
					b_Cancel = true;
				}
				p_v = SecureZeroMemory((PVOID)tab_rp_registers,sizeof(uint16_t));
				for(ui8_addr = p_this->Get_Low_Address(); !b_Cancel && (ui8_addr <= p_this->Get_High_Address()); ui8_addr++)
				{
					if (NULL != p_this->Get_Scan_Upd_Callback_Func_Ptr())
					{
						p_wstr_upd->assign(_T("Scanning "));
						p_wstr_upd->append(lip_it->c_str());
						p_wstr_upd->append(_T(" address ")+to_wstring((LONGLONG)ui8_addr));
						p_this->Get_Scan_Upd_Callback_Func_Ptr()(p_this->m_p_to_owner,p_wstr_upd);
					}
					i_r = read_input_registers(
						&mb_param,
						ui8_addr,
						PRODUCT_TYPE_REGISTER_ADDR,
						1,
						tab_rp_registers);
					if ((NULL != p_this->Get_SDC_List()) && (1 == i_r))
					{
						if ((ui16_Residential_Sola_Type_Code == tab_rp_registers[0]) ||
							(ui16_Commercial_Sola_Type_Code == tab_rp_registers[0]))
						{
							sdc.ct = p_this->Get_Conn_Type();
							sdc.Interface_Name = *lip_it;
							sdc.ui8_addr = ui8_addr;
							p_this->Get_SDC_List()->push_back(sdc); /* Found a Sola on wstr_COM_Port at ui8_addr */
						}
					}
					dw_wr = WaitForSingleObject(p_this->m_h_Cancel_Event,500);
					b_Cancel = (WAIT_FAILED == dw_wr);
					b_Cancel = (!b_Cancel && (WAIT_OBJECT_0 == dw_wr)) ? true : false;
					b_Cancel = (!b_Cancel && (WAIT_TIMEOUT == dw_wr)) ? false : b_Cancel;
				}
				modbus_close(&mb_param);
			}
		}
	}
	if (NULL != p_this->Get_Scan_Done_Callback_Func_Ptr())
	{
		p_this->Get_Scan_Done_Callback_Func_Ptr()(p_this->m_p_to_owner);
	}
#if _DEBUG
	b_Cancel = b_Cancel;
#endif
	delete[] tab_rp_registers;
	delete p_wstr_upd;
	return dw_rc;
}

void* CSola_Auto_ID_DLL::deleteSDCListPtr(void)
{
	if (!(NULL == m_p_SDCL))
	{
		delete m_p_SDCL;
		m_p_SDCL = NULL;
	}
	return (void*)m_p_SDCL;
}

Ctrash81_Modeless_Dlg_DLL::Ctrash81_Modeless_Dlg_DLL()
{
	this->m_szDlgQuitEvent = NULL;
	this->m_szDlgRdyEvent = NULL;
	this->m_hDlgQuitEvent = NULL;
	this->m_hwndOwner = NULL;
	this->m_hwndDlg = NULL;
	this->m_hDlgThread = NULL;
	this->m_hdupDlgThread = NULL;
	this->m_hInst = (HINSTANCE) NULL;
	this->m_szTitle = NULL;
	this->m_dwDlgThreadID = NULL;
	this->m_hDlgRdyEvent = NULL;
	m_p_callback_func = NULL;
	m_h_Dlg_Glbl = NULL;
	m_p_upd_callback_func = Upd_Callback_Func;
	this->m_p_lipddn = NULL;
	m_p_sid = NULL;
	m_i_Port = STD_MODBUS_TCP_PORT;
	m_ct = CSola_Auto_ID_DLL::conn_type::RTU;
	m_hw_caller = NULL;
	m_p_Dlg_Item_Tmpl = NULL;
	m_p_Dlg_Tmpl = NULL;
	m_pf_scan_done_handler = NULL;
}

Ctrash81_Modeless_Dlg_DLL::Ctrash81_Modeless_Dlg_DLL(HWND hOwner,HINSTANCE hInst,TCHAR* szTitle) : m_hwndOwner(hOwner),
	m_hInst(hInst),m_szTitle(szTitle)
{
	BOOL bSuccess = true;
	this->m_szDlgQuitEvent = _T("DlgQuitEvent");
	this->m_szDlgRdyEvent = _T("DlgRdyEvent");
	this->m_hDlgQuitEvent = NULL;
	this->m_hDlgRdyEvent = NULL;
	this->m_hwndDlg = NULL;
	this->m_hDlgThread = NULL;
	this->m_hdupDlgThread = NULL;
	this->m_dwDlgThreadID = 0L;
	m_p_callback_func = NULL;
	m_h_Dlg_Glbl = NULL;
	m_p_upd_callback_func = Upd_Callback_Func;
	this->m_p_lipddn = NULL;
	m_p_sid = NULL;
	m_i_Port = STD_MODBUS_TCP_PORT;
	m_ct = CSola_Auto_ID_DLL::conn_type::RTU;
	m_hw_caller = NULL;
	m_p_Dlg_Item_Tmpl = NULL;
	m_p_Dlg_Tmpl = NULL;
	m_pf_scan_done_handler = NULL;
}

Ctrash81_Modeless_Dlg_DLL::~Ctrash81_Modeless_Dlg_DLL()
{
	BOOL bResult;
	if ( this->m_hDlgThread )
	{
		Stop();
	}
	if (NULL != this->m_hDlgQuitEvent)
	{
		bResult = ::CloseHandle(m_hDlgQuitEvent);
		m_hDlgQuitEvent = NULL;
	}
	if (NULL != m_h_Dlg_Glbl)
	{
		GlobalFree(m_h_Dlg_Glbl);
	}
	if (NULL != m_p_sid)
	{
		delete m_p_sid;
		m_p_sid = NULL;
	}
}

void Ctrash81_Modeless_Dlg_DLL::Set_Callback(pf_callback p)
{
	this->m_p_callback_func = p;
}

void Ctrash81_Modeless_Dlg_DLL::Upd_Callback_Func(std::wstring *p_str)
{
}

void Ctrash81_Modeless_Dlg_DLL::Set_Scan_Done_Handler(pf_scan_done_handler p_f)
{
	this->m_pf_scan_done_handler = p_f;
}

void Ctrash81_Modeless_Dlg_DLL::Set_TCP_Port(int p)
{
	m_i_Port = p;
}

BOOL Ctrash81_Modeless_Dlg_DLL::Run(LPCONNTYPEPARMS lp_ctp)
{
	BOOL bResult;
	DWORD dwResult;
	int nResult;
	if (NULL == lp_ctp)
	{
		return false;
	}
	if (NULL != lp_ctp)
	{
		m_ct = lp_ctp->ct;
		if ((CSola_Auto_ID_DLL::TCP == m_ct) && (NULL == lp_ctp->p_lipddn))
		{
			return false;
		}
		if ((CSola_Auto_ID_DLL::TCP == m_ct) && (NULL != lp_ctp->p_lipddn))
		{
			try
			{
				m_p_lipddn = reinterpret_cast<std::list<wstring>*>(new std::list<wstring>);
				*m_p_lipddn = *(lp_ctp->p_lipddn);
			}
			catch (std::bad_alloc &ba)
			{
				ReportError(ba.what());
				return false;
			}
		}
	}
	if (NULL == m_hDlgThread)
	{
		m_hDlgQuitEvent = ::CreateEvent(NULL,true,false,m_szDlgQuitEvent);
		bResult = ::ResetEvent(m_hDlgQuitEvent);
		m_hDlgThread = ::CreateThread(NULL,0,DlgThreadProc,(LPVOID)this,CREATE_SUSPENDED,&m_dwDlgThreadID);
		dwResult = ::GetLastError();
		if ( m_hDlgThread == NULL )
		{
			nResult = ::MessageBox(this->m_hwndOwner,_T("Error starting Dlg thread"),m_szTitle,MB_OK);
			ReportError(dwResult);
		}
		else
		{
			bResult = ::DuplicateHandle(::GetCurrentProcess(),m_hDlgThread,::GetCurrentProcess(),&m_hdupDlgThread,DUPLICATE_SAME_ACCESS,true,0);
			dwResult = ::ResumeThread(m_hDlgThread);
		}
	}
	return true;
}

BOOL Ctrash81_Modeless_Dlg_DLL::Stop()
{
	BOOL bResult;
	DWORD dw_wr;
	DWORD dw_rc;
	int nResult;

	if ( this->m_hDlgThread )
	{
		bResult = ::SetEvent(this->m_hDlgQuitEvent);
		if (IsWindow(this->m_hwndDlg))
		{
			bResult = ::PostMessage(m_hwndDlg,WM_APPENDDLGTHREAD,(WPARAM)0,(LPARAM)0);
			dw_wr = WaitForSingleObject(m_hDlgThread,5000);
			dw_rc = GetLastError();
			if (WAIT_TIMEOUT == dw_wr)
			{
				nResult = MessageBox(this->m_hwndOwner,_T("Timed out waiting dlg thread"),this->m_szTitle,MB_OK);
			}
			if (WAIT_TIMEOUT == dw_wr)
			{
				ReportError(dw_rc);
			}
		}
		bResult = ::CloseHandle(m_hDlgThread);
		bResult = ::CloseHandle(m_hdupDlgThread);
		m_hDlgThread = NULL;
		m_hdupDlgThread = NULL;
	}
	if ( m_hDlgQuitEvent )
	{
		bResult = ::CloseHandle(m_hDlgQuitEvent);
		m_hDlgQuitEvent = NULL;
	}
	return true;
}

DWORD WINAPI Ctrash81_Modeless_Dlg_DLL::DlgThreadProc(LPVOID lParam)
{
	MSG message;
	BOOL bResult;

	Ctrash81_Modeless_Dlg_DLL* pnd = (Ctrash81_Modeless_Dlg_DLL*)lParam;
	pnd->m_hwndDlg = NULL;
	
	if (!IsWindow(pnd->m_hwndDlg) && IsWindow(pnd->m_hwndOwner))
	{
		if (NULL != (pnd->m_h_Dlg_Glbl = pnd->Make_Dlg_Template("Click Scan to start")))
		{
			pnd->m_hwndDlg = CreateDialogIndirectParam(pnd->m_hInst,(LPDLGTEMPLATE)pnd->m_h_Dlg_Glbl,pnd->m_hwndOwner,DlgProc,(LPARAM)pnd);
		}
		if (NULL == pnd->m_hwndDlg)
		{
			ReportError(GetLastError());
			return 1;
		}
		bResult = ShowWindow(pnd->m_hwndDlg,SW_SHOW); 

		while ((bResult = GetMessage(&message, NULL, 0, 0)) != 0) 
		{ 
			if (bResult == -1)
			{
	     // Handle the error and possibly exit
				break;
			}
			else if (!IsWindow(pnd->m_hwndDlg) || !IsDialogMessage(pnd->m_hwndDlg,&message)) 
			{ 
				TranslateMessage(&message); 
				DispatchMessage(&message); 
			} 
		}
	}
	return 0;
}

INT_PTR CALLBACK Ctrash81_Modeless_Dlg_DLL::DlgProc(HWND hDlg,UINT message,WPARAM wParam,LPARAM lParam)
{
	BOOL b_r;
	static Ctrash81_Modeless_Dlg_DLL* pnd;
	BOOL bResult;
	int i_r;
	static std::wstring *p_ip;
	static HWND hwnd_mblo;
	static HWND hwnd_mbhi;
	static INITCOMMONCONTROLSEX iccex_icc;
	std::wstring *p_scan_upd;
	uint8_t ui8_mblo;
	uint8_t ui8_mbhi;
	wstring wstr_mblo;
	wstring wstr_mbhi;
	TCHAR sz_mb_addr[4];
	PVOID p_v;
	std::list<wstring>::iterator ipl_it;

	switch (message)
	{
	case WM_INITDIALOG:
		ui8_mblo = 1;
		ui8_mbhi = 1;
		iccex_icc.dwSize = sizeof(INITCOMMONCONTROLSEX);
		iccex_icc.dwICC = ICC_STANDARD_CLASSES|ICC_INTERNET_CLASSES;
		InitCommonControlsEx(&iccex_icc);
		pnd = (Ctrash81_Modeless_Dlg_DLL*)lParam;
		pnd->m_p_sid = NULL;
		hwnd_mblo = CreateWindow(
			WC_EDIT, 
			L"", 
			WS_CHILD|WS_OVERLAPPED|WS_VISIBLE|WS_BORDER|ES_NUMBER|ES_RIGHT,
			10,
			85,
			70, 
			20,  
			hDlg, 
			NULL, 
			pnd->m_hInst, 
			NULL);
		hwnd_mbhi = CreateWindow(
			WC_EDIT, 
			L"", 
			WS_CHILD|WS_OVERLAPPED|WS_VISIBLE|WS_BORDER|ES_NUMBER|ES_RIGHT,
			10,
			115,
			70, 
			20,  
			hDlg, 
			NULL, 
			pnd->m_hInst, 
			NULL);
		wstr_mblo.assign(to_wstring((ULONGLONG)ui8_mblo));
		wstr_mbhi.assign(to_wstring((ULONGLONG)ui8_mbhi));
		b_r = SetWindowText(hwnd_mblo,wstr_mblo.c_str());
		b_r = SetWindowText(hwnd_mbhi,wstr_mbhi.c_str());
		return (INT_PTR)TRUE;
	case WM_APPAIDCALLBACK:
		i_r = MessageBox(pnd->m_hwndDlg,_T("Error callback from auto discovery"),pnd->m_szTitle,MB_OK);
		ReportError((DWORD)lParam);
		return (INT_PTR)TRUE;
	case WM_APPAIDSCANUPDCALLBACK:
		p_scan_upd = reinterpret_cast<std::wstring*>(lParam);
		if (NULL != p_scan_upd)
		{
			b_r = SetWindowText(GetDlgItem(hDlg,ID_TEXT),p_scan_upd->c_str());
		}
		return (INT_PTR)TRUE;
	case WM_APPAIDSCANDONECALLBACK:
		b_r = SetWindowText(GetDlgItem(hDlg,ID_TEXT),_T("Scan complete!"));
		if ((NULL != pnd->m_pf_scan_done_handler) && (NULL != pnd->m_p_sid))
		{
			pnd->m_pf_scan_done_handler(pnd);
		}
		if ((NULL == pnd->m_pf_scan_done_handler) && (NULL != pnd->m_p_sid))
		{
			delete pnd->m_p_sid;
			pnd->m_p_sid = NULL;
		}
		return (INT_PTR)TRUE;
	case WM_APPENDDLGTHREAD:
		bResult = ::DestroyWindow(hDlg);
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == ID_INIT)
		{
			p_v = SecureZeroMemory((PVOID)sz_mb_addr,sizeof(sz_mb_addr));
			i_r = GetWindowText(hwnd_mblo,sz_mb_addr,sizeof(sz_mb_addr)/sizeof(TCHAR));
			ui8_mblo = (uint8_t)std::stoi(wstring(sz_mb_addr));
			p_v = SecureZeroMemory((PVOID)sz_mb_addr,sizeof(sz_mb_addr));
			i_r = GetWindowText(hwnd_mbhi,sz_mb_addr,sizeof(sz_mb_addr)/sizeof(TCHAR));
			ui8_mbhi = (uint8_t)std::stoi(wstring(sz_mb_addr));
			try
			{
				if (CSola_Auto_ID_DLL::RTU == pnd->m_ct)
				{
					pnd->m_p_sid = (CSola_Auto_ID_DLL*)new CSola_Auto_ID_DLL(ui8_mblo,ui8_mbhi);
				}
				if (CSola_Auto_ID_DLL::TCP == pnd->m_ct)
				{
					if (NULL == pnd->m_p_lipddn)
					{
						i_r = MessageBox(hDlg,_T("No gateway IP address(es) provided"),pnd->m_szTitle,MB_OK);
						return (INT_PTR)TRUE;
					}
					if (pnd->m_p_lipddn->empty())
					{
						i_r = MessageBox(hDlg,_T("No gateway IP address(es) provided"),pnd->m_szTitle,MB_OK);
						return (INT_PTR)TRUE;
					}
					ipl_it = pnd->m_p_lipddn->begin();
					pnd->m_p_sid = (CSola_Auto_ID_DLL*)new CSola_Auto_ID_DLL(ui8_mblo,ui8_mbhi,pnd->m_p_lipddn,pnd->Get_TCP_Port());
				}
			}
			catch(std::bad_alloc &ba)
			{
				ReportError(ba.what());
			}
			if (NULL != pnd->m_p_sid)
			{
				pnd->m_p_sid->Set_Pointer_To_Owner((void*)pnd);
				pnd->m_p_sid->Set_Callback_Func_Ptr(Auto_ID_Callback);
				pnd->m_p_sid->Set_Scan_Upd_Callback_Func_Ptr(Auto_ID_Scan_Upd_Callback);
				pnd->m_p_sid->Set_Scan_Done_Callback_Func_Ptr(Auto_ID_Scan_Done_Callback);
				pnd->m_p_sid->Start_Auto_Discovery();
			}
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			if (NULL != pnd->m_p_sid)
			{
				delete pnd->m_p_sid;
				pnd->m_p_sid = NULL;
			}
			if (pnd->Get_Callback())
			{
				pnd->Get_Callback()((void*)pnd);
			}
			else if (IsWindow(hDlg))
			{
				bResult = PostMessage(hDlg,WM_APPENDDLGTHREAD,(WPARAM)0,(LPARAM)0);
			}
			else
			{
				bResult = DestroyWindow(hDlg);
			}
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDOK)
		{
			return (INT_PTR)TRUE;
		}
		break;
	case WM_DESTROY:
		if (NULL != pnd->m_p_sid)
		{
			delete pnd->m_p_sid;
			pnd->m_p_sid = NULL;
		}
		::PostQuitMessage(0);
		return (INT_PTR)TRUE;
	}
	return (INT_PTR)FALSE;
}

HGLOBAL Ctrash81_Modeless_Dlg_DLL::Make_Dlg_Template(LPSTR lpszMessage)
{
    HGLOBAL hgbl;
    LPDLGTEMPLATE lpdt;
    LPDLGITEMTEMPLATE lpdit;
    LPWORD lpw;
    LPWSTR lpwsz;
    int nchar;

    hgbl = GlobalAlloc(GMEM_ZEROINIT, 1024);
    if (!hgbl)
        return NULL;
 
    lpdt = (LPDLGTEMPLATE)GlobalLock(hgbl);
 
    // Define a dialog box.
 
    lpdt->style = WS_POPUP | WS_BORDER | WS_SYSMENU | DS_MODALFRAME | WS_CAPTION;
    lpdt->cdit = 3;         // Number of controls
    lpdt->x  = 10;
	lpdt->y  = 10;
    lpdt->cx = 150;
	lpdt->cy = 100;

    lpw = (LPWORD)(lpdt + 1);
    *lpw++ = 0;             // No menu
    *lpw++ = 0;             // Predefined dialog box class (by default)

    lpwsz = (LPWSTR)lpw;
/*    nchar = 1 + MultiByteToWideChar(CP_ACP,0,"My Dialog",-1,lpwsz,50);*/
    nchar = 1 + MultiByteToWideChar(CP_ACP,0,"Auto ID",-1,lpwsz,50);
    lpw += nchar;

    //-----------------------
    // Define an OK button.
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 10;
	lpdit->y  = 70;
    lpdit->cx = 80;
	lpdit->cy = 20;
    lpdit->id = IDCANCEL;       // Cancel button identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080;        // Button class

    lpwsz = (LPWSTR)lpw;
    nchar = 1 + MultiByteToWideChar(CP_ACP,0,"Cancel",-1,lpwsz,50);
    lpw += nchar;
    *lpw++ = 0;             // No creation data

    //-----------------------
    // Define Init button.
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 100;
	lpdit->y  = 10;
    lpdit->cx = 40;
	lpdit->cy = 20;
    lpdit->id = ID_INIT;    // Init button identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0080;        // Button class atom

    lpwsz = (LPWSTR)lpw;
/*    nchar = 1 + MultiByteToWideChar(CP_ACP,0,"Help",-1,lpwsz,50);*/
    nchar = 1 + MultiByteToWideChar(CP_ACP,0,"Scan",-1,lpwsz,50);
    lpw += nchar;
    *lpw++ = 0;             // No creation data

    //-----------------------
    // Define a static text control.
    //-----------------------
    lpw = lpwAlign(lpw);    // Align DLGITEMTEMPLATE on DWORD boundary
    lpdit = (LPDLGITEMTEMPLATE)lpw;
    lpdit->x  = 10;
	lpdit->y  = 10;
    lpdit->cx = 60;
	lpdit->cy = 30;
    lpdit->id = ID_TEXT;    // Text identifier
    lpdit->style = WS_CHILD | WS_VISIBLE | SS_LEFT;

    lpw = (LPWORD)(lpdit + 1);
    *lpw++ = 0xFFFF;
    *lpw++ = 0x0082;        // Static class

    for (lpwsz = (LPWSTR)lpw; *lpwsz++ = (WCHAR)*lpszMessage++;);
    lpw = (LPWORD)lpwsz;
    *lpw++ = 0;             // No creation data

    GlobalUnlock(hgbl); 
/*    ret = DialogBoxIndirect(hinst, 
                           (LPDLGTEMPLATE)hgbl, 
                           hwndOwner, 
                           (DLGPROC)DialogProc); 
    GlobalFree(hgbl); */
    return hgbl; 
}

LPWORD Ctrash81_Modeless_Dlg_DLL::lpwAlign(LPWORD lpIn)
{
    ULONG ul;

    ul = (ULONG)lpIn;
    ul ++;
    ul >>=1;
    ul <<=1;
    return (LPWORD)ul;
}

void Ctrash81_Modeless_Dlg_DLL::Scan_Done_Handler(void)
{
	LRESULT l_r = SendMessage(this->m_hwndDlg,WM_APPAIDSCANDONECALLBACK,(WPARAM)0,(LPARAM)0);
}

void Ctrash81_Modeless_Dlg_DLL::Auto_ID_Callback(void *pto,DWORD dw_code)
{
	Ctrash81_Modeless_Dlg_DLL *p_this;
	LRESULT l_r;
	p_this = reinterpret_cast<Ctrash81_Modeless_Dlg_DLL*>(pto);
	l_r = SendMessage(p_this->m_hwndDlg,WM_APPAIDCALLBACK,(WPARAM)0,(LPARAM)dw_code);
}

void Ctrash81_Modeless_Dlg_DLL::Auto_ID_Scan_Upd_Callback(void *pto,std::wstring *p)
{
	Ctrash81_Modeless_Dlg_DLL *p_this;
	LRESULT l_r;
	p_this = reinterpret_cast<Ctrash81_Modeless_Dlg_DLL*>(pto);
	l_r = SendMessage(p_this->m_hwndDlg,WM_APPAIDSCANUPDCALLBACK,(WPARAM)0,(LPARAM)p);
}

void Ctrash81_Modeless_Dlg_DLL::Auto_ID_Scan_Done_Callback(void *pto)
{
	Ctrash81_Modeless_Dlg_DLL *p_this;
	p_this = reinterpret_cast<Ctrash81_Modeless_Dlg_DLL*>(pto);
	p_this->Scan_Done_Handler();
}

void Ctrash81_Modeless_Dlg_DLL::Set_Caller_HWND(HWND &hw)
{
	m_hw_caller = hw;
}