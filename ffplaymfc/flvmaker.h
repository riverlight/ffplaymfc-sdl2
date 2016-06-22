#ifndef FLVMAKER_H
#define FLVMAKER_H

#include <fstream>

class CFlvMaker
{
public:
	CFlvMaker(char *szName);
	virtual ~CFlvMaker();

	int WriteAVCDecoderConfigurationRecord(unsigned char *data, int size);

	/* datatype: 0 is video, 1 is audio */
	int Write(unsigned char *data, int size, int datatype);

private:
	int WriteVideo(unsigned char *data, int size);

private:
	std::ofstream *_pFlvFile;

};

#endif // FLVMAKER_H
