///////////////////////////////////////////////////////////////////////////////
// Name:        ocvapp.cpp
// Purpose:     Application skeleton
// Author:      PB
// Created:     2020-09-16
// Copyright:   (c) 2020 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>

#include "ocvframe.h"
#include "MainFrame.h"
#include <wx/socket.h>

class OpenCVApp : public wxApp
{
public:
    bool OnInit() override
    {
        wxSocketBase::Initialize();
        SetVendorName("bjh");
        SetAppName("FaceTracker");
        wxInitAllImageHandlers();
        wxXmlResource::Get()->InitAllHandlers();
        SetExitOnFrameDelete (true);
        (new MainFrame(nullptr))->Show();
        return true;
    }

    int OnExit()
    {
        wxSocketBase::Shutdown();
        return wxApp::OnExit();
    }


}; wxIMPLEMENT_APP(OpenCVApp);
