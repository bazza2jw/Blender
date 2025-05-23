///////////////////////////////////////////////////////////////////////////////
// Name:        bmpfromocvpanel.cpp
// Purpose:     Displays a wxBitmap originated from OpenCV
// Author:      PB
// Created:     2020-09-16
// Copyright:   (c) 2020 PB
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/colordlg.h>
#include <wx/dcbuffer.h>
#include <wx/fontdlg.h>

#include "bmpfromocvpanel.h"

wxBitmapFromOpenCVPanel::wxBitmapFromOpenCVPanel(wxWindow* parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long)
    : wxScrolledCanvas(parent, wxID_ANY, pos, size, wxFULL_REPAINT_ON_RESIZE)
{
    m_overlayTextColour = *wxGREEN;
    m_overlayFont = GetFont();

    SetBackgroundColour(*wxBLACK);
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    SetScrollRate(FromDIP(8), FromDIP(8));
    // We need to do this to prevent drawing artefacts
    // due to the info "overlay" which does not scroll with the bitmap.
    EnableScrolling(false, false);

    Bind(wxEVT_PAINT, &wxBitmapFromOpenCVPanel::OnPaint, this);

    Bind(wxEVT_LEFT_DCLICK, &wxBitmapFromOpenCVPanel::OnChangeOverlayTextColour, this);
    Bind(wxEVT_RIGHT_DCLICK, &wxBitmapFromOpenCVPanel::OnChangeOverlayFont, this);
}

bool wxBitmapFromOpenCVPanel::SetBitmap(const wxBitmap& bitmap, const long timeGet, const long timeConvert)
{
    m_bitmap = bitmap;

    if ( m_bitmap.IsOk() )
    {
        if ( m_bitmap.GetSize() != GetVirtualSize() )
        {
            InvalidateBestSize();
            SetVirtualSize(m_bitmap.GetSize());
        }
    }
    else
    {
        InvalidateBestSize();
        SetVirtualSize(1, 1);
    }

    m_timeGetCVBitmap = timeGet;
    m_timeConvertBitmap = timeConvert;

    Refresh(); Update();
    return true;
}

wxSize wxBitmapFromOpenCVPanel::DoGetBestClientSize() const
{
    if ( !m_bitmap.IsOk() )
        return FromDIP(wxSize(64, 48)); // completely arbitrary

    return m_bitmap.GetSize();
}

void wxBitmapFromOpenCVPanel::OnPaint(wxPaintEvent&)
{
    wxAutoBufferedPaintDC dc(this);

    dc.Clear();

    if ( !m_bitmap.IsOk() )
        return;

    const wxSize clientSize = GetClientSize();
    wxPoint      offset = GetViewStart();
    int          pixelsPerUnitX = 0, pixelsPerUnitY = 0;
    wxStopWatch  stopWatch;

    stopWatch.Start();

    DoPrepareDC(dc);

    dc.DrawBitmap(m_bitmap, 0, 0, false);

    GetScrollPixelsPerUnit(&pixelsPerUnitX, &pixelsPerUnitY);
    offset.x *= pixelsPerUnitX; offset.y *= pixelsPerUnitY;

}


void wxBitmapFromOpenCVPanel::OnChangeOverlayTextColour(wxMouseEvent&)
{
    const wxColour colour = wxGetColourFromUser(this, m_overlayTextColour,
                                "Color for text overlay");

    if ( !colour.IsOk() )
        return;

    m_overlayTextColour = colour;
    Refresh(); Update();
}

void wxBitmapFromOpenCVPanel::OnChangeOverlayFont(wxMouseEvent&)
{
    const wxFont font = wxGetFontFromUser(this, m_overlayFont,
                            "Font for text overlay");

    if ( !font.IsOk() )
        return;

    m_overlayFont = font;
    Refresh(); Update();
}
