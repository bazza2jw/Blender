//////////////////////////////////////////////////////////////////////
// This file was auto-generated by codelite's wxCrafter Plugin
// wxCrafter project file: FaceTracker.wxcp
// Do not modify this file by hand!
//////////////////////////////////////////////////////////////////////

#include "FaceTracker.h"


// Declare the bitmap loading function
extern void wxCrafterOtcw9wInitBitmapResources();

static bool bBitmapLoaded = false;


MainFrameBase::MainFrameBase(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxFrame(parent, id, title, pos, size, style)
{
    if ( !bBitmapLoaded ) {
        // We need to initialise the default bitmap handler
        wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
        wxCrafterOtcw9wInitBitmapResources();
        bBitmapLoaded = true;
    }
    
    boxSizer3 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(boxSizer3);
    
    m_topToolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxTB_FLAT);
    m_topToolbar->SetToolBitmapSize(wxSize(16,16));
    
    boxSizer3->Add(m_topToolbar, 0, wxALL, WXC_FROM_DIP(5));
    
    m_topToolbar->AddTool(wxID_CLOSE, _("Tool Label"), wxXmlResource::Get()->LoadBitmap(wxT("system-shutdown-restart-panel")), wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT(""), NULL);
    
    m_topToolbar->AddTool(wxID_FORWARD, _("Start"), wxXmlResource::Get()->LoadBitmap(wxT("package-installed-updated")), wxNullBitmap, wxITEM_NORMAL, _("Start Processing"), wxT(""), NULL);
    
    m_topToolbar->AddTool(wxID_CLEAR, _("Stop"), wxXmlResource::Get()->LoadBitmap(wxT("package-purge")), wxNullBitmap, wxITEM_NORMAL, _("Stop Processing"), wxT(""), NULL);
    
    m_staticText37 = new wxStaticText(m_topToolbar, wxID_ANY, _("Source:"), wxDefaultPosition, wxDLG_UNIT(m_topToolbar, wxSize(-1,-1)), 0);
    m_topToolbar->AddControl(m_staticText37);
    
    wxArrayString m_sourceArr;
    m_sourceArr.Add(wxT("Empty"));
    m_sourceArr.Add(wxT("Image"));
    m_sourceArr.Add(wxT("Video"));
    m_sourceArr.Add(wxT("WebCam"));
    m_sourceArr.Add(wxT("IPCamera"));
    m_source = new wxChoice(m_topToolbar, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_topToolbar, wxSize(-1,-1)), m_sourceArr, 0);
    m_source->SetSelection(0);
    m_topToolbar->AddControl(m_source);
    
    m_topToolbar->AddTool(wxID_PREFERENCES, _("Tool Label"), wxArtProvider::GetBitmap(wxART_REPORT_VIEW, wxART_TOOLBAR, wxDefaultSize), wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT(""), NULL);
    
    m_topToolbar->AddTool(wxID_SAVE, _("Tool Label"), wxXmlResource::Get()->LoadBitmap(wxT("media-record")), wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT(""), NULL);
    
    m_topToolbar->AddTool(wxID_UNDO, _("Tool Label"), wxXmlResource::Get()->LoadBitmap(wxT("media-playback-stop")), wxNullBitmap, wxITEM_NORMAL, wxT(""), wxT(""), NULL);
    
    m_writeImage = new wxCheckBox(m_topToolbar, wxID_ANY, _("Write Image"), wxDefaultPosition, wxDLG_UNIT(m_topToolbar, wxSize(-1,-1)), 0);
    m_writeImage->SetValue(false);
    m_topToolbar->AddControl(m_writeImage);
    
    m_writeLandmarks = new wxCheckBox(m_topToolbar, wxID_ANY, _("Draw Landmarks"), wxDefaultPosition, wxDLG_UNIT(m_topToolbar, wxSize(-1,-1)), 0);
    m_writeLandmarks->SetValue(false);
    m_topToolbar->AddControl(m_writeLandmarks);
    
    m_writeBones = new wxCheckBox(m_topToolbar, wxID_ANY, _("Draw Bones"), wxDefaultPosition, wxDLG_UNIT(m_topToolbar, wxSize(-1,-1)), 0);
    m_writeBones->SetValue(false);
    m_topToolbar->AddControl(m_writeBones);
    
    m_singleStepEnable = new wxCheckBox(m_topToolbar, wxID_ANY, _("Single Step"), wxDefaultPosition, wxDLG_UNIT(m_topToolbar, wxSize(-1,-1)), 0);
    m_singleStepEnable->SetValue(false);
    m_topToolbar->AddControl(m_singleStepEnable);
    m_topToolbar->Realize();
    
    m_Pages = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1,-1)), wxBK_DEFAULT);
    m_Pages->SetName(wxT("m_Pages"));
    
    boxSizer3->Add(m_Pages, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_panel21 = new wxPanel(m_Pages, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_Pages, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_Pages->AddPage(m_panel21, _("Image"), false);
    
    MainPageSizer = new wxBoxSizer(wxVERTICAL);
    m_panel21->SetSizer(MainPageSizer);
    
    m_Display = new OpenCVFrame(m_panel21, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_panel21, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    
    MainPageSizer->Add(m_Display, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    flexGridSizer129 = new wxFlexGridSizer(0, 3, 0, 0);
    flexGridSizer129->SetFlexibleDirection( wxBOTH );
    flexGridSizer129->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    flexGridSizer129->AddGrowableCol(0);
    
    MainPageSizer->Add(flexGridSizer129, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_videoPosition = new wxSlider(m_panel21, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDLG_UNIT(m_panel21, wxSize(-1,-1)), wxSL_HORIZONTAL);
    
    flexGridSizer129->Add(m_videoPosition, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_SingleStep = new wxButton(m_panel21, wxID_ANY, _("Step"), wxDefaultPosition, wxDLG_UNIT(m_panel21, wxSize(-1,-1)), 0);
    m_SingleStep->Enable(false);
    
    flexGridSizer129->Add(m_SingleStep, 0, wxALL, WXC_FROM_DIP(5));
    
    m_button133 = new wxButton(m_panel21, wxID_ANY, _("Set Ref."), wxDefaultPosition, wxDLG_UNIT(m_panel21, wxSize(-1,-1)), 0);
    
    flexGridSizer129->Add(m_button133, 0, wxALL, WXC_FROM_DIP(5));
    
    m_panel39 = new wxPanel(m_Pages, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_Pages, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_Pages->AddPage(m_panel39, _("File"), false);
    
    boxSizer41 = new wxBoxSizer(wxVERTICAL);
    m_panel39->SetSizer(boxSizer41);
    
    m_staticText79 = new wxStaticText(m_panel39, wxID_ANY, _("Track Data Output File Base"), wxDefaultPosition, wxDLG_UNIT(m_panel39, wxSize(-1,-1)), 0);
    
    boxSizer41->Add(m_staticText79, 0, wxALL, WXC_FROM_DIP(5));
    
    m_nameMap = new wxFilePickerCtrl(m_panel39, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*"), wxDefaultPosition, wxDLG_UNIT(m_panel39, wxSize(-1,-1)), wxFLP_USE_TEXTCTRL|wxFLP_SMALL|wxFLP_SAVE);
    
    boxSizer41->Add(m_nameMap, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    flexGridSizer57 = new wxFlexGridSizer(0, 3, 0, 0);
    flexGridSizer57->SetFlexibleDirection( wxBOTH );
    flexGridSizer57->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    
    boxSizer41->Add(flexGridSizer57, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button61 = new wxButton(m_panel39, wxID_ANY, _("Save Landmarks"), wxDefaultPosition, wxDLG_UNIT(m_panel39, wxSize(-1,-1)), 0);
    
    flexGridSizer57->Add(m_button61, 0, wxALL, WXC_FROM_DIP(5));
    
    m_button85 = new wxButton(m_panel39, wxID_ANY, _("Save Reduced"), wxDefaultPosition, wxDLG_UNIT(m_panel39, wxSize(-1,-1)), 0);
    
    flexGridSizer57->Add(m_button85, 0, wxALL, WXC_FROM_DIP(5));
    
    m_button87 = new wxButton(m_panel39, wxID_ANY, _("Save Rigify"), wxDefaultPosition, wxDLG_UNIT(m_panel39, wxSize(-1,-1)), 0);
    
    flexGridSizer57->Add(m_button87, 0, wxALL, WXC_FROM_DIP(5));
    
    m_staticText93 = new wxStaticText(m_panel39, wxID_ANY, _("Image Sequence Directory"), wxDefaultPosition, wxDLG_UNIT(m_panel39, wxSize(-1,-1)), 0);
    
    boxSizer41->Add(m_staticText93, 0, wxALL, WXC_FROM_DIP(5));
    
    m_imageDir = new wxDirPickerCtrl(m_panel39, wxID_ANY, wxEmptyString, _("Select a folder"), wxDefaultPosition, wxDLG_UNIT(m_panel39, wxSize(-1,-1)), wxDIRP_SMALL|wxDIRP_DEFAULT_STYLE|wxDIRP_USE_TEXTCTRL);
    
    boxSizer41->Add(m_imageDir, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_staticText119 = new wxStaticText(m_panel39, wxID_ANY, _("Rest Reference"), wxDefaultPosition, wxDLG_UNIT(m_panel39, wxSize(-1,-1)), 0);
    
    boxSizer41->Add(m_staticText119, 0, wxALL, WXC_FROM_DIP(5));
    
    m_restReference = new wxFilePickerCtrl(m_panel39, wxID_ANY, wxEmptyString, _("Select a file"), wxT("*.rfy"), wxDefaultPosition, wxDLG_UNIT(m_panel39, wxSize(-1,-1)), wxFLP_DEFAULT_STYLE|wxFLP_USE_TEXTCTRL|wxFLP_SMALL|wxFLP_OPEN);
    
    boxSizer41->Add(m_restReference, 0, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    gridSizer125 = new wxGridSizer(0, 2, 0, 0);
    
    boxSizer41->Add(gridSizer125, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_button127 = new wxButton(m_panel39, wxID_ANY, _("Save Ref."), wxDefaultPosition, wxDLG_UNIT(m_panel39, wxSize(-1,-1)), 0);
    
    gridSizer125->Add(m_button127, 0, wxALL, WXC_FROM_DIP(5));
    
    m_panel113 = new wxPanel(m_Pages, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(m_Pages, wxSize(-1,-1)), wxTAB_TRAVERSAL);
    m_Pages->AddPage(m_panel113, _("Data"), false);
    
    boxSizer115 = new wxBoxSizer(wxVERTICAL);
    m_panel113->SetSizer(boxSizer115);
    
    m_actionData = new wxTextCtrl(m_panel113, wxID_ANY, wxT(""), wxDefaultPosition, wxDLG_UNIT(m_panel113, wxSize(-1,-1)), wxTE_MULTILINE);
    
    boxSizer115->Add(m_actionData, 1, wxALL|wxEXPAND, WXC_FROM_DIP(5));
    
    m_frameTimer = new wxTimer;
    m_frameTimer->Start(100, false);
    
    
    #if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(m_Pages)){
        wxPersistenceManager::Get().RegisterAndRestore(m_Pages);
    } else {
        wxPersistenceManager::Get().Restore(m_Pages);
    }
    #endif
    
    SetName(wxT("MainFrameBase"));
    SetSize(wxDLG_UNIT(this, wxSize(800,600)));
    if (GetSizer()) {
         GetSizer()->Fit(this);
    }
    if(GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }
#if wxVERSION_NUMBER >= 2900
    if(!wxPersistenceManager::Get().Find(this)) {
        wxPersistenceManager::Get().RegisterAndRestore(this);
    } else {
        wxPersistenceManager::Get().Restore(this);
    }
#endif
    // Connect events
    this->Connect(wxID_CLOSE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onQuit), NULL, this);
    this->Connect(wxID_FORWARD, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onStart), NULL, this);
    this->Connect(wxID_CLEAR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onStop), NULL, this);
    this->Connect(wxID_PREFERENCES, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onProperties), NULL, this);
    this->Connect(wxID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onRecordStart), NULL, this);
    this->Connect(wxID_UNDO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onRecordStop), NULL, this);
    m_writeImage->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrameBase::onWriteImageChange), NULL, this);
    m_writeLandmarks->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrameBase::onWriteLandmarks), NULL, this);
    m_writeBones->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrameBase::onWriteBones), NULL, this);
    m_singleStepEnable->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrameBase::onSingleStep), NULL, this);
    m_videoPosition->Connect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(MainFrameBase::onVideoChanged), NULL, this);
    m_SingleStep->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::OnFrameStep), NULL, this);
    m_button133->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::onSetRef), NULL, this);
    m_nameMap->Connect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(MainFrameBase::onFileChanged), NULL, this);
    m_button61->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::onSaveRecords), NULL, this);
    m_button85->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::onSaveReduced), NULL, this);
    m_button87->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::onSaveRigify), NULL, this);
    m_imageDir->Connect(wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(MainFrameBase::onDirNameChange), NULL, this);
    m_button127->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::onSaveReference), NULL, this);
    m_frameTimer->Connect(wxEVT_TIMER, wxTimerEventHandler(MainFrameBase::onTimer), NULL, this);
    
}

MainFrameBase::~MainFrameBase()
{
    this->Disconnect(wxID_CLOSE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onQuit), NULL, this);
    this->Disconnect(wxID_FORWARD, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onStart), NULL, this);
    this->Disconnect(wxID_CLEAR, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onStop), NULL, this);
    this->Disconnect(wxID_PREFERENCES, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onProperties), NULL, this);
    this->Disconnect(wxID_SAVE, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onRecordStart), NULL, this);
    this->Disconnect(wxID_UNDO, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(MainFrameBase::onRecordStop), NULL, this);
    m_writeImage->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrameBase::onWriteImageChange), NULL, this);
    m_writeLandmarks->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrameBase::onWriteLandmarks), NULL, this);
    m_writeBones->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrameBase::onWriteBones), NULL, this);
    m_singleStepEnable->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(MainFrameBase::onSingleStep), NULL, this);
    m_videoPosition->Disconnect(wxEVT_SCROLL_CHANGED, wxScrollEventHandler(MainFrameBase::onVideoChanged), NULL, this);
    m_SingleStep->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::OnFrameStep), NULL, this);
    m_button133->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::onSetRef), NULL, this);
    m_nameMap->Disconnect(wxEVT_COMMAND_FILEPICKER_CHANGED, wxFileDirPickerEventHandler(MainFrameBase::onFileChanged), NULL, this);
    m_button61->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::onSaveRecords), NULL, this);
    m_button85->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::onSaveReduced), NULL, this);
    m_button87->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::onSaveRigify), NULL, this);
    m_imageDir->Disconnect(wxEVT_COMMAND_DIRPICKER_CHANGED, wxFileDirPickerEventHandler(MainFrameBase::onDirNameChange), NULL, this);
    m_button127->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(MainFrameBase::onSaveReference), NULL, this);
    m_frameTimer->Disconnect(wxEVT_TIMER, wxTimerEventHandler(MainFrameBase::onTimer), NULL, this);
    
    m_frameTimer->Stop();
    wxDELETE( m_frameTimer );

}