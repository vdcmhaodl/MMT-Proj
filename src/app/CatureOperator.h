#pragma once

#include <windows.h> 
#include <iostream>
#include <fstream>
#include <mfapi.h> 
#include <mfidl.h>
#include <mfreadwrite.h> 
#include <mferror.h> 
#include <shellscalingapi.h>

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

HRESULT WebcamCapture(LPCWSTR filename);
bool TakeScreenShot(const std::string &filename);

HRESULT GetWebcamMediaSource(IMFMediaSource **ppSource);
HRESULT InitializeAndFormatSourceReader(IMFMediaSource *pSource, IMFSourceReader** pReader, GUID *pVideoFormat);
HRESULT InitializeAndFormatSinkWriter(LPCWSTR filename, GUID pVideoFormat, IMFSinkWriter **ppWriter, DWORD* pStreamIndex);
HRESULT StartWebcamCapture(IMFSourceReader *pReader, IMFSinkWriter *pWriter, DWORD streamIndex);


