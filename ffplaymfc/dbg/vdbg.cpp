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
