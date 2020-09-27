/**********************************************
*	文件功能：主函数，程序起点
*	编写平台：WIN10 + VS2013
*	作    者：罗敏
*	编写时间：2020-08-02
**/
#include "Main.h"
#include "MainDlg.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	/* 创建一个对话框对象 */
	CMainDlg dlg;

	/* 进入消息循环 */
	dlg.DoModal(IDD_DIALOG_MAIN);

	/* 退出程序 */
	return 0;
}


