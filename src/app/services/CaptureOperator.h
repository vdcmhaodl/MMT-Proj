#pragma once

#include <windows.h> 
#include <iostream>
#include <fstream>
#include <mfapi.h> 
#include <mfidl.h>
#include <mfreadwrite.h> 
#include <mferror.h> 
#include <shellscalingapi.h>
#include "serviceCommand.h"

extern UINT32 VIDEO_WIDTH;
extern UINT32 VIDEO_HEIGHT;
extern UINT32 VIDEO_FPS;
extern UINT64 VIDEO_FRAME_DURATION;
extern UINT32 VIDEO_BIT_RATE;
extern LONGLONG VIDEO_MAX_DURATION;

template <class T> 
void SafeRelease(T **ppT) {
    if (*ppT) {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

namespace Services {
    bool webcamRecord(const std::string &filename, LONGLONG videoDuration = 30);
    bool screenShot(const std::string &filename);

    std::vector<std::string> webcamRecord(Command command);
    std::vector<std::string> screenShot(Command command);
}

HRESULT GetWebcamMediaSource(IMFMediaSource **ppSource);
HRESULT InitializeAndFormatSourceReader(IMFMediaSource *pSource, IMFSourceReader** pReader, GUID *pVideoFormat);
HRESULT InitializeAndFormatSinkWriter(LPCWSTR filename, GUID pVideoFormat, IMFSinkWriter **ppWriter, DWORD* pStreamIndex);
HRESULT StartRecord(IMFSourceReader *pReader, IMFSinkWriter *pWriter, DWORD streamIndex, LONGLONG videoDuration);
HRESULT WebcamRecord(LPCWSTR filename, LONGLONG videoDuration);

