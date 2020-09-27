#include "Com.h"


CCom::CCom()
{
	m_com = NULL;

	m_thread = NULL;

	m_pBuff = NULL;
}

CCom::~CCom()
{
	Close();

	if (NULL != m_thread)
	{
		TerminateThread(m_thread, 0);
		WaitForSingleObject(m_thread, INFINITE);
		CloseHandle(m_thread);
	}

	if (m_pBuff != NULL)
	{
		delete[] m_pBuff;
	}
}

BOOL CCom::Open(TCPCH com)
{
	/* 先关闭以前的串口 */
	Close();

	/**
	通过函数CreateFile打开串口,其第一个参数就是串口的逻辑端口名,是用”COMX”表示的,其中X是1~N的整数,
	当X大于10时,有时会出现无法打开的问题,把逻辑端口名改成"\\\\.\\COMX"即可解决.
	**/
	m_com = CreateFile(com, GENERIC_READ | GENERIC_WRITE, 0,
		NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL, NULL);

	if (m_com == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL CCom::SetInPutSize(TCUQ(int) inSize, TCUQ(int) putSize)
{
	if (m_com && FALSE == SetupComm(m_com, inSize, putSize))
	{
		//printf("设置串口缓冲区失败...%d\n", GetLastError());
		return FALSE;
	}

	if (m_pBuff != NULL)
	{
		delete[] m_pBuff;
	}
	m_pBuff = new char[putSize];
	m_size = putSize;

	return TRUE;
}

BOOL CCom::SetTimeout(TCUQ(int) msTimes)
{
	COMMTIMEOUTS to;
	GetCommTimeouts(m_com, &to);

	to.ReadIntervalTimeout = msTimes; // 读超时间隔 10ms
	if (m_com && FALSE == SetCommTimeouts(m_com, &to))
	{
		//printf("设置串口接收超时失败...%d\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}

void CCom::Close()
{
	if (m_com == NULL)
	{
		return;
	}

	/* 清理数据 */
	SetCommMask(m_com, 0);
	//清除数据终端就绪信号
	EscapeCommFunction(m_com, CLRDTR);
	//丢弃通信资源的输出或输入缓冲区字符并终止在通信资源上挂起的读、写操操作
	PurgeComm(m_com, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	CloseHandle(m_com);
	m_com = NULL;
}

BOOL CCom::SetDCB(TCUQ(int) bps, TCUQ(int) data, TCUQ(int) stop,
	TCUQ(int) fParity)
{
	/* 设置DCB参数 */
	DCB dcb = { 0 };

	dcb.DCBlength = sizeof(DCB);
	if (m_com && FALSE == GetCommState(m_com, &dcb))
	{
		return FALSE;
	}

	dcb.BaudRate = bps;     //  baud rate
	dcb.ByteSize = data;    //  data size, xmit and rcv
	dcb.Parity = fParity;   //  parity bit
	dcb.StopBits = stop;	//	stop size

	EVENPARITY;

	if (FALSE == SetCommState(m_com, &dcb))
	{
		DWORD err = GetLastError();
		if (0 != err)
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOL CCom::Write(TCPCH buf, TCUQ(int) len, TCUQ(int) timeout)
{
	OVERLAPPED over = { 0 };
	BOOL ret = TRUE;

	if (NULL == m_com)
	{
		return FALSE;
	}

	over.hEvent = CreateEvent(
		NULL,   // default security attributes 
		TRUE,   // manual-reset event 
		FALSE,  // not signaled 
		NULL    // no name
		);

	WriteFile(m_com, buf, len, NULL, &over);

	if (WAIT_OBJECT_0 != WaitForSingleObject(over.hEvent, timeout))
	{
		ret = FALSE;
	}

	CloseHandle(over.hEvent);
	return ret;
}

void CCom::ReadStr(char * buf, UINT size)
{
	OVERLAPPED over = { 0 };

	over.hEvent = CreateEvent(
		NULL,   // default security attributes 
		FALSE,   // manual-reset event 
		FALSE,  // not signaled 
		NULL    // no name
		);

	ReadFile(m_com, buf, size, NULL, &over);

	WaitForSingleObject(over.hEvent, INFINITE);
	CloseHandle(over.hEvent);
}

BOOL CCom::SetComEvent(TCQ(HWND) hwnd, TCQ(UINT) msg)
{
	m_hwnd = hwnd;
	m_msg = msg;

	if (m_com && m_pBuff)
	{
		m_thread = CreateThread(NULL, 0, ReadThread, this, 0, NULL);
		if (m_thread == FALSE)
		{
			return FALSE;
		}

		return TRUE;
	}
	return FALSE;
}

DWORD WINAPI ReadThread(LPVOID arg)
{
	CCom* pCom = (CCom*)arg;

	do
	{
		memset(pCom->m_pBuff, 0, pCom->m_size);
		pCom->ReadStr(pCom->m_pBuff, pCom->m_size);
		SendMessage(pCom->m_hwnd, pCom->m_msg, 0, 0);
	}while(1);

	return 0;
}

TCPCH CCom::GetReadData()
{
	return m_pBuff;
}

