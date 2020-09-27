#include "MainDlg.h"
#include "Com.h"
#include <stdio.h>
#include <Dbt.h>

#define WM_COM_EVENT_READ	(WM_USER + 1)
#define ID_STATE_BAR		0x4001
#define MIN_WIDTH			680
#define MIN_HEIGHT			380

/* 系统窗口兼容性宏设置 */
#define _OS_WIN7_

#ifdef _OS_WIN10_
#define EDIT_XPOS			300 // 编辑框X坐标
#define EDIT_BOTTOM_POS		132 // 编辑框底部距离窗口底部
#endif

#ifdef _OS_WIN7_
#define EDIT_XPOS			340 // 编辑框X坐标
#define EDIT_BOTTOM_POS		160 // 编辑框底部距离窗口底部
#endif

CMainDlg::CMainDlg()
{
	m_pCom = new CCom;
	m_pEdit = NULL;
	m_autoEnter = TRUE;
	m_flg = FALSE;
	m_pStateBar = NULL;
	m_HaveComSet = TRUE;
	m_font = NULL;
	m_bInit = FALSE;
	m_isTimerSend = FALSE;
	m_no = 0;

	m_iinfo.ReadInfo();

	m_pMenuGroup.SetIds(2, ID_COM_OPEN, ID_COM_CLOSE);
}
CMainDlg::~CMainDlg()
{
	if (m_flg == TRUE)
	{
		m_pCom->Close();
	}
	delete m_pCom;

	if (m_pEdit)
	{
		delete m_pEdit;
	}
	if (m_pStateBar)
	{
		delete m_pStateBar;
	}
	if (m_font != nullptr)
	{
		LOGFONT lf;
		FILE* fp;
		GetObject(m_font, sizeof(LOGFONT), &lf);
		fp = fopen("./data.font", "wb");
		fwrite(&lf, 1, sizeof(LOGFONT), fp);
		fclose(fp);

		DeleteObject(m_font);
	}

	m_iinfo.SaveInfo();
}

void CMainDlg::GetUsingCom()
{
	lmw::CComboBox *pComBox = new lmw::CComboBox(GetSafeHwnd(), IDC_COMBO_COM);
	lmw::CReg reg;
	char buf[256], name[256];
	int comnum = 0;
	HKEY hKey;
	DWORD valueNum, vn, dwSize; // 串口数量, 

	pComBox->DeleteAllItems();

	if (TRUE == reg.Open(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM"))
	{
		hKey = reg.GetHKey();

		/* 获取串口数量 */
		RegQueryInfoKey(hKey, NULL, NULL, NULL,
			NULL, NULL, NULL, &valueNum, NULL, NULL, NULL, NULL);

		/* 添加串口 */
		for (uint i = 0; i < valueNum; i++)
		{
			/* 设置缓冲区长度 */
			vn = dwSize = 256;
			/* 枚举键与值 */
			RegEnumValue(hKey, i, name, &vn, NULL, NULL, (PUCHAR)buf, &dwSize);

			/* 没有错误，添加 */
			if (0 != dwSize)
			{
				pComBox->AddStr(buf);
				comnum++;
			}
		}

		reg.Close();
	}

	comnum = comnum == 1 ? 0 : 1;
	pComBox->Select(comnum);
	
	delete pComBox;
}

void CMainDlg::OnCommand(TCUQ(int) id, TCUQ(int) msg)
{
	switch (id)
	{
	case IDC_BUTTON_NEWCOM:
		/* 获取可用串口 */
		GetUsingCom();
		break;
	case ID_COM_OPEN:
	case ID_COM_CLOSE:
	case IDC_BUTTON_OPENCOM:
		OnButtonOpenCom();
		break;
	case IDC_BUTTON_SEND:
		OnButtonSend();
		break;
	case IDC_COMBO_STOP:
	case IDC_COMBO_DATA:
	case IDC_COMBO_BPS:
		OnChangeBps();
		break;
	case ID_MENU_EXIT:
		this->EndDialog(1);
		break;
	case ID_MENU_AUTOENTER:
		m_autoEnter = !m_autoEnter;
		SetMenuCheck(ID_MENU_AUTOENTER, m_autoEnter);
		break;
	case IDC_BUTTON_RESET:
		SetHaveComSet(!m_HaveComSet);
		break;
	case ID_FONT_SET:
		ChangeFont();
		break;
	case ID_FILE_SAVE: // 将数据保存到文件中
		SaveTo();
		break;
	case ID_FILE_CLEAR: // 将数据清楚
		m_pEdit->SetTitle("");
		break;
	case ID_OP_TIMERS:
		SetTimerSend();
		break;
	}
}

void CMainDlg::SetTimerSend()
{
	if (FALSE == m_isTimerSend)
	{
		SetTimer(ID_OP_TIMERS, 1000);
		SetMenuCheck(ID_OP_TIMERS, TRUE);
		m_isTimerSend = TRUE;
	}
	else
	{
		KillTimer(ID_OP_TIMERS);
		SetMenuCheck(ID_OP_TIMERS, FALSE);
		m_isTimerSend = FALSE;
	}
}

void CMainDlg::SaveTo()
{
	lmw::CStr str = "C:\\data.txt";
	lmw::CStr path;

	// explorer /select,C:\data.txt
	if (FALSE != lmw::CComDlg::SaveFileDlg(GetSafeHwnd(), str))
	{
		lmw::CFile file;

		file.Open(str.c_str(), TRUE);
		path = GetCtrlText(0x3001);
		file.Write(path.c_str(), path.size());
		file.Close();
	}

	path.Format("explorer /select,%s", str.c_str());
	system(path.c_str());
}

void CMainDlg::OnChangeBps()
{
	lmw::CStr temp = GetCtrlText(IDC_COMBO_STOP);

	int stop = (int)(temp.ToDouble(temp.c_str()) * 2 - 2); // 停止位
	int bps = GetCtrlInt(IDC_COMBO_BPS); // 波特率
	int data = GetCtrlInt(IDC_COMBO_DATA); // 数据位
	//int jy = GetCtrlInt(IDC_COMBO_XY); // 校验位
	lmw::CComboBox pCbb(GetSafeHwnd(), IDC_COMBO_XY);
	int jy = pCbb.GetSelect();

	/* 正确打开状态 */
	if (m_flg == TRUE)
	{
		if (FALSE == m_pCom->SetDCB(bps, data, stop, jy))
		{
			MsgBox("设置串口参数失败", "提示", MB_OK | MB_ICONERROR);
		}
	}

	m_iinfo.m_bps = bps;
	m_iinfo.m_bData = data;
	m_iinfo.m_bStop = stop;
	m_iinfo.m_xy = jy;

	UpdateStateBar();
}

void CMainDlg::InitStateBar()
{
	lmw::CStr temp, str;

	m_pStateBar = new lmw::CStateBar(GetSafeHwnd(), ID_STATE_BAR);

	str = GetCtrlText(IDC_COMBO_COM);
	temp.Format("串口: %s", str.c_str());
	m_pStateBar->AddPart(temp.c_str(), 140);

	str = GetCtrlText(IDC_COMBO_BPS);
	temp.Format("波特率: %s", str.c_str());
	m_pStateBar->AddPart(temp.c_str(), 120);

	str = GetCtrlText(IDC_COMBO_STOP);
	temp.Format("停止位: %s", str.c_str());
	m_pStateBar->AddPart(temp.c_str(), 100);

	str = GetCtrlText(IDC_COMBO_DATA);
	temp.Format("数据位: %s", str.c_str());
	m_pStateBar->AddPart(temp.c_str(), 100);

	str = GetCtrlText(IDC_COMBO_XY);
	temp.Format("效验位: %s", str.c_str());
	m_pStateBar->AddPart(temp.c_str(), 100);

	m_pStateBar->Paint();
}

void CMainDlg::OnInit()
{
	GetUsingCom();

	InitComboBox();

	SetIcon(IDI_ICON1);

	m_pEdit = new lmw::CTextEdit(GetSafeHwnd(), 0x3001);
	// m_pEdit->SetPos(300, 8);
	// m_pEdit->SetSize(365, 188);
	m_pEdit->AddStyle(WS_VSCROLL | WS_HSCROLL);

	// 只读
	PostMessage(m_pEdit->GetSafeHwnd(), EM_SETREADONLY, 1, 0);

	InitStateBar();

	// 使能按键
	SetCtrlEnable(IDC_BUTTON_SEND, FALSE);

	// 隐藏串口的设置
	SetHaveComSet(!m_HaveComSet);

	// 初始化串口
	InitFont();

	AddStyle(WS_THICKFRAME);
	
	m_bInit = TRUE;

	SetSize(m_iinfo.m_width, m_iinfo.m_height);

	SetCenterPos();

	SetComInOutEvent();

	SetMenuEnable(ID_OP_TIMERS, FALSE);
}

void CMainDlg::InitFont()
{
	LOGFONT lf;
	FILE* fp;
	lmw::CCtrl ctrl(GetSafeHwnd(), IDC_EDIT_DATA);

	fp = fopen("./data.font", "rb+");
	if (fp == NULL)
	{
		return;
	}
	int len = fread(&lf, 1, sizeof(LOGFONT), fp);
	if (len != 60)
	{
		fclose(fp);
		return;
	}
	fclose(fp);
	m_font = CreateFontIndirect(&lf);

	m_pEdit->SetFont(m_font);
	ctrl.SetFont(m_font);
}

void CMainDlg::UpdateStateBar()
{
	lmw::CStr temp, str;

	str = GetCtrlText(IDC_COMBO_COM);
	temp.Format("串口: %s", str.c_str());
	m_pStateBar->SetItemText(0, temp.c_str());

	str = GetCtrlText(IDC_COMBO_BPS);
	temp.Format("波特率: %s", str.c_str());
	m_pStateBar->SetItemText(1, temp.c_str());

	str = GetCtrlText(IDC_COMBO_STOP);
	temp.Format("停止位: %s", str.c_str());
	m_pStateBar->SetItemText(2, temp.c_str());

	str = GetCtrlText(IDC_COMBO_DATA);
	temp.Format("数据位: %s", str.c_str());
	m_pStateBar->SetItemText(3, temp.c_str());

	str = GetCtrlText(IDC_COMBO_XY);
	temp.Format("效验位: %s", str.c_str());
	m_pStateBar->SetItemText(4, temp.c_str());

	m_pStateBar->Paint();
}

void CMainDlg::OnSize(TCUQ(int) width, TCUQ(int) height)
{
	m_pStateBar->OnSize();

	if (m_bInit == TRUE)
	{
		if (m_HaveComSet == TRUE)
		{
			m_pEdit->SetPos(EDIT_XPOS, 6);
			m_pEdit->SetSize(width - 5 - EDIT_XPOS, height - EDIT_BOTTOM_POS);
		}
		else
		{
			m_pEdit->SetPos(10, 6);
			m_pEdit->SetSize(width - 20, height - EDIT_BOTTOM_POS);
		}

		SetCtrlPos(IDC_EDIT_DATA, 10, height - EDIT_BOTTOM_POS + 12);
		SetCtrlSize(IDC_EDIT_DATA, width - 200, 88);
		SetCtrlPos(IDC_BUTTON_SEND, width - 180, height - EDIT_BOTTOM_POS + 12);
		SetCtrlPos(IDC_BUTTON_RESET, width - 90, height - EDIT_BOTTOM_POS + 12);
	
		m_iinfo.m_width = width + 20;
		m_iinfo.m_height = height + 60;
	}
}

void CMainDlg::InitComboBox()
{
	lmw::CComboBox * pComboBox;
	int bpss[] = { 1200 , 2400, 4800, 9600, 19200, 28800, 38400, 57600, 115200, 128000, 256000, 460800 };
	int i, len, select = 3;
	lmw::CStr temp;

	pComboBox = new lmw::CComboBox(GetSafeHwnd(), IDC_COMBO_BPS);

	len = sizeof(bpss) / sizeof(bpss[0]);
	for (i = 0; i < len; i++)
	{
		temp.Format("%d", bpss[i]);
		pComboBox->AddStr(temp.c_str());

		if (m_iinfo.m_bps == bpss[i])
		{
			select = i;
		}
	}
	pComboBox->Select(select);
	delete pComboBox;

	pComboBox = new lmw::CComboBox(GetSafeHwnd(), IDC_COMBO_STOP);
	pComboBox->AddStr("1");
	pComboBox->AddStr("1.5");
	pComboBox->AddStr("2");
	pComboBox->Select(m_iinfo.m_bStop);
	delete pComboBox;

	pComboBox = new lmw::CComboBox(GetSafeHwnd(), IDC_COMBO_DATA);
	pComboBox->AddStr("5");
	pComboBox->AddStr("6");
	pComboBox->AddStr("7");
	pComboBox->AddStr("8");

	if (m_iinfo.m_bData > 4 && m_iinfo.m_bData < 9)
	{
		pComboBox->Select(m_iinfo.m_bData - 5);
	}
	else
	{
		pComboBox->Select(3);
	}
	delete pComboBox;

	pComboBox = new lmw::CComboBox(GetSafeHwnd(), IDC_COMBO_XY);
	pComboBox->AddStr("不校验");
	pComboBox->AddStr("奇校验");
	pComboBox->AddStr("偶校验");
	pComboBox->AddStr("校验位始终为1");
	pComboBox->AddStr("校验位始终为0");
	pComboBox->Select(m_iinfo.m_xy);
	delete pComboBox;
}

void CMainDlg::OnButtonOpenCom()
{
	lmw::CHwnd hwnd;
	lmw::CStr temp;

	char com[256] = { 0 };
	/* 串口号 */
	hwnd.SetHwnd(GetSafeHwnd(), IDC_COMBO_COM); // 串口号
	hwnd.GetTitle(com, 256);

	temp = GetCtrlText(IDC_COMBO_STOP); // 停止位
	int stop = (int)(temp.ToDouble(temp.c_str()) * 2) - 2;
	int bps = GetCtrlInt(IDC_COMBO_BPS); // 波特率
	int data = GetCtrlInt(IDC_COMBO_DATA); // 数据位
	lmw::CComboBox pCbb(GetSafeHwnd(), IDC_COMBO_XY);
	int jy = pCbb.GetSelect();// 校验位

	if (FALSE == m_flg && TRUE == m_pCom->Open(com))
	{
		if (FALSE == m_pCom->SetInPutSize(2048, 2048))
		{
			MsgBox("设置缓冲区大小失败", "提示", MB_OK | MB_ICONERROR);
			//DWORD err = GetLastError();
			return;
		}
		if (FALSE == m_pCom->SetTimeout())
		{
			MsgBox("设置接收超时失败", "提示", MB_OK | MB_ICONERROR);
			//DWORD err = GetLastError();
			return;
		}
		if (FALSE == m_pCom->SetDCB(bps, data, stop, jy))
		{
			MsgBox("设置串口参数失败", "提示", MB_OK | MB_ICONERROR);
			//DWORD err = GetLastError();
			return;
		}
		if (FALSE == m_pCom->SetComEvent(GetSafeHwnd(), WM_COM_EVENT_READ))
		{
			MsgBox("设置串口事件失败", "提示", MB_OK | MB_ICONERROR);
		}
		else
		{
			m_flg = TRUE;
			SetCtrlEnable(IDC_BUTTON_SEND, TRUE);
			SetCtrlText(IDC_BUTTON_OPENCOM, "关  闭  串  口");
			m_comStr = com;
			SetMenuEnable(ID_OP_TIMERS, TRUE);
		}
	}
	else
	{
		m_pCom->Close();
		m_flg = FALSE;
		SetCtrlEnable(IDC_BUTTON_SEND, FALSE);
		SetCtrlText(IDC_BUTTON_OPENCOM, "打  开  串  口");
		SetMenuEnable(ID_OP_TIMERS, FALSE);
		KillTimer(ID_OP_TIMERS);
	}

	m_pMenuGroup.CheckId(GetSafeHwnd(), m_flg ? ID_COM_OPEN : ID_COM_CLOSE);
}

void CMainDlg::OnButtonSend()
{
	if (TRUE == m_flg)
	{
		lmw::CStr str = GetCtrlText(IDC_EDIT_DATA);

		if (m_autoEnter == TRUE)
		{
			str += "\r\n";
		}

		m_pCom->Write(str.c_str(), str.size());
	}
	else
	{
		MsgBox("请先打开串口", "提示", MB_OK | MB_ICONERROR);
	}
}

BOOL CMainDlg::OnOtherMsgHandle(TCQ(HWND) hwnd, TCQ(int) msg, 
	TCQ(WPARAM) wParam, TCQ(LPARAM) lParam)
{
	static int no = 0;

	/* 处理串口的消息 */
	if (m_flg == TRUE && WM_COM_EVENT_READ == msg)
	{
		lmw::CStr str = GetCtrlText(0x3001);
		lmw::CStr temp;
		temp.Format("%05d、", m_no++);
		str += temp;
		str += m_pCom->GetReadData();

		m_pEdit->SetTitle(str.c_str());
		m_pEdit->SendMsg(WM_VSCROLL, SB_BOTTOM, 0);
	}
	/* 窗口最小大小 */
	else if (WM_GETMINMAXINFO == msg)
	{
		PMINMAXINFO pMinMaxInfo = (PMINMAXINFO)lParam;

		pMinMaxInfo->ptMinTrackSize.x = MIN_WIDTH;
		pMinMaxInfo->ptMinTrackSize.y = MIN_HEIGHT;
	}
	// 热插拔事件处理
	else if (WM_DEVICECHANGE == msg)
	{
		if (wParam == DBT_DEVICEARRIVAL) // 新硬件插入
		{
			if (DBT_DEVTYP_PORT == ((DEV_BROADCAST_HDR*)lParam)->dbch_devicetype)
			{
				lmw::CComboBox pComboBox(GetSafeHwnd(), IDC_COMBO_COM);
				pComboBox.AddStr(((PDEV_BROADCAST_PORT_A)lParam)->dbcp_name);
				pComboBox.Select(1);
			}
		}
		if (wParam == DBT_DEVICEREMOVECOMPLETE) // 硬件拔出
		{
			if (DBT_DEVTYP_PORT == ((DEV_BROADCAST_HDR*)lParam)->dbch_devicetype)
			{
				lmw::CComboBox pComboBox(GetSafeHwnd(), IDC_COMBO_COM);
				int count = pComboBox.GetCount();
				lmw::CStr str;

				// 如果串口打开了，就关闭
				if (m_flg == TRUE)
				{
					if (m_comStr == ((PDEV_BROADCAST_PORT_A)lParam)->dbcp_name)
					{
						OnButtonOpenCom();
					}
				}

				// 删除指定项
				for (int i = 0; i < count; i++)
				{
					str = pComboBox.GetItemText(i);
					if (str == ((PDEV_BROADCAST_PORT_A)lParam)->dbcp_name)
					{
						pComboBox.DeleteItem(NULL, i);
						if (count == 2)
						{
							pComboBox.Select(0);
						}
						else
						{
							pComboBox.Select(1);
						}
						break;
					}
				}
			}
		}
	}

	return CDlgApp::OnOtherMsgHandle(hwnd, msg, wParam, lParam);
}

void CMainDlg::SetHaveComSet(BOOL HaveComSet)
{
	RECT rc = GetClientRect();

	if (TRUE == HaveComSet)
	{
		SetCtrlShow(IDC_STATIC1, SW_SHOW);
		SetCtrlShow(IDC_STATIC2, SW_SHOW);
		SetCtrlShow(IDC_STATIC3, SW_SHOW);
		SetCtrlShow(IDC_STATIC4, SW_SHOW);
		SetCtrlShow(IDC_STATIC5, SW_SHOW);
		SetCtrlShow(IDC_STATIC_GROUP, SW_SHOW);
		SetCtrlShow(IDC_COMBO_COM, SW_SHOW);
		SetCtrlShow(IDC_COMBO_BPS, SW_SHOW);
		SetCtrlShow(IDC_COMBO_XY, SW_SHOW);
		SetCtrlShow(IDC_COMBO_DATA, SW_SHOW);
		SetCtrlShow(IDC_COMBO_STOP, SW_SHOW);
		SetCtrlShow(IDC_BUTTON_NEWCOM, SW_SHOW);
		SetCtrlShow(IDC_BUTTON_OPENCOM, SW_SHOW);

		m_HaveComSet = TRUE;

		SetCtrlText(IDC_BUTTON_RESET, "隐  藏\n\n修 改 参 数");

		m_pEdit->SetPos(EDIT_XPOS, 6);
		m_pEdit->SetSize(rc.right - 5 - EDIT_XPOS, rc.bottom - EDIT_BOTTOM_POS);
	}
	else
	{
		SetCtrlShow(IDC_STATIC1, SW_HIDE);
		SetCtrlShow(IDC_STATIC2, SW_HIDE);
		SetCtrlShow(IDC_STATIC3, SW_HIDE);
		SetCtrlShow(IDC_STATIC4, SW_HIDE);
		SetCtrlShow(IDC_STATIC5, SW_HIDE);
		SetCtrlShow(IDC_STATIC_GROUP, SW_HIDE);

		SetCtrlShow(IDC_COMBO_COM, SW_HIDE);
		SetCtrlShow(IDC_COMBO_BPS, SW_HIDE);
		SetCtrlShow(IDC_COMBO_XY, SW_HIDE);
		SetCtrlShow(IDC_COMBO_DATA, SW_HIDE);
		SetCtrlShow(IDC_COMBO_STOP, SW_HIDE);
		SetCtrlShow(IDC_BUTTON_NEWCOM, SW_HIDE);
		SetCtrlShow(IDC_BUTTON_OPENCOM, SW_HIDE);

		m_HaveComSet = FALSE;

		SetCtrlText(IDC_BUTTON_RESET, "显  示\n\n修 改 参 数");

		m_pEdit->SetPos(10, 6);
		m_pEdit->SetSize(rc.right - 20, rc.bottom - EDIT_BOTTOM_POS);
	}
}

// 修改字体
void CMainDlg::ChangeFont()
{
	HFONT font = NULL;
	lmw::CCtrl ctrl(GetSafeHwnd(), IDC_EDIT_DATA);

	if (TRUE == lmw::CComDlg::FontSetDlg(GetSafeHwnd(), &font))
	{
		m_pEdit->SetFont(font);
		ctrl.SetFont(font);

		if (m_font != nullptr)
		{
			DeleteObject(m_font);
		}
		m_font = font;
	}
}

// 注册热插拔事件
void CMainDlg::SetComInOutEvent()
{
	DEV_BROADCAST_DEVICEINTERFACE dbdi;
	HDEVNOTIFY  hDevNotify; //硬件插拔句柄

	memset(&dbdi, 0, sizeof(dbdi));
	dbdi.dbcc_size = sizeof(dbdi);
	dbdi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	hDevNotify = RegisterDeviceNotification(GetSafeHwnd(), &dbdi, 
		DEVICE_NOTIFY_WINDOW_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
}

void CMainDlg::OnTimer(TCUQ(int) id)
{
	if (ID_OP_TIMERS == id)
	{
		OnButtonSend();
	}
}


