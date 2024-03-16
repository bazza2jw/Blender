#include "MainFrame.h"
#include "IrisLandmark.hpp"
#include <iostream>
#include <wx/thread.h>
#include "ocvframe.h"
#include "keypoints.h"
#include <wx/wfstream.h>
#include <wx/sstream.h>

// initalse the bone map list to the
static const char * boneMapList =
    "lipsUpperOuter, (none)\n"
    "lipsLowerOuter, (none)\n"
    "lipsUpperInner, (none)\n"
    "lipsLowerInner, (none)\n"
    "rightEyeUpper0, (none)\n"
    "rightEyeLower0, (none)\n"
    "rightEyeUpper1, (none)\n"
    "rightEyeLower1, (none)\n"
    "rightEyeUpper2, (none)\n"
    "rightEyeLower2, (none)\n"
    "rightEyeLower3, (none)\n"
    "rightEyebrowUpper, (none)\n"
    "rightEyebrowLower, (none)\n"
    "rightEyeIris, (none)\n"
    "leftEyeUpper0, (none)\n"
    "leftEyeLower0, (none)\n"
    "leftEyeUpper1, (none)\n"
    "leftEyeLower1, (none)\n"
    "leftEyeUpper2, (none)\n"
    "leftEyeLower2, (none)\n"
    "leftEyeLower3, (none)\n"
    "leftEyebrowUpper, (none)\n"
    "leftEyebrowLower, (none)\n"
    "leftEyeIris, (none)\n"
    "midwayBetweenEyes, (none)\n"
    "noseTip, (none)\n"
    "noseBottom, (none)\n"
    "noseRightCorner, (none)\n"
    "noseLeftCorner, (none)\n"
    "rightCheek, (none)\n"
    "leftCheek, (none)";


enum
{
    // id for sockets
    SERVER_ID = 100,
    SOCKET_ID
};

wxBEGIN_EVENT_TABLE(MainFrame, MainFrameBase)
    EVT_SOCKET(SERVER_ID,  MainFrame::serverEvent)
    EVT_SOCKET(SOCKET_ID,  MainFrame::socketEvent)
wxEND_EVENT_TABLE()


/*!
 * \brief MainFrame::MainFrame
 * \param parent
 */
MainFrame::MainFrame(wxWindow* parent)
    : MainFrameBase(parent)
{
    GetDisplay()->setVideoSlider(GetVideoPosition());
    //
    wxIPV4address addr;

    addr.Service(5000);
    //
    _server = new wxSocketServer(addr);
    _server->SetEventHandler(*this,SERVER_ID);
    _server->SetNotify(wxSOCKET_CONNECTION_FLAG);
    _server->Notify(true);

}
/*!
 * \brief MainFrame::~MainFrame
 */
MainFrame::~MainFrame()
{

}

/*!
 * \brief MainFrame::serverEvent
 * \param event
 */
void MainFrame::serverEvent(wxSocketEvent& event)
{
    // Accept a connection

    wxSocketBase * c =  _server->Accept();
    if(!_client)
    {
        _client = c;
        _client->SetEventHandler(*this,SOCKET_ID);
        _client->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
        _client->Notify(true);
        _client->Write("\n",1);
    }
    else
    {
        // only one connection at a time
        c->Close();
        c->Destroy();
        _client = nullptr;
    }
}

/*!
 * \brief MainFrame::socketEvent
 * \param event
 */
void MainFrame::socketEvent(wxSocketEvent& event)
{
    switch(event.GetSocketEvent())
    {
    case wxSOCKET_INPUT:

        break;
    case wxSOCKET_LOST:
        _client->Destroy();
        _client = nullptr;
        break;
    default:
        break;
    }
}

/*!
 * \brief MainFrame::onQuit
 * \param e
 */
void MainFrame::onQuit(wxCommandEvent& e)
{
    GetDisplay()->OnClear();
    Destroy();
    wxSleep(1);
}
/*!
 * \brief MainFrame::onStart
 * \param event
 */
void MainFrame::onStart(wxCommandEvent& event)
{
    _currentFrame = 0;
    _playVideo = false;
    switch(static_cast<OpenCVFrame::Mode>(m_source->GetCurrentSelection()))
    {
    case OpenCVFrame::Mode::Empty:
    {
        GetTopToolbar()->EnableTool(wxID_PREFERENCES,false);
        GetDisplay()->OnClear();
    }
    break;
    case OpenCVFrame::Mode::Image:
    {
        GetTopToolbar()->EnableTool(wxID_PREFERENCES,GetDisplay()->OnImage());
    }
    break;
    case OpenCVFrame::Mode::Video:
    {
        bool f = GetDisplay()->OnVideo();
        GetVideoPosition()->SetValue(0);
        GetTopToolbar()->EnableTool(wxID_PREFERENCES,f);
        if(f)
        {
            _currentFrame = 1;
            _playVideo = !GetSingleStepEnable()->GetValue();

        }
    }
    break;
    case OpenCVFrame::Mode::WebCam:
    {
        GetTopToolbar()->EnableTool(wxID_PREFERENCES,GetDisplay()->OnWebCam());
    }
    break;
    case OpenCVFrame::Mode::IPCamera:
    {
        GetTopToolbar()->EnableTool(wxID_PREFERENCES,GetDisplay()->OnIPCamera());
    }
    break;
    default:
        break;
    }
}
/*!
 * \brief MainFrame::onStop
 * \param event
 */
void MainFrame::onStop(wxCommandEvent& event)
{
    _currentFrame = 0;
    _playVideo = false;
    GetDisplay()->OnClear();
}
/*!
 * \brief MainFrame::onTimer
 * \param event
 */
void MainFrame::onTimer(wxTimerEvent& event)
{
    // if in video mode play the next frame unless at the end
    if(_playVideo)
    {
        if(_currentFrame < GetVideoPosition()->GetMax())
        {
            GetDisplay()->OnVideoSetFrame(_currentFrame++);
        }
        else
        {
            GetVideoPosition()->SetValue(0);
            _currentFrame = 0;
            _playVideo = false;
        }
    }
    if(_client) // poll for commands
    {
        if(_client->IsConnected())
        {
            // Single character commands
            char c;
            if(_client->Read(&c,1).LastReadCount() == 1)
            {
                switch(c)
                {
                case 'R': // read last record
                {
                    // open a text stream to the socket
                    wxStringOutputStream s;
                    wxTextOutputStream ts( s);
                    GetDisplay()->writeRigifyLast(ts);
                    std::string o = s.GetString().ToStdString();
                    _client->Write(o.c_str(),o.size());
                }
                break;

                case 'S':
                {
                    if(_currentFrame < GetVideoPosition()->GetMax())
                    {
                        GetDisplay()->OnVideoSetFrame(_currentFrame++);
                    }
                };
                break;

                default:
                    break;
                }
            }
        }
    }
}
/*!
 * \brief MainFrame::onProperties
 * \param event
 */
void MainFrame::onProperties(wxCommandEvent& event)
{
    GetDisplay()->OnProperties();
}
/*!
 * \brief MainFrame::onVideoChanged
 * \param event
 */
void MainFrame::onVideoChanged(wxScrollEvent& event)
{
    _currentFrame = event.GetPosition();
    GetDisplay()->OnVideoSetFrame(_currentFrame);
}
/*!
 * \brief MainFrame::onSaveRecords
 * \param event
 */
void MainFrame::onSaveRecords(wxCommandEvent& event)
{
    wxString s = GetNameMap()->GetPath() + ".lmk";
    wxFFileOutputStream output( s,"w" );
    if(output.IsOk())
    {
        wxTextOutputStream ts( output );
        GetDisplay()->writeLandmarks(ts);
    }
}
/*!
 * \brief MainFrame::onFileChanged
 * \param event
 */
void MainFrame::onFileChanged(wxFileDirPickerEvent& event)
{
}
/*!
 * \brief MainFrame::onSaveReduced
 * \param event
 */
void MainFrame::onSaveReduced(wxCommandEvent& event)
{
    wxString s = GetNameMap()->GetPath() + ".red";
    wxFFileOutputStream output( s,"w" );
    if(output.IsOk())
    {
        wxTextOutputStream ts( output );
        GetDisplay()->writeRecord(ts);
    }
}
/*!
 * \brief MainFrame::onSaveRigify
 * \param event
 */
void MainFrame::onSaveRigify(wxCommandEvent& event)
{
    wxString s = GetNameMap()->GetPath() + ".rfy";
    wxFFileOutputStream output( s,"w" );
    if(output.IsOk())
    {
        wxTextOutputStream ts( output );
        GetDisplay()->writeRigify(ts);
    }
}
/*!
 * \brief MainFrame::onRecordStart
 * \param event
 */
void MainFrame::onRecordStart(wxCommandEvent& event)
{
    GetDisplay()->setRecordEnable(true);
}
/*!
 * \brief MainFrame::onRecordStop
 * \param event
 */
void MainFrame::onRecordStop(wxCommandEvent& event)
{
    GetDisplay()->setRecordEnable(false);
}
/*!
 * \brief MainFrame::onDirNameChange
 * \param event
 */
void MainFrame::onDirNameChange(wxFileDirPickerEvent& event)
{
    wxString s = GetImageDir()->GetPath();
    GetDisplay()->setImageFileName(s);
}
/*!
 * \brief MainFrame::onWriteImageChange
 * \param event
 */
void MainFrame::onWriteImageChange(wxCommandEvent& event)
{
    GetDisplay()->setWriteToFile(event.GetInt());
}
/*!
 * \brief MainFrame::onWriteBones
 * \param event
 */
void MainFrame::onWriteBones(wxCommandEvent& event)
{
    GetDisplay()->setDrawBones(event.GetInt());
}
/*!
 * \brief MainFrame::onWriteLandmarks
 * \param event
 */
void MainFrame::onWriteLandmarks(wxCommandEvent& event)
{
    GetDisplay()->setDrawLandmarks(event.GetInt());
}
/*!
 * \brief MainFrame::onSaveReference
 * \param event
 */
void MainFrame::onSaveReference(wxCommandEvent& event)
{
    wxString s = GetRestReference()->GetPath();
    GetDisplay()->writeRefRecord(s);
}

void MainFrame::onSetCurrentAsRef(wxCommandEvent& event)
{

}
/*!
 * \brief MainFrame::OnFrameStep
 * \param event
 */
void MainFrame::OnFrameStep(wxCommandEvent& event)
{
    if(_currentFrame < GetVideoPosition()->GetMax())
    {
        GetDisplay()->OnVideoSetFrame(_currentFrame++);
    }
}
/*!
 * \brief MainFrame::onSetRef
 */
void MainFrame::onSetRef(wxCommandEvent& /*event*/)
{
    // the current frame is set to the reference
    GetDisplay()->setCurrentAsReference();
}

/*!
 * \brief MainFrame::onSingleStep
 * \param event
 */
void MainFrame::onSingleStep(wxCommandEvent& event)
{
    GetSingleStep()->Enable(event.GetInt());
}
