#pragma once

#define DEFAULT_INI_FILE	"./com.ini"	// Ĭ�������ļ�
#define DEFAULT_INI_PROJ	"COM" // ����ѡ����
#define DEFAULT_INI_BPS		"BPS" // ������
#define DEFAULT_INI_BDT		"DATAB" // ����λ
#define DEFAULT_INI_STOP	"STOP" // ֹͣλ
#define DEFAULT_INI_XY		"XY" // Ч��λ

class CIniInfo
{
public:
	void ReadInfo();
	void SaveInfo();

public:
	int m_bps; // ������
	int m_bData; // ����λ
	int m_bStop; // ֹͣλ
	int m_xy; // Ч��λ
	int m_width; // ��
	int m_height; // ��
};

