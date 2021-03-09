#ifndef _WAVPLAY_H
#define _WAVPLAY_H

typedef struct _WAV_MERGE_DATA
{
    char    FileName[40];   ///< original wav file name
    UINT32  uiAddr;         ///< wav data address in PStore section
    UINT32  uiSize;         ///< wav data size
} WAV_MERGE_DATA;

extern ER WavPlay_TransWavToAAC(char *pFolderPath);
extern ER WavPlay_WriteWavData(void);
extern ER WavPlay_PlayWavData(char *pName);
extern ER WavPlay_PlayAACData(char *pName);
extern ER WavPlay_PlayWavDataFromFile(char *pFileName);
extern ER WavPlay_PlayAACDataFromFile(char *pFileName);

#endif
