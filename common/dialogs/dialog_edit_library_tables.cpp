/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <wx/statline.h>

#include "dialog_edit_library_tables.h"


DIALOG_EDIT_LIBRARY_TABLES::DIALOG_EDIT_LIBRARY_TABLES( wxWindow* aParent,
                                                        const wxString& aTitle ) :
        DIALOG_SHIM( aParent, wxID_ANY, aTitle, wxDefaultPosition, wxDefaultSize,
                     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER ),
        m_GlobalTableChanged( false ),
        m_ProjectTableChanged( false )
{
    // Construction delayed until after panel is installed
}


void DIALOG_EDIT_LIBRARY_TABLES::InstallPanel( wxPanel* aPanel )
{
    m_contentPanel = aPanel;

    // Now perform the body of the constructor
    auto mainSizer = new wxBoxSizer( wxVERTICAL );
    SetSizer( mainSizer );

    mainSizer->Add( m_contentPanel, 1, wxEXPAND|wxALL, 5 );
    m_contentPanel->SetMinSize( wxSize( 1000, 600 ) );

    auto line = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    mainSizer->Add( line, 0, wxEXPAND|wxLEFT|wxTOP|wxRIGHT, 10 );

    auto sdbSizer = new wxStdDialogButtonSizer();
    auto sdbSizerOK = new wxButton( this, wxID_OK );
    sdbSizer->AddButton( sdbSizerOK );
    auto sdbSizerCancel = new wxButton( this, wxID_CANCEL );
    sdbSizer->AddButton( sdbSizerCancel );
    sdbSizer->Realize();

    mainSizer->Add( sdbSizer, 0, wxALL|wxEXPAND, 5 );

    sdbSizerOK->SetDefault();

    FinishDialogSettings();

    // On some windows manager (Unity, XFCE), this dialog is not always raised, depending
    // on how the dialog is run.
    Raise();
}


bool DIALOG_EDIT_LIBRARY_TABLES::TransferDataToWindow()
{
    if( !DIALOG_SHIM::TransferDataToWindow() )
        return false;

    return m_contentPanel->TransferDataToWindow();
}


bool DIALOG_EDIT_LIBRARY_TABLES::TransferDataFromWindow()
{
    if( !DIALOG_SHIM::TransferDataFromWindow() )
        return false;

    return m_contentPanel->TransferDataFromWindow();
}

