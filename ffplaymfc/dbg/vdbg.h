#ifndef VDBG_H
#define VDBG_H

#ifdef __cplusplus
extern "C"
{
#endif

void dumpToFile(void *p, int len, char *szName);
void printToFile(char *format, ...);
void vdbg_close();

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // VDBG_H
