///////////////////////////////////////////////////////////////////////////////
// Name:        ocvframe.cpp
// Purpose:     Gets and displays images coming from various OpenCV sources
// Author:      PB
// Created:     2020-09-16
// Copyright:   (c) 2020 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/choicdlg.h>
#include <wx/filedlg.h>
#include <wx/listctrl.h>
#include <wx/slider.h>
#include <wx/textdlg.h>
#include <wx/thread.h>
#include <wx/utils.h>
#include "keypoints.h"

#include "convertmattowxbmp.h"
#include "ocvframe.h"

#include <wx/wfstream.h>
#include <wx/sstream.h>
// A frame was retrieved from WebCam or IP Camera.
wxDEFINE_EVENT(wxEVT_CAMERA_FRAME, wxThreadEvent);
// Could not retrieve a frame, consider connection to the camera lost.
wxDEFINE_EVENT(wxEVT_CAMERA_EMPTY, wxThreadEvent);
// An exception was thrown in the camera thread.
wxDEFINE_EVENT(wxEVT_CAMERA_EXCEPTION, wxThreadEvent);


/*!
 * \brief reduceToBones
 * \param ir
 * \param pts
 */
void reduceToBones(my::IrisLandmark &ir, std::vector<cv::Point> &pts)
{
    std::vector<std::string> &bl =  getBoneList();
    pts.resize(bl.size());
    auto l = ir.getAllFaceLandmarks();
    if(l.size() > 0)
    {
        pointMap &m =  getFaceKeyPoints();
        for(int i = 0; i < bl.size(); i++)
        {
            pointList &a = m[bl[i]];
            if(a.size() > 0)
            {
                // sum x and y and calculate mean
                int x = 0;
                int y = 0;
                for(int j = 0; j < a.size(); j++)
                {
                    int k = a[j];
                    x += l[k].x;
                    y += l[k].y;
                }
                pts[i] = cv::Point(x/a.size(),y/a.size());
            }
        }
    }
}
/*!
 * \brief doFaceTrack
 * \param ir
 * \param matBitmap
 * \param meanx
 * \param meany
 * \param irisRight
 * \param irisLeft
 */
void doFaceTrack(my::IrisLandmark &ir, cv::Mat &matBitmap,int &meanx, int &meany, cv::Point &irisRight, cv::Point &irisLeft, bool drawLandmarks = true )
{
    cv::flip(matBitmap, matBitmap, 1);
    ir.loadImageToInput(matBitmap);
    ir.runInference();
    meanx = meany = 0;
    int n = 0;
    std::vector<cv::Point> al = ir.getAllFaceLandmarks();
    for (auto landmark: al) {
        meanx += landmark.x;
        meany += landmark.y;
        if(drawLandmarks) cv::circle(matBitmap, landmark, 2, cv::Scalar(0, 255, 0), -1);
    }
    if(al.size()  > 0)
    {
        meanx /= al.size();
        meany /= al.size();
    }
    //
    int mx = 0;
    int my = 0;
    // determine the iris positions
    std::vector<cv::Point> il = ir.getAllEyeLandmarks(true, true);
    if(il.size() > 0)
    {
        for (auto landmark: il) {
            mx += landmark.x;
            my += landmark.y;
            if(drawLandmarks) cv::circle(matBitmap, landmark, 2, cv::Scalar(0, 0, 255), -1);
        }
        irisLeft.x = mx / il.size();
        irisLeft.y = my / il.size();
    }
    //
    mx = 0;
    my = 0;
    il = ir.getAllEyeLandmarks(false, true);
    if(il.size() > 0)
    {
        for (auto landmark: il) {
            mx += landmark.x;
            my += landmark.y;
            if(drawLandmarks) cv::circle(matBitmap, landmark, 2, cv::Scalar(0,0, 255), -1);
        }
        irisRight.x = mx / il.size();
        irisRight.y = my / il.size();
    }
}

//
// Worker thread for retrieving images from WebCam or IP Camera
// and sending them to the main thread for display.
/*!
 * \brief The CameraThread class
 */
class CameraThread : public wxThread
{
public:
    struct CameraFrame
    {
        cv::Mat matBitmap;
        long    timeGet{0};
    };

    CameraThread(wxEvtHandler* eventSink, cv::VideoCapture* camera);

protected:
    wxEvtHandler*     m_eventSink{nullptr};
    cv::VideoCapture* m_camera{nullptr};
    ExitCode Entry() override;
};
/*!
 * \brief CameraThread::CameraThread
 * \param eventSink
 * \param camera
 */
CameraThread::CameraThread(wxEvtHandler* eventSink, cv::VideoCapture* camera)
    : wxThread(wxTHREAD_JOINABLE),
      m_eventSink(eventSink),
      m_camera(camera)
{
    wxASSERT(m_eventSink);
    wxASSERT(m_camera);
}
/*!
 * \brief CameraThread::Entry
 * \return
 */
wxThread::ExitCode CameraThread::Entry()
{
    wxStopWatch  stopWatch;

    while ( !TestDestroy() )
    {
        CameraFrame* frame = nullptr;

        try
        {
            frame = new CameraFrame;
            stopWatch.Start();
            (*m_camera) >> frame->matBitmap;
            frame->timeGet = stopWatch.Time();

            if ( !frame->matBitmap.empty() )
            {
                wxThreadEvent* evt = new wxThreadEvent(wxEVT_CAMERA_FRAME);

                evt->SetPayload(frame);
                m_eventSink->QueueEvent(evt);
                // In a real code, the duration to sleep would normally
                // be computed based on the camera framerate, time taken
                // to process the image, and system clock tick resolution.
                wxMilliSleep(30);
            }
            else // connection to camera lost
            {
                m_eventSink->QueueEvent(new wxThreadEvent(wxEVT_CAMERA_EMPTY));
                wxDELETE(frame);
                break;
            }
        }
        catch ( const std::exception& e )
        {
            wxThreadEvent* evt = new wxThreadEvent(wxEVT_CAMERA_EXCEPTION);

            wxDELETE(frame);
            evt->SetString(e.what());
            m_eventSink->QueueEvent(evt);
            break;
        }
        catch ( ... )
        {
            wxThreadEvent* evt = new wxThreadEvent(wxEVT_CAMERA_EXCEPTION);

            wxDELETE(frame);
            evt->SetString("Unknown exception");
            m_eventSink->QueueEvent(evt);
            break;
        }
    }

    return static_cast<wxThread::ExitCode>(nullptr);
}

//
// OpenCVFrame
//
/*!
 * \brief OpenCVFrame::OpenCVFrame
 * \param parent
 * \param id
 * \param pos
 * \param size
 * \param style
 */
OpenCVFrame::OpenCVFrame(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style)
    : wxBitmapFromOpenCVPanel(parent, wxID_ANY,pos,size,style),irisLandmarker("./models")
{
    Clear();
    Bind(wxEVT_CAMERA_FRAME, &OpenCVFrame::OnCameraFrame, this);
    Bind(wxEVT_CAMERA_EMPTY, &OpenCVFrame::OnCameraEmpty, this);
    Bind(wxEVT_CAMERA_EXCEPTION, &OpenCVFrame::OnCameraException, this);
}
/*!
 * \brief OpenCVFrame::~OpenCVFrame
 */
OpenCVFrame::~OpenCVFrame()
{
    DeleteCameraThread();
}

/*!
 * \brief OpenCVFrame::writeLandmarks
 * \param os
 */
void OpenCVFrame::writeLandmarks(wxTextOutputStream &os)
{
    for(auto &i : _landmarkList)
    {
        os << int(i.size()) << ",";
        for(auto p : i)
        {
            os << p.x << " " << p.y << ",";
        }
        os << endl;
    }
}
/*!
 * \brief OpenCVFrame::writeRecord
 * \param os
 */
void OpenCVFrame::writeRecord(wxTextOutputStream &os)
{
    for(auto &i : _pointRecord)
    {
        os << int(i.size()) << ",";
        for(auto p : i)
        {
            os << p.x << " " << p.y << ",";
        }
        os << endl;
    }
}
//
// to generate the actions for Makehuman primary face bone rigify, generate in this order
//
#define N_FACE_RIGIFY (22)
static const int rigifyIndex[N_FACE_RIGIFY] = {
    37, //"chin"
    23,//"nose.002", #23
    25,//"nose.R.001",#25
    26,//"nose.L.001",#26
    27,//"cheek.B.R.001",#27
    28,//"cheek.B.L.001",#28
    35,//"lip.B",#35
    36,//"lip.T",#36
    33,//"lip.T.R.001",#33
    34,//"lip.T.L.001",#34
    31,//"lip.B.R.001",#31
    32,//"lip.B.L.001",#32
    29,//"lip_end.R.001",#29
    30,//"lip_end.L.001",#30
    4,//"lid.T.R.002",# 4
    13,//"lid.T.L.002",#13
    7,//"lid.B.R.002",#7
    16,//"lid.B.L.002",#16
    11,//"brow.T.R.002",#11
    20,//"brow.T.L.002",#20
    12,//"brow.T.R.003",#12
    21//"brow.T.L.003" # 21
};

/*!
 * \brief OpenCVFrame::writeRigify
 * \param os
 */
void OpenCVFrame::writeRigify(wxTextOutputStream &os)
{
    //
    // assume the first record is the rest point. Convert to doubles
    //
    if( _refRecord.size() >= N_FACE_RIGIFY)
    {
        for(auto &i : _pointRecord)
        {
            // the last two entries are the left and right irises
            int k = int(i.size());
            float w = abs(i[k-2].x - i[k-1].x);
            if(w > 0)
            {
                for(int j = 0; j < N_FACE_RIGIFY; j++ )
                {
                    cv::Point &a = i[j];
                    double x = double(a.x -  _refRecord[j].x)/w;
                    double y = double(a.y -  _refRecord[j].y)/w;
                    os << x << " " << y << ",";
                }
            }
            os << endl;
        }
    }
}

void OpenCVFrame::writeRefRecord(wxString &s)
{
    wxFFileOutputStream output( s,"w" );
    if(output.IsOk())
    {
        wxTextOutputStream os( output );

        for(auto &i : _pointRecord)
        {
            os << int(i.size()) << ",";
            for(auto p : i)
            {
                os << p.x << " " << p.y << ",";
            }
            os << endl;
        }
    }
}


void OpenCVFrame::writeRigifyLast(wxTextOutputStream &os)
{
    if( _refRecord.size() >= N_FACE_RIGIFY)
    {
        // the last two entries are the left and right irises
        int k = int(_lastRecord.size());
        float w = abs(_lastRecord[k-2].x - _lastRecord[k-1].x);
        if(w > 0)
        {
            for(int j = 0; j < N_FACE_RIGIFY; j++ )
            {
                cv::Point &a = _lastRecord[j];
                double x = double(a.x -  _refRecord[j].x)/w;
                double y = double(a.y -  _refRecord[j].y)/w;
                os << x << " " << y << ",";
            }
        }
        os << endl;
    }
}



/*!
 * \brief OpenCVFrame::setCurrentAsReference
 */
void OpenCVFrame::setCurrentAsReference()
{
    _refRecord = _lastRecord;
}


/*!
 * \brief OpenCVFrame::processFrame
 * \param matBitmap
 */
static const cv::Rect  marker(0,0,5,5);
void OpenCVFrame::processFrame(cv::Mat &matBitmap )
{
    cv::Point il; // eyes
    cv::Point ir;
    // centre on zero
    int meanx;
    int meany;
    //
    doFaceTrack(irisLandmarker,matBitmap,meanx,meany,ir,il,_drawLandmarks); // track the face
    reduceToBones(irisLandmarker, _lastRecord);
    _lastRecord.push_back(ir);
    _lastRecord.push_back(il);
    //
    for(auto &i : _lastRecord)
    {

        if(_drawBones) cv::rectangle(matBitmap, marker + i, cv::Scalar(255, 0, 0),2); // mark reduced bones on image
        i.x = (i.x - meanx);
        i.y = (i.y - meany);
    }
    //
    if(_drawBones)
    {
        cv::rectangle(matBitmap, marker + il,cv::Scalar(255, 0, 0)); // mark reduced bones on image
        cv::rectangle(matBitmap, marker + ir, cv::Scalar(255, 0, 0)); // mark reduced bones on image
    }
    //
    if(_recordEnable)
    {
        _landmarkList.push_back(irisLandmarker.getAllFaceLandmarks());// save all landmarks
        _pointRecord.push_back(_lastRecord); // save the reduced bones
    }
}
/*!
 * \brief OpenCVFrame::ConvertMatToBitmap
 * \param matBitmap
 * \param timeConvert
 * \return
 */
wxBitmap OpenCVFrame::ConvertMatToBitmap(const cv::Mat& matBitmap, long& timeConvert)
{
    wxCHECK(!matBitmap.empty(), wxBitmap());

    wxBitmap    bitmap(matBitmap.cols, matBitmap.rows, 24);
    bool        converted = false;
    wxStopWatch stopWatch;
    long        time = 0;

    stopWatch.Start();
    converted = ConvertMatBitmapTowxBitmap(matBitmap, bitmap);
    time = stopWatch.Time();

    if ( !converted )
    {
        wxLogError("Could not convert Mat to wxBitmap.");
        return wxBitmap();
    }

    timeConvert = time;
    return bitmap;
}
/*!
 * \brief OpenCVFrame::Clear
 */
void OpenCVFrame::Clear()
{
    DeleteCameraThread();
    _pointRecord.clear();
    _landmarkList.clear();
    _recordEnable = false;
    if ( m_videoCapture )
    {
        endVideoWrite();
        wxDELETE(m_videoCapture);
    }

    m_mode = Empty;
    m_sourceName.clear();
    m_currentVideoFrameNumber = 0;

    SetBitmap(wxBitmap(), 0, 0);
    if(m_videoSlider)
    {
        m_videoSlider->SetValue(0);
        m_videoSlider->SetRange(0, 1);
        m_videoSlider->Disable();
        m_videoSlider->Hide();
    }

}

/*!
 * \brief OpenCVFrame::ShowVideoFrame
 * \param frameNumber
 */
void OpenCVFrame::ShowVideoFrame(int frameNumber)
{
    if(frameNumber < _frameCount)
    {
        const int   captureFrameNumber = (int)(m_videoCapture->get(cv::CAP_PROP_POS_FRAMES));
        cv::Mat     matBitmap;
        wxStopWatch stopWatch;
        long        timeGet = 0;

        stopWatch.Start();
        if ( frameNumber != captureFrameNumber)
            m_videoCapture->set(cv::VideoCaptureProperties::CAP_PROP_POS_FRAMES, frameNumber);
        (*m_videoCapture) >> matBitmap;
        timeGet = stopWatch.Time();

        if ( matBitmap.empty() )
        {
            SetBitmap(wxBitmap(), 0, 0);
            wxLogError("Could not retrieve frame %d.", frameNumber);
            return;
        }

        wxBitmap bitmap;
        long     timeConvert = 0;
        processFrame(matBitmap);
        bitmap = ConvertMatToBitmap(matBitmap, timeConvert);

        if ( !bitmap.IsOk() )
        {
            SetBitmap(wxBitmap(), 0, 0);
            wxLogError("Could not convert frame %d to wxBitmap.", frameNumber);
            return;
        }
        //
        SetBitmap(bitmap, timeGet, timeConvert);
        //
        if(_videoWriter)
        {
            cv::Mat xframe;
            cv::resize(matBitmap,xframe,_sizeFrame);
            _videoWriter->write(matBitmap);
        }

        m_videoSlider->SetValue(frameNumber);
    }
}
/*!
 * \brief OpenCVFrame::StartCameraCapture
 * \param address
 * \param resolution
 * \param useMJPEG
 * \return
 */
bool OpenCVFrame::StartCameraCapture(const wxString& address, const wxSize& resolution,
                                     bool useMJPEG)
{
    const bool        isDefaultWebCam = address.empty();
    cv::VideoCapture* cap = nullptr;
    _sizeFrame = cv::Size(resolution.GetWidth(),resolution.GetHeight());
    Clear();

    {
        wxWindowDisabler disabler;
        wxBusyCursor     busyCursor;

        if ( isDefaultWebCam )
            cap = new cv::VideoCapture(0);
        else
            cap = new cv::VideoCapture(address.ToStdString());
    }

    if ( !cap->isOpened() )
    {
        delete cap;
        wxLogError("Could not connect to the camera.");
        return false;
    }

    m_videoCapture = cap;

    if ( isDefaultWebCam )
    {
        m_videoCapture->set(cv::CAP_PROP_FRAME_WIDTH, resolution.GetWidth());
        m_videoCapture->set(cv::CAP_PROP_FRAME_HEIGHT, resolution.GetHeight());

        if ( useMJPEG )
            m_videoCapture->set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    }

    if ( !StartCameraThread() )
    {
        Clear();
        return false;
    }

    startVideoWrite(m_videoCapture);
    return true;
}
/*!
 * \brief OpenCVFrame::StartCameraThread
 * \return
 */
bool OpenCVFrame::StartCameraThread()
{
    DeleteCameraThread();

    m_cameraThread = new CameraThread(this, m_videoCapture);
    if ( m_cameraThread->Run() != wxTHREAD_NO_ERROR )
    {
        wxDELETE(m_cameraThread);
        wxLogError("Could not create the thread needed to retrieve the images from a camera.");
        return false;
    }

    return true;
}
/*!
 * \brief OpenCVFrame::DeleteCameraThread
 */
void OpenCVFrame::DeleteCameraThread()
{
    if ( m_cameraThread )
    {
        m_cameraThread->Delete(nullptr, wxTHREAD_WAIT_BLOCK);
        wxDELETE(m_cameraThread);
    }
}

/*!
 * \brief OpenCVFrame::startVideoWrite
 */
void OpenCVFrame::startVideoWrite(cv::VideoCapture *src)
{
    if(_writeToFile)
    {
        if(!_videoWriter)
        {
            _videoWriter = new cv::VideoWriter();
            int codec = cv::VideoWriter::fourcc('M', 'P', '4', 'V');
            double fps = 15.0;
            wxString f;
            // find the next free file name
            do
            {
                f = _imageFileName + wxString::Format("/M%06d.mp4",_fileSequenceId++);
            } while(wxFileExists(f));
            std::string filename = f.ToStdString();
            _videoWriter->open(filename, codec, fps, _sizeFrame, true);
        }
    }
}
/*!
 * \brief OpenCVFrame::endVideoWrite
 */
void OpenCVFrame::endVideoWrite()
{
    if(_videoWriter)
    {
        _videoWriter->release();
        delete _videoWriter;
        _videoWriter = nullptr;
    }
}


/*!
 * \brief OpenCVFrame::OnImage
 * \return
 */
bool OpenCVFrame::OnImage()
{
    static wxString fileName;

    fileName = wxFileSelector("Select Bitmap Image", "", fileName, "",
                              "Image files (*.jpg;*.png;*.tga;*.bmp)|*.jpg;*.png;*.tga;*.bmp",
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

    if ( fileName.empty() )
        return false;

    cv::Mat matBitmap;
    wxStopWatch stopWatch;
    long timeGet = 0, timeConvert = 0;

    stopWatch.Start();
    matBitmap = cv::imread(fileName.ToStdString(), cv::IMREAD_COLOR);
    timeGet = stopWatch.Time();

    if ( matBitmap.empty() )
    {
        wxLogError("Could not read image '%s'.", fileName);
        return false;
    }

    //
    processFrame(matBitmap);

    wxBitmap bitmap = ConvertMatToBitmap(matBitmap, timeConvert);

    if ( !bitmap.IsOk())
    {
        wxLogError("Could not convert Mat to wxBitmap.", fileName);
        Clear();
        return false;
    }

    SetBitmap(bitmap, timeGet, timeConvert);
    m_mode = Image;
    m_sourceName = fileName;
    return true;
}
/*!
 * \brief OpenCVFrame::OnVideo
 * \return
 */
bool OpenCVFrame::OnVideo()
{
    static wxString fileName;

    fileName = wxFileSelector("Select Video", "", fileName, "",
                              "Video files (*.avi;*.mp4;*.mkv)|*.avi;*.mp4;*.mkv",
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST, this);

    if ( fileName.empty() )
        return false;

    cv::VideoCapture* cap = new cv::VideoCapture(fileName.ToStdString());
    int               frameCount = 0;

    if ( !cap->isOpened() )
    {
        wxLogError("Could not read video '%s'.", fileName);
        delete cap;
        Clear();
        return false;
    }

    m_videoCapture = cap;
    m_mode = Video;
    m_sourceName = fileName;
    m_currentVideoFrameNumber = 0;
    //
    _pointRecord.clear();
    _landmarkList.clear();
    _recordEnable = true;
    //
    ShowVideoFrame(m_currentVideoFrameNumber);

    _frameCount = m_videoCapture->get(cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT);
    //
    startVideoWrite(m_videoCapture);
    //
    if(m_videoSlider)
    {
        m_videoSlider->SetValue(0);
        m_videoSlider->SetRange(0, _frameCount - 1);
        m_videoSlider->Enable();
        m_videoSlider->Show();
        m_videoSlider->SetFocus();
    }
    return true;
}

/*!
 * \brief OpenCVFrame::OnWebCam
 * \return
 */
bool OpenCVFrame::OnWebCam()
{
    static const wxSize resolutions[] = { { 320,  240},
        { 640,  480},
        { 800,  600},
        {1024,  576},
        {1280,  720},
        {1920, 1080}
    };
    static int    resolutionIndex = 1;
    wxArrayString resolutionStrings;
    bool          useMJPEG = false;

    for ( const auto& r : resolutions )
        resolutionStrings.push_back(wxString::Format("%d x %d", r.GetWidth(), r.GetHeight()));

    resolutionIndex = wxGetSingleChoiceIndex("Select resolution", "WebCam",
                      resolutionStrings, resolutionIndex, this);
    if ( resolutionIndex != -1 )
    {
        useMJPEG = wxMessageBox("Press Yes to use MJPEG or No to use the default FourCC.\nMJPEG may be much faster, particularly at higher resolutions.",
                                "WebCamera", wxYES_NO, this) == wxYES;

        if ( StartCameraCapture(wxEmptyString, resolutions[resolutionIndex], useMJPEG) )
        {
            m_mode = WebCam;
            m_sourceName = "Default WebCam";
            return true;
        }
    }
    return false;
}
/*!
 * \brief OpenCVFrame::OnIPCamera
 * \return
 */
bool OpenCVFrame::OnIPCamera()
{
    static wxString address;

    address = wxGetTextFromUser("Enter the URL including protocol, address, port etc.",
                                "IP camera", address, this);

    if ( !address.empty() )
    {
        if ( StartCameraCapture(address) )
        {
            m_mode = IPCamera;
            m_sourceName = address;
            return true;
        }
    }
    return false;
}
/*!
 * \brief OpenCVFrame::OnClear
 */
void OpenCVFrame::OnClear()
{
    Clear();
}
/*!
 * \brief OpenCVFrame::OnProperties
 */
void OpenCVFrame::OnProperties()
{
    wxArrayString properties;

    properties.push_back(wxString::Format("Source: %s", m_sourceName));

    if ( m_mode == Image )
    {
        const wxBitmap& bmp = GetBitmap();

        wxCHECK_RET(bmp.IsOk(), "Invalid bitmap in m_bitmapPanel");
        properties.push_back(wxString::Format("Width: %d", bmp.GetWidth()));
        properties.push_back(wxString::Format("Height: %d", bmp.GetHeight()));
    }

    if ( m_videoCapture )
    {
        const int  fourCCInt   = static_cast<int>(m_videoCapture->get(cv::CAP_PROP_FOURCC));
        const char fourCCStr[] = {(char)(fourCCInt  & 0XFF),
                                  (char)((fourCCInt & 0XFF00) >> 8),
                                  (char)((fourCCInt & 0XFF0000) >> 16),
                                  (char)((fourCCInt & 0XFF000000) >> 24), 0
                                 };

        properties.push_back(wxString::Format("Backend: %s", wxString(m_videoCapture->getBackendName())));

        properties.push_back(wxString::Format("Width: %.0f", m_videoCapture->get(cv::CAP_PROP_FRAME_WIDTH)));
        properties.push_back(wxString::Format("Height: %0.f", m_videoCapture->get(cv::CAP_PROP_FRAME_HEIGHT)));

        properties.push_back(wxString::Format("FourCC: %s", fourCCStr));
        properties.push_back(wxString::Format("FPS: %.1f", m_videoCapture->get(cv::CAP_PROP_FPS)));

        if ( m_mode == Video )
        {
            // abuse wxDateTime to display position in video as time
            wxDateTime time(static_cast<time_t>(m_videoCapture->get(cv::CAP_PROP_POS_MSEC) / 1000));

            time.MakeUTC(true);

            properties.push_back(wxString::Format("Current frame: %.0f", m_videoCapture->get(cv::CAP_PROP_POS_FRAMES) - 1.0));
            properties.push_back(wxString::Format("Current time: %s", time.FormatISOTime()));
            properties.push_back(wxString::Format("Total frame count: %.f", m_videoCapture->get(cv::CAP_PROP_FRAME_COUNT)));
#if CV_VERSION_MAJOR > 4 || (CV_VERSION_MAJOR == 4 && CV_VERSION_MINOR >= 3 )
            properties.push_back(wxString::Format("Bitrate: %.0f kbits/s", m_videoCapture->get(cv::CAP_PROP_BITRATE)));
#endif
        }
    }

    wxGetSingleChoice("Name: value", "Properties", properties, this);
}
/*!
 * \brief OpenCVFrame::OnVideoSetFrame
 * \param requestedFrameNumber
 */
void OpenCVFrame::OnVideoSetFrame(int requestedFrameNumber)
{
    if ( requestedFrameNumber == m_currentVideoFrameNumber )
        return;
    m_currentVideoFrameNumber = requestedFrameNumber;
    ShowVideoFrame(m_currentVideoFrameNumber);
}
/*!
 * \brief OpenCVFrame::OnCameraFrame
 * \param evt
 */
void OpenCVFrame::OnCameraFrame(wxThreadEvent& evt)
{
    CameraThread::CameraFrame* frame = evt.GetPayload<CameraThread::CameraFrame*>();

    // After deleting the camera thread we may still get a stray frame
    // from yet unprocessed event, just silently drop it.
    if ( m_mode != IPCamera && m_mode != WebCam )
    {
        delete frame;
        return;
    }

    long     timeConvert = 0;

    processFrame(frame->matBitmap);

    wxBitmap bitmap = ConvertMatToBitmap(frame->matBitmap, timeConvert);

    if ( bitmap.IsOk() )
    {
        SetBitmap(bitmap, frame->timeGet, timeConvert);
        if(_videoWriter)
        {
            cv::Mat xframe;
            cv::resize(frame->matBitmap,xframe,_sizeFrame);
            _videoWriter->write(frame->matBitmap);
        }
    }
    else
    {
        SetBitmap(wxBitmap(), 0, 0);
    }

    delete frame;
}
/*!
 * \brief OpenCVFrame::OnCameraEmpty
 */
void OpenCVFrame::OnCameraEmpty(wxThreadEvent&)
{
    wxLogError("Connection to the camera lost.");
    endVideoWrite();
    Clear();
}
/*!
 * \brief OpenCVFrame::OnCameraException
 * \param evt
 */
void OpenCVFrame::OnCameraException(wxThreadEvent& evt)
{
    wxLogError("Exception in the camera thread: %s", evt.GetString());
    Clear();
}
