#include "stdafx.h"

#include "flvmaker.h"

#define NONE_BASETIME (-10000)

static const unsigned int h264StartCode = 0x01000000;

CFlvMaker::CFlvMaker(char *szName)
{
	_pH264File = new std::ofstream("d:\\workroom\\testroom\\ffplay.264", std::ios::binary | std::ios::out);
	_pAACFile = new std::ofstream("d:\\workroom\\testroom\\ffplay.aac", std::ios::binary | std::ios::out);

	_pCnvt = new CConverter();
	_pCnvt->Open(szName, 1, 1);

	_pVideoBuffer = new unsigned char[2000000];
	_pAudioBuffer = new unsigned char[10000];

	n64VideoBaseTimeStamp = NONE_BASETIME;
}

CFlvMaker::~CFlvMaker()
{
	delete _pAudioBuffer;
	_pAudioBuffer = NULL;
	delete _pVideoBuffer;
	_pVideoBuffer = NULL;

	_pAACFile->close();
	_pH264File->close();

	_pCnvt->Close();
	delete _pCnvt;
	_pCnvt = NULL;
}

int CFlvMaker::Write(unsigned char *data, int size, int datatype, int64_t dts)
{
	TRACE("datatype : %d\n", datatype);
	if (datatype == 0)
		WriteVideo(data, size, dts);
	if (datatype == 1)
		WriteAudio(data, size, dts);

	return 1;
}

int CFlvMaker::WriteAVCDecoderConfigurationRecord(unsigned char *data, int size)
{
	int spsSize = (data[6]<<8) + data[7];
	_pH264File->write((char *)&h264StartCode, 4);
	_pH264File->write((char*)data + 8, spsSize);

	int ppsSize = (data[9 + spsSize] << 8) + data[10 + spsSize];
	_pH264File->write((char *)&h264StartCode, 4);
	_pH264File->write((char*)data + 9 + spsSize + 2, ppsSize);

	// for flv
	unsigned char sps_pps[1000];
	memset(sps_pps, 0, 1000);
	memcpy(sps_pps, &h264StartCode, 4);
	memcpy(sps_pps + 4, data + 8, spsSize);
	_pCnvt->ConvertH264((char *)sps_pps, 4 + spsSize, 0);

	memcpy(sps_pps, &h264StartCode, 4);
	memcpy(sps_pps + 4, data + 9 + spsSize + 2, ppsSize);
	_pCnvt->ConvertH264((char *)sps_pps, 4 + ppsSize, 0);

	return 1;
}

int CFlvMaker::WriteAudioSpecificConfig(unsigned char *data, int size)
{
	_aacProfile = ((data[0] & 0xf8) >> 3) - 1;
	_sampleRateIndex = ((data[0] & 0x07) << 1) | (data[1] >> 7);
	_channelConfig = (data[1] >> 3) & 0x0f;

	return 1;
}

int CFlvMaker::WriteVideo(unsigned char *data, int size, int64_t dts)
{
	int count = 0;
	unsigned int nTimeStamp;
	if (n64VideoBaseTimeStamp == NONE_BASETIME)
		n64VideoBaseTimeStamp = dts;

	nTimeStamp = (dts - n64VideoBaseTimeStamp) * 1000 / 30000;
	while (1)
	{
		int nFrameSize = (data[count+0]<<24) + (data[count+1]<<16) + (data[count+2]<<8) + data[count+3];
		if (nFrameSize + 4 + count > size)
		{
			TRACE("Fotal error in function []\n", __FUNCTION__);
			break;
		}

		_pH264File->write((char *)&h264StartCode, 4);
		_pH264File->write((char*)data + count + 4, nFrameSize);

		memcpy(_pVideoBuffer, &h264StartCode, 4);
		memcpy(_pVideoBuffer + 4, data + count + 4, nFrameSize);
		_pCnvt->ConvertH264((char *)_pVideoBuffer, 4 + nFrameSize, nTimeStamp);

		count += (nFrameSize + 4);
		if (count >= size)
			break;
	}

	return 1;
}

int CFlvMaker::WriteAudio(unsigned char *data, int size, int64_t dts)
{
	unsigned int nTimeStamp;
	uint64_t bits = 0;
	int dataSize = size;

	if (dts < 0)
		nTimeStamp = 0;
	else
		nTimeStamp = (dts * 1000) / 44100;

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

	_pAACFile->write((char *)p64 + 1, 7);
	_pAACFile->write((char *)data, size);

	memcpy(_pAudioBuffer, (char *)p64 + 1, 7);
	memcpy(_pAudioBuffer + 7, data, size);
	_pCnvt->ConvertAAC((char *)_pAudioBuffer, 7 + size, nTimeStamp);

	return 1;
}
