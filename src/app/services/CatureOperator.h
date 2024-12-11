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

#pragma comment(lib, "mfplat.lib") 
#pragma comment(lib, "mf.lib") 
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")

template <class T> 
void SafeRelease(T **ppT) {
    if (*ppT) {
        (*ppT)->Release();
        *ppT = NULL;
    }
}

namespace Services {
    class CaptureCommand : public Command {

    };
    bool webcamCapture(const std::string &filename);
    bool screenShot(const std::string &filename);
}
HRESULT GetWebcamMediaSource(IMFMediaSource **ppSource);
HRESULT InitializeAndFormatSourceReader(IMFMediaSource *pSource, IMFSourceReader** pReader, GUID *pVideoFormat);
HRESULT InitializeAndFormatSinkWriter(LPCWSTR filename, GUID pVideoFormat, IMFSinkWriter **ppWriter, DWORD* pStreamIndex);
HRESULT StartRecord(IMFSourceReader *pReader, IMFSinkWriter *pWriter, DWORD streamIndex);
HRESULT WebcamCapture(LPCWSTR filename);

