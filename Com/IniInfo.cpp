#include "IniInfo.h"
#include <Windows.h>
#include <stdio.h>

void CIniInfo::ReadInfo()
{
	m_bps = GetPrivateProfileInt(DEFAULT_INI_PROJ, DEFAULT_INI_BPS, 9600, DEFAULT_INI_FILE);
	m_bData = GetPrivateProfileInt(DEFAULT_INI_PROJ, DEFAULT_INI_BDT, 8, DEFAULT_INI_FILE);
	m_bStop = GetPrivateProfileInt(DEFAULT_INI_PROJ, DEFAULT_INI_STOP, 0, DEFAULT_INI_FILE);
	m_xy = GetPrivateProfileInt(DEFAULT_INI_PROJ, DEFAULT_INI_XY, 0, DEFAULT_INI_FILE);
	m_width = GetPrivateProfileInt(DEFAULT_INI_PROJ, "WIDTH", 680, DEFAULT_INI_FILE);
	m_height = GetPrivateProfileInt(DEFAULT_INI_PROJ, "HEIGHT", 380, DEFAULT_INI_FILE);
}


void CIniInfo::SaveInfo()
{
	char str[256];

	sprintf(str, "%d", m_bps);
	WritePrivateProfileString(DEFAULT_INI_PROJ, DEFAULT_INI_BPS, str, DEFAULT_INI_FILE);

	sprintf(str, "%d", m_bData);
	WritePrivateProfileString(DEFAULT_INI_PROJ, DEFAULT_INI_BDT, str, DEFAULT_INI_FILE);

	sprintf(str, "%d", m_bStop);
	WritePrivateProfileString(DEFAULT_INI_PROJ, DEFAULT_INI_STOP, str, DEFAULT_INI_FILE);

	sprintf(str, "%d", m_xy);
	WritePrivateProfileString(DEFAULT_INI_PROJ, DEFAULT_INI_XY, str, DEFAULT_INI_FILE);

	sprintf(str, "%d", m_width);
	WritePrivateProfileString(DEFAULT_INI_PROJ, "WIDTH", str, DEFAULT_INI_FILE);

	sprintf(str, "%d", m_height);
	WritePrivateProfileString(DEFAULT_INI_PROJ, "HEIGHT", str, DEFAULT_INI_FILE);
}
