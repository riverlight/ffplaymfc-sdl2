#ifndef FLVMAKER_H
#define FLVMAKER_H

#include <fstream>

#include ".\include\mediaCnvt\converter.h"

using namespace Cnvt;

typedef unsigned long long uint64_t;
typedef long long int64_t;

class CFlvMaker
{
public:
	CFlvMaker(char *szName);
	virtual ~CFlvMaker();

	int WriteAVCDecoderConfigurationRecord(unsigned char *data, int size);
	int WriteAudioSpecificConfig(unsigned char *data, int size);
	
	/* datatype: 0 is video, 1 is audio */
	int Write(unsigned char *data, int size, int datatype, int64_t dts);

private:
	int WriteVideo(unsigned char *data, int size, int64_t dts);
	int WriteAudio(unsigned char *data, int size, int64_t dts);

private:
	static void WriteU64(uint64_t & x, int length, int value)
	{
		uint64_t mask = 0xFFFFFFFFFFFFFFFF >> (64 - length);
		x = (x << length) | ((uint64_t)value & mask);
	}

private:
	std::ofstream *_pH264File;
	std::ofstream *_pAACFile;

	CConverter *_pCnvt;
	unsigned char *_pVideoBuffer;
	unsigned char *_pAudioBuffer;
	int64_t n64VideoBaseTimeStamp;

	// aac
	int _aacProfile;
	int _sampleRateIndex;
	int _channelConfig;
};

#endif // FLVMAKER_H
