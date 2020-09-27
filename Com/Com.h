#pragma once
#include "Main.h"

DWORD WINAPI ReadThread(LPVOID);

class CCom
{
public:
	CCom();
	~CCom();
	/*****************************************
	*	功能：打开串口
	*	参数列表：
	*		com:	串口名
	*		bps:	波特率
	*		data	数据位
	*		stop	停止位
	*		fParity	校验位，必须为0
	*	返回值：
	*		TRUE	打开成功
	*		FALSE	打开失败
	*/
	BOOL Open(TCPCH com);
	/*****************************************
	*	功能：设置串口输入输出缓冲区大小
	*	参数列表：
	*		inSize:		输入缓冲区大小
	*		putSize:	输出缓冲区大小，接收数据缓冲的大小
	*	返回值：
	*		TRUE	设置成功
	*		FALSE	设置失败
	*/
	BOOL SetInPutSize(TCUQ(int) inSize, TCUQ(int) putSize);
	/*****************************************
	*	功能：设置串口接收超时
	*	参数列表：
	*		msTimes：	接收超时，以ms为单位
	*	返回值：
	*		TRUE	设置成功
	*		FALSE	设置失败
	*/
	BOOL SetTimeout(TCUQ(int) msTimes = 100);
	/*****************************************
	*	功能：设置串口接收参数
	*	参数列表：
	*		bps：		波特率
	*		data：		数据位
	*		stop：		停止位(只能为0，未完成其他)
	*		fParity：	校验位(只能为0，未完成其他)
	*	返回值：
	*		TRUE	设置成功
	*		FALSE	设置失败
	*/
	BOOL SetDCB(TCUQ(int) bps = 9600, TCUQ(int) data = 8, TCUQ(int) stop = 0,
		TCUQ(int) fParity = 0);
	/*****************************************
	*	功能：		关闭串口
	*	参数列表：	无
	*	返回值：	无
	*/
	void Close();
	/*****************************************
	*	功能：向打开的串口发送消息
	*	参数列表：
	*		buf：		字符串
	*		len：		字符串的长度
	*		timeout:	超时时间，单位为ms
	*	返回值：
	*		TRUE	设置成功
	*		FALSE	设置失败
	*/
	BOOL Write(TCPCH buf, TCUQ(int) len, TCUQ(int) timeout = 1000);
	/*****************************************
	*	功能：读取串口的消息(完全异步阻塞版本)
	*	参数列表：
	*		buf：		字符数组首地址
	*		len：		字符数组内存大小
	*	返回值：无
	*/
	void ReadStr(char * buf, UINT size);
	/*****************************************
	*	功能：设置串口读取事件关联的窗口消息
	*	参数列表：
	*		hwnd：		窗口句柄
	*		msg：		窗口消息
	*	返回值：
	*		TRUE	设置成功
	*		FALSE	设置失败
	*/
	BOOL SetComEvent(TCQ(HWND) hwnd, TCQ(UINT) msg);
	/* 监听数据读取的线程 */
	friend DWORD WINAPI ReadThread(LPVOID);
	/*****************************************
	*	功能：获取
	*	参数列表：无
	*	返回值：读取到的串口消息
	*/
	TCPCH GetReadData();

private:
	HANDLE m_com;	/* 串口句柄 */
	HANDLE m_thread;	/* 线程句柄 */

	HWND m_hwnd;	/* 串口读事件关联串口句柄 */
	UINT m_msg;		/* 串口读事件关联串口消息 */

	char* m_pBuff;	/* 读取数据的指针地址 */
	UINT m_size;	/* 内存大小 */
};

