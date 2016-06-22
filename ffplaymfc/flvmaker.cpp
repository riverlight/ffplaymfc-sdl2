#include "stdafx.h"

#include "flvmaker.h"

static const unsigned int h264StartCode = 0x01000000;

CFlvMaker::CFlvMaker(char *szName)
{
	_pFlvFile = new std::ofstream(szName, std::ios::binary | std::ios::out);
}

CFlvMaker::~CFlvMaker()
{
	_pFlvFile->close();
}

int CFlvMaker::Write(unsigned char *data, int size, int datatype)
{
	if (datatype == 0)
		WriteVideo(data, size);

	return 1;
}

int CFlvMaker::WriteAVCDecoderConfigurationRecord(unsigned char *data, int size)
{
	int spsSize = (data[6]<<8) + data[7];
	_pFlvFile->write((char *)&h264StartCode, 4);
	_pFlvFile->write((char*)data + 8, spsSize);

	int ppsSize = (data[9 + spsSize] << 8) + data[10 + spsSize];
	_pFlvFile->write((char *)&h264StartCode, 4);
	_pFlvFile->write((char*)data + 9 + spsSize + 2, ppsSize);

	return 1;
}

int CFlvMaker::WriteVideo(unsigned char *data, int size)
{
	int count = 0;
	while (1)
	{
		int nFrameSize = (data[count+0]<<24) + (data[count+1]<<16) + (data[count+2]<<8) + data[count+3];
		_pFlvFile->write((char *)&h264StartCode, 4);
		_pFlvFile->write((char*)data + count + 4, nFrameSize);

		count += (nFrameSize + 4);
		if (count >= size)
			break;
	}
	

	return 1;
}