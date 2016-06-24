#ifndef FLVMAKER_H
#define FLVMAKER_H

#include <fstream>

typedef unsigned long long uint64_t;

class CFlvMaker
{
public:
	CFlvMaker(char *szName);
	virtual ~CFlvMaker();

	int WriteAVCDecoderConfigurationRecord(unsigned char *data, int size);
	int WriteAudioSpecificConfig(unsigned char *data, int size);
	
	/* datatype: 0 is video, 1 is audio */
	int Write(unsigned char *data, int size, int datatype);

private:
	int WriteVideo(unsigned char *data, int size);
	int WriteAudio(unsigned char *data, int size);

private:
	static void WriteU64(uint64_t & x, int length, int value)
	{
		uint64_t mask = 0xFFFFFFFFFFFFFFFF >> (64 - length);
		x = (x << length) | ((uint64_t)value & mask);
	}

private:
	std::ofstream *_pFlvFile;
	std::ofstream *_pH264File;
	std::ofstream *_pAACFile;

	// aac
	int _aacProfile;
	int _sampleRateIndex;
	int _channelConfig;
};

#endif // FLVMAKER_H
