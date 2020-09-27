/**********************************************
*	�ļ����ܣ�CMainDlg�Ի����࣬��Ϣ���߼�����
*	��дƽ̨��WIN10 + VS2013
*	��    �ߣ�����
*	��дʱ�䣺2020-08-02
**/
#pragma once
#include "Main.h"
#include "IniInfo.h"

class CCom;
class CMainDlg : public lmw::CDlgApp
{
public:
	CMainDlg();
	~CMainDlg();

public:
	/* ��ʼ���ؼ� */
	virtual void OnInit();
	/* ������˵���Ϣ���� */
	virtual void OnCommand(TCUQ(int) id, TCUQ(int) msg);
	/* ��ȡ���õĴ��ڣ���֧��COM9���� */
	void GetUsingCom();
	/* ��һ������ */
	void OnButtonOpenCom();
	/* ��ʼ���������� */
	void InitComboBox();
	/* ��ʼ��״̬��  */
	void InitStateBar();
	/* �򴮿ڷ�������  */
	void OnButtonSend();
	/* ����Ӵ��ڽ��յ�����  */
	virtual BOOL OnOtherMsgHandle(TCQ(HWND) hwnd, TCQ(int) msg, TCQ(WPARAM) wParam, TCQ(LPARAM) lParam);
	/* �޸Ĵ������� */
	void OnChangeBps();
	/* ���ڴ�С�ı� */
	virtual void OnSize(TCUQ(int) width, TCUQ(int) height);
	/* ����״̬�������� */
	void UpdateStateBar();
	/* ���޸Ĳ�������ʽ */
	void SetHaveComSet(BOOL HaveComSet);
	/* �޸����� */
	void ChangeFont();
	/* ��ʼ������ */
	void InitFont();
	/* �����Ȳ���¼� */
	void SetComInOutEvent();
	/* �������� */
	void SaveTo();
	/* ���ö�ʱ���� */
	void SetTimerSend();
	/* ��ʱ���¼� */
	virtual void OnTimer(TCUQ(int) id);

private:
	CCom* m_pCom; // ���ڶ���
	lmw::CTextEdit * m_pEdit; // �༭�����
	BOOL m_flg, m_HaveComSet; // �Ƿ�򿪴��ڣ��Ƿ���ʾ��������
	BOOL m_autoEnter; // �Ƿ��Զ�����
	BOOL m_bInit; // �Ƿ��ʼ�����
	CIniInfo m_iinfo; // �����ļ���Ϣ
	lmw::CStateBar *m_pStateBar; // ״̬��
	lmw::CMenuGroup m_pMenuGroup; // �˵�ѡ����
	HFONT m_font; // ������Ϣ
	lmw::CStr m_comStr; // �򿪵Ĵ���
	BOOL m_isTimerSend; // ��ʱ����
	int m_no; //��Ϣ���
};

