#include "CaptureOperator.h"


bool Services::webcamCapture(const std::string &fileName) {
    std::wstring filename (fileName.begin(), fileName.end());
    return SUCCEEDED(WebcamCapture(filename.c_str()));
}

bool Services::screenShot(const std::string &filename) {
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    // Lấy kích thước màn hình
    UINT dpi = 96;
    HMONITOR hMonitor = MonitorFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
    if (hMonitor)
        GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpi, &dpi);

    int screenWidth = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
    int screenHeight = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);

    // Tạo một DC để tương tác với màn hình
    HDC hScreenDC = GetDC(NULL);
    HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

    if (!hMemoryDC)
    {
        std::cerr << "Không thể tạo DC tương thích!" << std::endl;
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // Cấu hình cho bitmap với BITMAPINFO
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = screenWidth;
    bmi.bmiHeader.biHeight = -screenHeight; // Đặt chiều cao âm để lưu ảnh đúng chiều
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Tạo một DIB section
    void *pBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

    if (!hBitmap)
    {
        std::cerr << "Không thể tạo DIB section!" << std::endl;
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // Chọn bitmap vào DC bộ nhớ
    SelectObject(hMemoryDC, hBitmap);

    // Sao chép dữ liệu từ màn hình vào bitmap
    if (!BitBlt(hMemoryDC, 0, 0, screenWidth, screenHeight, hScreenDC, 0, 0, SRCCOPY))
    {
        std::cerr << "Không thể sao chép dữ liệu từ màn hình vào bitmap!" << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // Tạo file để lưu bitmap
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER &bi = bmi.bmiHeader;
    DWORD dwBmpSize = ((screenWidth * bi.biBitCount + 31) / 32) * 4 * screenHeight;
    DWORD dwFileSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwBmpSize;

    fileHeader.bfType = 0x4D42; // "BM"
    fileHeader.bfSize = dwFileSize;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file)
    {
        std::cerr << "Không thể tạo file ảnh!" << std::endl;
        DeleteObject(hBitmap);
        DeleteDC(hMemoryDC);
        ReleaseDC(NULL, hScreenDC);
        return false;
    }

    // Ghi dữ liệu vào file
    file.write(reinterpret_cast<const char *>(&fileHeader), sizeof(fileHeader));
    file.write(reinterpret_cast<const char *>(&bi), sizeof(bi));
    file.write(reinterpret_cast<const char *>(pBits), dwBmpSize);
    file.close();

    // Giải phóng tài nguyên
    DeleteObject(hBitmap);
    DeleteDC(hMemoryDC);
    ReleaseDC(NULL, hScreenDC);

    std::cout << "Chụp màn hình thành công! File lưu tại: " << filename << std::endl;
    return true;
}


HRESULT GetWebcamMediaSource(IMFMediaSource **ppSource) {
    HRESULT hr = S_OK;

    *ppSource = nullptr;
    IMFAttributes* pAttributes = nullptr;
    IMFActivate** ppDevices = nullptr;
    IMFMediaSource* pSource = nullptr;
    UINT32 count = 0;

    hr = MFCreateAttributes(&pAttributes, 1);
    if (FAILED(hr)) {
        std::cerr << "Fail to create Attribute!\n";
        goto done;
    }
    hr = pAttributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
    if (FAILED(hr)) {
        std::cerr << "Fail to set GUID!\n";
        goto done;
    }

    hr = MFEnumDeviceSources(pAttributes, &ppDevices, &count);
    if (FAILED(hr)) {
        std::cerr << "Fail to enum device source!\n";
        goto done;
    }
    if (count < 1) {
        hr = E_FAIL;
        std::cerr << "No device source!\n";
        goto done;
    }
    hr = ppDevices[0]->ActivateObject(IID_PPV_ARGS(&pSource));
    if (FAILED(hr)) {
        std::cerr << "Fail to create media source!\n";
        goto done;
    }
    
    *ppSource = pSource;
    (*ppSource)->AddRef();

done:
    SafeRelease(&pAttributes);
    for (DWORD i = 0; i < count; ++i) {
        SafeRelease(&ppDevices[i]);
    }
    CoTaskMemFree(ppDevices);
    SafeRelease(&pSource);

    return hr;
}

HRESULT InitializeAndFormatSourceReader(IMFMediaSource *pSource, IMFSourceReader **pReader, GUID *pVideoFormat) {
    HRESULT hr = S_OK;

    IMFMediaType* pVideoType = nullptr;
    IMFSourceReader* pSReader = nullptr;

    hr = MFCreateSourceReaderFromMediaSource(pSource, nullptr, &pSReader);
    if (FAILED(hr)) {
        std::cerr << "Fail to create Source Reader!\n";
        goto done;
    }

    // get video format (SUBTYPE: GUID)
    hr = pSReader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, &pVideoType);
    if (FAILED(hr)) {
        std::cerr << "Cannot get media type of Source Reader!\n";
        goto done;
    }
    hr = pVideoType->GetGUID(MF_MT_SUBTYPE, pVideoFormat);
    if (FAILED(hr)) {
        std::cerr << "Cannot get subtype of media type!\n";
        goto done;
    }
    SafeRelease(&pVideoType);

    // set up video format
    pVideoType = nullptr;
    hr = MFCreateMediaType(&pVideoType);
    if (FAILED(hr)) {
        std::cerr << "Fail to create media type!\n";
        goto done;
    }
    hr = pVideoType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    if (FAILED(hr)) {
        std::cerr << "Fail to set major type media type!\n";
        goto done;
    }
    hr = pVideoType->SetGUID(MF_MT_SUBTYPE, *pVideoFormat);
    if (FAILED(hr)) {
        std::cerr << "Fail to set subtype of media type!\n";
        goto done;
    }
    
    hr = pSReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pVideoType);
    if (FAILED(hr)) {
        std::cerr << "Fail to set media type for Source Reader!\n";
        goto done;
    }

    (*pReader) = pSReader;
    (*pReader)->AddRef();

done:
    SafeRelease(&pVideoType);
    SafeRelease(&pSReader);

    return hr;
}

HRESULT InitializeAndFormatSinkWriter(LPCWSTR filename, GUID pInputVideoFormat, IMFSinkWriter **ppWriter, DWORD *pStreamIndex) {
    // Format constants
    const UINT32 VIDEO_WIDTH = 1920;
    const UINT32 VIDEO_HEIGHT = 1080;
    const UINT32 VIDEO_FPS = 30;
    const UINT32 VIDEO_BIT_RATE = 2500000;
    const GUID   VIDEO_ENCODING_FORMAT = MFVideoFormat_H264;
    const GUID   VIDEO_INPUT_FORMAT = pInputVideoFormat;

    HRESULT hr = S_OK;
    IMFSinkWriter* pSWriter = nullptr;
    IMFMediaType *pMediaTypeOut = nullptr;   
    IMFMediaType *pMediaTypeIn = nullptr;
    IMFAttributes *pAttributes = nullptr;
    DWORD streamIndex;

    hr = MFCreateSinkWriterFromURL(filename, nullptr, pAttributes, &pSWriter);
    if (FAILED(hr)) {
        std::cerr << "Fail to create Sink Writer!\n";
        goto done;
    }
    // Set the output media type.
    if (SUCCEEDED(hr))
    {
        hr = MFCreateMediaType(&pMediaTypeOut);   
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);     
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetGUID(MF_MT_SUBTYPE, VIDEO_ENCODING_FORMAT);   
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetUINT32(MF_MT_AVG_BITRATE, VIDEO_BIT_RATE);   
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeOut->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);   
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeSize(pMediaTypeOut, MF_MT_FRAME_SIZE, VIDEO_WIDTH, VIDEO_HEIGHT);   
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_FRAME_RATE, VIDEO_FPS, 1);   
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeOut, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);   
    }
    if (SUCCEEDED(hr))
    {
        hr = pSWriter->AddStream(pMediaTypeOut, &streamIndex);   
    }
    // Set the input media type.
    if (SUCCEEDED(hr))
    {
        hr = MFCreateMediaType(&pMediaTypeIn);   
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeIn->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);   
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeIn->SetGUID(MF_MT_SUBTYPE, VIDEO_INPUT_FORMAT);     
    }
    if (SUCCEEDED(hr))
    {
        hr = pMediaTypeIn->SetUINT32(MF_MT_INTERLACE_MODE, MFVideoInterlace_Progressive);   
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeSize(pMediaTypeIn, MF_MT_FRAME_SIZE, VIDEO_WIDTH, VIDEO_HEIGHT);   
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_FRAME_RATE, VIDEO_FPS, 1);   
    }
    if (SUCCEEDED(hr))
    {
        hr = MFSetAttributeRatio(pMediaTypeIn, MF_MT_PIXEL_ASPECT_RATIO, 1, 1);   
    }
    if (SUCCEEDED(hr))
    {
        hr = pSWriter->SetInputMediaType(streamIndex, pMediaTypeIn, NULL);   
    }
    
    (*ppWriter) = pSWriter;
    (*ppWriter)->AddRef();
    *pStreamIndex = streamIndex;

done:
    SafeRelease(&pMediaTypeOut);
    SafeRelease(&pMediaTypeIn);
    SafeRelease(&pSWriter);
    return hr;
}

HRESULT StartRecord(IMFSourceReader *pReader, IMFSinkWriter *pWriter, DWORD streamIndex) {
    HRESULT hr = S_OK;
    DWORD flags = 0;
    IMFSample* pSample = nullptr;
    LONGLONG time = 0;
    LONGLONG sampleTimeStamp = 1e7 / 30;
    LONGLONG videoDuration = 5;

    videoDuration *= 1e7;
    while (time < videoDuration) {        
        hr = pReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, 0, nullptr, &flags, nullptr, &pSample);
        if (FAILED(hr)) {
            std::cerr << "Read sample error!\n";
            break;
        }
        if (flags & MF_SOURCE_READERF_ENDOFSTREAM) {
            std::cerr << "Finish!\n";
            break;
        }
        if (pSample) {
            pSample->SetSampleTime(time);
            pSample->SetSampleDuration(sampleTimeStamp);
            hr = pWriter->WriteSample(streamIndex, pSample);
            if (FAILED(hr)) {
                std::cerr << "Fail to write sample!\n";
            }
        }
        SafeRelease(&pSample);
        if (FAILED(hr)) break;
        time += sampleTimeStamp;
    }

    return hr;
}

HRESULT WebcamCapture(LPCWSTR filename) { 
    IMFMediaSource* pSource = nullptr;
    IMFSourceReader* pReader = nullptr;
    IMFSinkWriter* pWriter = nullptr;
    GUID pVideoFormat = MFVideoFormat_YUY2;

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr)) {
        std::cerr << "Fail to init COM!\n";
        goto done;
    }
    hr = MFStartup(MF_VERSION);
    if (FAILED(hr)) {
        std::cerr << "Failed to init MF\n";
        goto done;
    }

    hr = GetWebcamMediaSource(&pSource);
    if (FAILED(hr)) {
        std::cerr << "Fail to get webcam's media source!\n";
        goto done;
    }

    hr = InitializeAndFormatSourceReader(pSource, &pReader, &pVideoFormat);
    if (FAILED(hr)) {
        std::cerr << "Fail to init Source Reader!\n";
        goto done;
    }
    DWORD streamIndex;
    hr = InitializeAndFormatSinkWriter(filename, pVideoFormat, &pWriter, &streamIndex);
    if (FAILED(hr)) {
        std::cerr << "Fail to init Sink Writer!\n";
        goto done;
    }

    hr = pWriter->BeginWriting();
    if (FAILED(hr)) {
        std::cerr << "Fail to begin writing!\n";
        goto done;
    }

    hr = StartRecord(pReader, pWriter, streamIndex);
    if (FAILED(hr)) {
        std::cerr << "Fail to record!\n";
        goto done;
    }

done:
    pSource->Shutdown();
    pWriter->Finalize();

    SafeRelease(&pSource);
    SafeRelease(&pReader);
    SafeRelease(&pWriter);

    MFShutdown();
    CoUninitialize();
    return 0;
}