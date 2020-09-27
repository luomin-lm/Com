#pragma once

#define DEFAULT_INI_FILE	"./com.ini"	// 默认配置文件
#define DEFAULT_INI_PROJ	"COM" // 配置选择项
#define DEFAULT_INI_BPS		"BPS" // 波特率
#define DEFAULT_INI_BDT		"DATAB" // 数据位
#define DEFAULT_INI_STOP	"STOP" // 停止位
#define DEFAULT_INI_XY		"XY" // 效验位

class CIniInfo
{
public:
	void ReadInfo();
	void SaveInfo();

public:
	int m_bps; // 波特率
	int m_bData; // 数据位
	int m_bStop; // 停止位
	int m_xy; // 效验位
	int m_width; // 宽
	int m_height; // 高
};

