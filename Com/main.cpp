/**********************************************
*	�ļ����ܣ����������������
*	��дƽ̨��WIN10 + VS2013
*	��    �ߣ�����
*	��дʱ�䣺2020-08-02
**/
#include "Main.h"
#include "MainDlg.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/* ����һ���Ի������ */
	CMainDlg dlg;

	/* ������Ϣѭ�� */
	dlg.DoModal(IDD_DIALOG_MAIN);

	/* �˳����� */
	return 0;
}


