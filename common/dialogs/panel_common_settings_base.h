///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 30 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __PANEL_COMMON_SETTINGS_BASE_H__
#define __PANEL_COMMON_SETTINGS_BASE_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include "widgets/stepped_slider.h"
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/spinctrl.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/radiobut.h>
#include <wx/gbsizer.h>
#include <wx/slider.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class PANEL_COMMON_SETTINGS_BASE
///////////////////////////////////////////////////////////////////////////////
class PANEL_COMMON_SETTINGS_BASE : public wxPanel 
{
	private:
	
	protected:
		enum
		{
			wxID_AUTOPAN = 1000
		};
		
		wxStaticText* m_staticTextautosave;
		wxSpinCtrl* m_SaveTime;
		wxTextCtrl* m_textEditorPath;
		wxBitmapButton* m_textEditorBtn;
		wxRadioButton* m_defaultPDFViewer;
		wxRadioButton* m_otherPDFViewer;
		wxTextCtrl* m_PDFViewerPath;
		wxBitmapButton* m_pdfViewerBtn;
		wxStaticText* m_staticText1;
		STEPPED_SLIDER* m_scaleSlider;
		wxStaticText* m_staticText2;
		wxCheckBox* m_scaleAuto;
		wxCheckBox* m_checkBoxIconsInMenus;
		wxCheckBox* m_ZoomCenterOpt;
		wxCheckBox* m_MousewheelPANOpt;
		wxCheckBox* m_AutoPANOpt;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnTextEditorClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnPDFViewerClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnScaleSlider( wxScrollEvent& event ) { event.Skip(); }
		virtual void OnScaleAuto( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		PANEL_COMMON_SETTINGS_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxTAB_TRAVERSAL ); 
		~PANEL_COMMON_SETTINGS_BASE();
	
};

#endif //__PANEL_COMMON_SETTINGS_BASE_H__
