/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <wx/combo.h>
#include <bitmap_types.h>
#include <bitmaps.h>
#include <kiway.h>
#include <kiway_player.h>
#include <dialog_shim.h>

#include <widgets/grid_text_button_helpers.h>
#include <eda_doc.h>


//-------- Renderer ---------------------------------------------------------------------
// None required; just render as normal text.



//-------- Editor Base Class ------------------------------------------------------------
//
// Note: this implementation is an adaptation of wxGridCellChoiceEditor


wxString GRID_CELL_TEXT_BUTTON::GetValue() const
{
    return Combo()->GetValue();
}

void GRID_CELL_TEXT_BUTTON::SetSize( const wxRect& aRect )
{
    wxRect rect( aRect );
    rect.Inflate( -1 );

#if defined( __WXMAC__ )
    rect.Inflate( 3 );      // no FOCUS_RING, even on Mac
#endif

    Combo()->SetSize( rect, wxSIZE_ALLOW_MINUS_ONE );
}


void GRID_CELL_TEXT_BUTTON::BeginEdit( int aRow, int aCol, wxGrid* aGrid )
{
    auto evtHandler = static_cast<wxGridCellEditorEvtHandler*>( m_control->GetEventHandler() );

    // Don't immediately end if we get a kill focus event within BeginEdit
    evtHandler->SetInSetFocus( true );

    m_value = aGrid->GetTable()->GetValue( aRow, aCol );

    Combo()->SetValue( m_value );
    Combo()->SetFocus();
}


bool GRID_CELL_TEXT_BUTTON::EndEdit( int , int , const wxGrid* , const wxString& , wxString *aNewVal )
{
    const wxString value = Combo()->GetValue();

    if( value == m_value )
        return false;

    m_value = value;

    if( aNewVal )
        *aNewVal = value;

    return true;
}


void GRID_CELL_TEXT_BUTTON::ApplyEdit( int aRow, int aCol, wxGrid* aGrid )
{
    aGrid->GetTable()->SetValue( aRow, aCol, m_value );
}


void GRID_CELL_TEXT_BUTTON::Reset()
{
}


class TEXT_BUTTON_FP_CHOOSER : public wxComboCtrl
{
public:
    TEXT_BUTTON_FP_CHOOSER( wxWindow* aParent, DIALOG_SHIM* aParentDlg ) :
            wxComboCtrl( aParent ),
            m_dlg( aParentDlg )
    {
        SetButtonBitmaps( KiBitmap( small_library_xpm ) );
    }

protected:
    void DoSetPopupControl( wxComboPopup* popup ) override
    {
        m_popup = nullptr;
    }

    void OnButtonClick() override
    {
        // pick a footprint using the footprint picker.
        wxString      fpid = GetValue();
        KIWAY_PLAYER* frame = m_dlg->Kiway().Player( FRAME_PCB_MODULE_VIEWER_MODAL, true, m_dlg );

        if( frame->ShowModal( &fpid, m_dlg ) )
            SetValue( fpid );

        frame->Destroy();
    }

    DIALOG_SHIM* m_dlg;
};


void GRID_CELL_FOOTPRINT_EDITOR::Create( wxWindow* aParent, wxWindowID aId,
                                         wxEvtHandler* aEventHandler )
{
    m_control = new TEXT_BUTTON_FP_CHOOSER( aParent, m_dlg );

    wxGridCellEditor::Create(aParent, aId, aEventHandler);
}


class TEXT_BUTTON_URL : public wxComboCtrl
{
public:
    TEXT_BUTTON_URL( wxWindow* aParent, DIALOG_SHIM* aParentDlg ) :
            wxComboCtrl( aParent ),
            m_dlg( aParentDlg )
    {
        SetButtonBitmaps( KiBitmap( www_xpm ) );
    }

protected:
    void DoSetPopupControl( wxComboPopup* popup ) override
    {
        m_popup = nullptr;
    }

    void OnButtonClick() override
    {
        wxString uri = ResolveUriByEnvVars( GetValue() );
        GetAssociatedDocument( m_dlg, uri );
    }

    DIALOG_SHIM* m_dlg;
};


void GRID_CELL_URL_EDITOR::Create( wxWindow* aParent, wxWindowID aId,
                                   wxEvtHandler* aEventHandler )
{
    m_control = new TEXT_BUTTON_URL( aParent, m_dlg );

    wxGridCellEditor::Create(aParent, aId, aEventHandler);
}


