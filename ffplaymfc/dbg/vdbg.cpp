#include "stdafx.h"

#include <fstream>

#include "vdbg.h"

void dumpToFile(void *p, int len, char *szName)
{
	static int count = 0;
	char name[100];
	sprintf_s(name, "%s-%d.bin", szName, count);

	std::ofstream outfile(name, std::ios::binary | std::ios::out);
	outfile.write((char *)p, len);
	outfile.close();
	count++;
}

std::ofstream g_outfile;
void printToFile(char *format, ...)
{
	char sprint_buf[1024];
	va_list ap;
	va_start(ap, format);
	vsprintf_s(sprint_buf, format, ap);
	va_end(ap);
	//TRACE(sprint_buf);

	if (g_outfile.is_open() == false)
		g_outfile.open("d:\\workroom\\testroom\\fileInfo.txt", std::ios::out);
	g_outfile.write(sprint_buf, strnlen_s(sprint_buf, 1024));
}

void vdbg_close()
{
	if (g_outfile.is_open())
		g_outfile.close();
}
