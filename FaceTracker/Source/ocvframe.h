///////////////////////////////////////////////////////////////////////////////
// Name:        ocvframe.h
// Purpose:     Gets and displays images coming from various OpenCV sources
// Author:      PB
// Created:     2020-09-16
// Copyright:   (c) 2020 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


#ifndef OCVFRAME_H
#define OCVFRAME_H

#include <wx/wx.h>
#include <wx/txtstrm.h>
#include <opencv2/opencv.hpp>
#include "IrisLandmark.hpp"
#include "bmpfromocvpanel.h"
#include <fstream>
#include <map>
#include <list>
// forward declarations
class WXDLLIMPEXP_FWD_CORE wxSlider;

namespace cv
{
    class Mat;
    class VideoCapture;
}

class CameraThread;

typedef std::vector<cv::Point> PointRecord;

// max number of records
#define MAX_RECORDS (10000)
// This class can open an OpenCV source of images (image file, video file,
// default WebCam, and IP camera) and display the images using wxBitmapFromOpenCVPanel.
class OpenCVFrame : public wxBitmapFromOpenCVPanel
{
public:
    OpenCVFrame(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0);
    ~OpenCVFrame();
    enum Mode
    {
        Empty = 0,
        Image,
        Video,
        WebCam,
        IPCamera,
    };
private:

    Mode                     m_mode{Empty};
    wxString                 m_sourceName;
    int                      m_currentVideoFrameNumber{0};

    cv::VideoCapture*        m_videoCapture{nullptr};
    cv::VideoWriter*         _videoWriter = nullptr;
    CameraThread*            m_cameraThread{nullptr};
    wxSlider*                m_videoSlider = nullptr;
    my::IrisLandmark        irisLandmarker;
    PointRecord             _lastRecord;
    PointRecord             _refRecord; // the current reference
    std::list<PointRecord>  _pointRecord; // set of point records
    std::list<PointRecord>  _landmarkList;
    int                     _fileSequenceId = 1;
    int                     _frameCount = 0;
    bool                    _recordEnable = false;
    bool                    _writeToFile = false;
    int                     _writeFrameId = 1;
    wxString                _imageFileName = "./Images";
    bool                    _drawBones=false;
    bool                    _drawLandmarks =false;
    cv::Size                _sizeFrame = cv::Size(640,480);

    static wxBitmap ConvertMatToBitmap(const cv::Mat& matBitmap, long& timeConvert);

    void Clear();


    // If address is empty, the default webcam is used.
    // resolution and useMJPEG are used only for webcam.
    bool StartCameraCapture(const wxString& address,
                            const wxSize& resolution = wxSize(),
                            bool useMJPEG = false);
    bool StartCameraThread();
    void DeleteCameraThread();
    //
    void OnCameraFrame(wxThreadEvent& evt);
    void OnCameraEmpty(wxThreadEvent&);
    void OnCameraException(wxThreadEvent& evt);
public:
    void setVideoSlider(wxSlider *s) { m_videoSlider = s; }
    bool OnImage();
    bool OnVideo();
    bool OnWebCam();
    bool OnIPCamera();
    void OnClear();
    void OnProperties();
    void OnVideoSetFrame(int);
    void writeRecord(wxTextOutputStream  &os);
    void processFrame(cv::Mat &matBitmap );
    void ShowVideoFrame(int frameNumber);
    void writeRigify(wxTextOutputStream &os);
    void writeRigifyLast(wxTextOutputStream &os);

    void writeLandmarks(wxTextOutputStream &os);
    void setRecordEnable(bool f) { _recordEnable = f;}
    void setWriteToFile(bool f) { _writeToFile = f;}
    void setImageFileName(const wxString &s) { _imageFileName = s;}
    void setDrawBones(bool f) { _drawBones = f;}
    void setDrawLandmarks(bool f) { _drawLandmarks = f;}
    void startVideoWrite(cv::VideoCapture *src);
    void endVideoWrite();
    void setCurrentAsReference();
    void writeRefRecord(wxString &s);


};

#endif // #ifndef OCVFRAME_H
