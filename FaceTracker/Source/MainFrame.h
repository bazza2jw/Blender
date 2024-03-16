#ifndef MAINFRAME_H
#define MAINFRAME_H
#include <opencv2/opencv.hpp>
#include "IrisLandmark.hpp"
#include "FaceTracker.h"
#include <chrono>
#include <wx/socket.h>


class OpenCVFrame;
class MainFrame : public MainFrameBase
{
    OpenCVFrame * _cvFrame = nullptr;
    bool _playVideo = false;
    int _currentFrame = 0;
    wxSocketServer  * _server = nullptr;
    wxSocketBase    * _client = nullptr;
public:
    MainFrame(wxWindow* parent);
    virtual ~MainFrame();
protected:
    virtual void onSingleStep(wxCommandEvent& event);
    virtual void onSetRef(wxCommandEvent& event);
    virtual void OnFrameStep(wxCommandEvent& event);
    virtual void onSaveReference(wxCommandEvent& event);
    virtual void onSetCurrentAsRef(wxCommandEvent& event);
    virtual void onWriteBones(wxCommandEvent& event);
    virtual void onWriteLandmarks(wxCommandEvent& event);
    virtual void onDirNameChange(wxFileDirPickerEvent& event);
    virtual void onWriteImageChange(wxCommandEvent& event);
    virtual void onRecordStart(wxCommandEvent& event);
    virtual void onRecordStop(wxCommandEvent& event);
    virtual void onSaveReduced(wxCommandEvent& event);
    virtual void onSaveRigify(wxCommandEvent& event);
    virtual void onFileChanged(wxFileDirPickerEvent& event);
    virtual void onSaveRecords(wxCommandEvent& event);
    virtual void onVideoChanged(wxScrollEvent& event);
    virtual void onProperties(wxCommandEvent& event);
    virtual void onTimer(wxTimerEvent& event);
    virtual void onQuit(wxCommandEvent& event);
    virtual void onStart(wxCommandEvent& event);
    virtual void onStop(wxCommandEvent& event);
    void socketEvent(wxSocketEvent& event);
    void serverEvent(wxSocketEvent& event);

    wxDECLARE_EVENT_TABLE();


};
#endif // MAINFRAME_H
