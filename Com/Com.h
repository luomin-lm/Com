#pragma once
#include "Main.h"

DWORD WINAPI ReadThread(LPVOID);

class CCom
{
public:
	CCom();
	~CCom();
	/*****************************************
	*	���ܣ��򿪴���
	*	�����б�
	*		com:	������
	*		bps:	������
	*		data	����λ
	*		stop	ֹͣλ
	*		fParity	У��λ������Ϊ0
	*	����ֵ��
	*		TRUE	�򿪳ɹ�
	*		FALSE	��ʧ��
	*/
	BOOL Open(TCPCH com);
	/*****************************************
	*	���ܣ����ô������������������С
	*	�����б�
	*		inSize:		���뻺������С
	*		putSize:	�����������С���������ݻ���Ĵ�С
	*	����ֵ��
	*		TRUE	���óɹ�
	*		FALSE	����ʧ��
	*/
	BOOL SetInPutSize(TCUQ(int) inSize, TCUQ(int) putSize);
	/*****************************************
	*	���ܣ����ô��ڽ��ճ�ʱ
	*	�����б�
	*		msTimes��	���ճ�ʱ����msΪ��λ
	*	����ֵ��
	*		TRUE	���óɹ�
	*		FALSE	����ʧ��
	*/
	BOOL SetTimeout(TCUQ(int) msTimes = 100);
	/*****************************************
	*	���ܣ����ô��ڽ��ղ���
	*	�����б�
	*		bps��		������
	*		data��		����λ
	*		stop��		ֹͣλ(ֻ��Ϊ0��δ�������)
	*		fParity��	У��λ(ֻ��Ϊ0��δ�������)
	*	����ֵ��
	*		TRUE	���óɹ�
	*		FALSE	����ʧ��
	*/
	BOOL SetDCB(TCUQ(int) bps = 9600, TCUQ(int) data = 8, TCUQ(int) stop = 0,
		TCUQ(int) fParity = 0);
	/*****************************************
	*	���ܣ�		�رմ���
	*	�����б�	��
	*	����ֵ��	��
	*/
	void Close();
	/*****************************************
	*	���ܣ���򿪵Ĵ��ڷ�����Ϣ
	*	�����б�
	*		buf��		�ַ���
	*		len��		�ַ����ĳ���
	*		timeout:	��ʱʱ�䣬��λΪms
	*	����ֵ��
	*		TRUE	���óɹ�
	*		FALSE	����ʧ��
	*/
	BOOL Write(TCPCH buf, TCUQ(int) len, TCUQ(int) timeout = 1000);
	/*****************************************
	*	���ܣ���ȡ���ڵ���Ϣ(��ȫ�첽�����汾)
	*	�����б�
	*		buf��		�ַ������׵�ַ
	*		len��		�ַ������ڴ��С
	*	����ֵ����
	*/
	void ReadStr(char * buf, UINT size);
	/*****************************************
	*	���ܣ����ô��ڶ�ȡ�¼������Ĵ�����Ϣ
	*	�����б�
	*		hwnd��		���ھ��
	*		msg��		������Ϣ
	*	����ֵ��
	*		TRUE	���óɹ�
	*		FALSE	����ʧ��
	*/
	BOOL SetComEvent(TCQ(HWND) hwnd, TCQ(UINT) msg);
	/* �������ݶ�ȡ���߳� */
	friend DWORD WINAPI ReadThread(LPVOID);
	/*****************************************
	*	���ܣ���ȡ
	*	�����б���
	*	����ֵ����ȡ���Ĵ�����Ϣ
	*/
	TCPCH GetReadData();

private:
	HANDLE m_com;	/* ���ھ�� */
	HANDLE m_thread;	/* �߳̾�� */

	HWND m_hwnd;	/* ���ڶ��¼��������ھ�� */
	UINT m_msg;		/* ���ڶ��¼�����������Ϣ */

	char* m_pBuff;	/* ��ȡ���ݵ�ָ���ַ */
	UINT m_size;	/* �ڴ��С */
};

