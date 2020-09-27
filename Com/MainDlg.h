/**********************************************
*	文件功能：CMainDlg对话框类，消息与逻辑处理
*	编写平台：WIN10 + VS2013
*	作    者：罗敏
*	编写时间：2020-08-02
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
	/* 初始化控件 */
	virtual void OnInit();
	/* 按键与菜单消息处理 */
	virtual void OnCommand(TCUQ(int) id, TCUQ(int) msg);
	/* 获取可用的串口，不支持COM9以上 */
	void GetUsingCom();
	/* 打开一个串口 */
	void OnButtonOpenCom();
	/* 初始化串口设置 */
	void InitComboBox();
	/* 初始化状态栏  */
	void InitStateBar();
	/* 向串口发生数据  */
	void OnButtonSend();
	/* 处理从串口接收的数据  */
	virtual BOOL OnOtherMsgHandle(TCQ(HWND) hwnd, TCQ(int) msg, TCQ(WPARAM) wParam, TCQ(LPARAM) lParam);
	/* 修改串口设置 */
	void OnChangeBps();
	/* 窗口大小改变 */
	virtual void OnSize(TCUQ(int) width, TCUQ(int) height);
	/* 更新状态栏的数据 */
	void UpdateStateBar();
	/* 有修改参数的形式 */
	void SetHaveComSet(BOOL HaveComSet);
	/* 修改字体 */
	void ChangeFont();
	/* 初始化字体 */
	void InitFont();
	/* 设置热插拔事件 */
	void SetComInOutEvent();
	/* 保存数据 */
	void SaveTo();
	/* 设置定时发送 */
	void SetTimerSend();
	/* 定时器事件 */
	virtual void OnTimer(TCUQ(int) id);

private:
	CCom* m_pCom; // 串口对象
	lmw::CTextEdit * m_pEdit; // 编辑框对象
	BOOL m_flg, m_HaveComSet; // 是否打开串口，是否显示串口设置
	BOOL m_autoEnter; // 是否自动换行
	BOOL m_bInit; // 是否初始化完成
	CIniInfo m_iinfo; // 配置文件信息
	lmw::CStateBar *m_pStateBar; // 状态栏
	lmw::CMenuGroup m_pMenuGroup; // 菜单选项组
	HFONT m_font; // 字体信息
	lmw::CStr m_comStr; // 打开的串口
	BOOL m_isTimerSend; // 定时发送
	int m_no; //消息编号
};

