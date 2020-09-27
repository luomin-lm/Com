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
	/* �ȹر���ǰ�Ĵ��� */
	Close();

	/**
	ͨ������CreateFile�򿪴���,���һ���������Ǵ��ڵ��߼��˿���,���á�COMX����ʾ��,����X��1~N������,
	��X����10ʱ,��ʱ������޷��򿪵�����,���߼��˿����ĳ�"\\\\.\\COMX"���ɽ��.
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
		//printf("���ô��ڻ�����ʧ��...%d\n", GetLastError());
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

	to.ReadIntervalTimeout = msTimes; // ����ʱ��� 10ms
	if (m_com && FALSE == SetCommTimeouts(m_com, &to))
	{
		//printf("���ô��ڽ��ճ�ʱʧ��...%d\n", GetLastError());
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

	/* �������� */
	SetCommMask(m_com, 0);
	//��������ն˾����ź�
	EscapeCommFunction(m_com, CLRDTR);
	//����ͨ����Դ����������뻺�����ַ�����ֹ��ͨ����Դ�Ϲ���Ķ���д�ٲ���
	PurgeComm(m_com, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	CloseHandle(m_com);
	m_com = NULL;
}

BOOL CCom::SetDCB(TCUQ(int) bps, TCUQ(int) data, TCUQ(int) stop,
	TCUQ(int) fParity)
{
	/* ����DCB���� */
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

