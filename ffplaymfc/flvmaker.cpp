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
	TRACE("datatype : %d\n", datatype);
//	if (datatype == 0)
//		WriteVideo(data, size);
	if (datatype == 1)
		WriteAudio(data, size);

	return 1;
}

int CFlvMaker::WriteAVCDecoderConfigurationRecord(unsigned char *data, int size)
{
#if 0
	int spsSize = (data[6]<<8) + data[7];
	_pFlvFile->write((char *)&h264StartCode, 4);
	_pFlvFile->write((char*)data + 8, spsSize);

	int ppsSize = (data[9 + spsSize] << 8) + data[10 + spsSize];
	_pFlvFile->write((char *)&h264StartCode, 4);
	_pFlvFile->write((char*)data + 9 + spsSize + 2, ppsSize);
#endif

	return 1;
}

int CFlvMaker::WriteAudioSpecificConfig(unsigned char *data, int size)
{
	_aacProfile = ((data[0] & 0xf8) >> 3) - 1;
	_sampleRateIndex = ((data[0] & 0x07) << 1) | (data[1] >> 7);
	_channelConfig = (data[1] >> 3) & 0x0f;

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

int CFlvMaker::WriteAudio(unsigned char *data, int size)
{
	uint64_t bits = 0;
	int dataSize = size;

	WriteU64(bits, 12, 0xFFF);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 2, 0);
	WriteU64(bits, 1, 1);
	WriteU64(bits, 2, _aacProfile);
	WriteU64(bits, 4, _sampleRateIndex);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 3, _channelConfig);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 1, 0);
	WriteU64(bits, 13, 7 + dataSize);
	WriteU64(bits, 11, 0x7FF);
	WriteU64(bits, 2, 0);
	unsigned char p64[8];
	p64[0] = (unsigned char)(bits >> 56);
	p64[1] = (unsigned char)(bits >> 48);
	p64[2] = (unsigned char)(bits >> 40);
	p64[3] = (unsigned char)(bits >> 32);
	p64[4] = (unsigned char)(bits >> 24);
	p64[5] = (unsigned char)(bits >> 16);
	p64[6] = (unsigned char)(bits >> 8);
	p64[7] = (unsigned char)(bits);

	_pFlvFile->write((char *)p64 + 1, 7);
	_pFlvFile->write((char *)data, size);

	TRACE("audio size : %d\n", size);

	return 1;
}
