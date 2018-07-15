/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Jean-Pierre Charras jp.charras at wanadoo.fr
 * Copyright (C) 1992-2018 KiCad Developers, see AUTHORS.txt for contributors.
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

/*
 * Edit properties of Lines, Circles, Arcs and Polygons for PCBNew and ModEdit
 */

#include <fctsys.h>
#include <macros.h>
#include <confirm.h>
#include <class_drawpanel.h>
#include <pcb_base_edit_frame.h>
#include <wx/valnum.h>
#include <board_commit.h>
#include <pcb_layer_box_selector.h>
#include <html_messagebox.h>
#include <class_board.h>
#include <class_drawsegment.h>
#include <class_edge_mod.h>
#include <widgets/unit_binder.h>

#include <dialog_graphic_item_properties_base.h>

class DIALOG_GRAPHIC_ITEM_PROPERTIES : public DIALOG_GRAPHIC_ITEM_PROPERTIES_BASE
{
private:
    PCB_BASE_EDIT_FRAME*  m_parent;
    wxDC*                 m_DC;
    DRAWSEGMENT*          m_item;
    EDGE_MODULE*          m_moduleItem;

    UNIT_BINDER           m_startX, m_startY;
    UNIT_BINDER           m_endX, m_endY;
    UNIT_BINDER           m_angle;
    UNIT_BINDER           m_thickness;

    wxFloatingPointValidator<double>    m_AngleValidator;
    double                m_AngleValue;

public:
    DIALOG_GRAPHIC_ITEM_PROPERTIES( PCB_BASE_EDIT_FRAME* aParent, BOARD_ITEM* aItem, wxDC* aDC );
    ~DIALOG_GRAPHIC_ITEM_PROPERTIES() {};

private:
    bool TransferDataToWindow() override;
    bool TransferDataFromWindow() override;

    void OnInitDlg( wxInitDialogEvent& event ) override
    {
        // Call the default wxDialog handler of a wxInitDialogEvent
        TransferDataToWindow();

        // Now all widgets have the size fixed, call FinishDialogSettings
        FinishDialogSettings();
    }

    bool Validate() override;
};

DIALOG_GRAPHIC_ITEM_PROPERTIES::DIALOG_GRAPHIC_ITEM_PROPERTIES( PCB_BASE_EDIT_FRAME* aParent,
                                                                BOARD_ITEM* aItem, wxDC* aDC ):
    DIALOG_GRAPHIC_ITEM_PROPERTIES_BASE( aParent ),
    m_startX( aParent, m_startXLabel, m_startXCtrl, m_startXUnits ),
    m_startY( aParent, m_startYLabel, m_startYCtrl, m_startYUnits ),
    m_endX( aParent, m_endXLabel, m_endXCtrl, m_endXUnits ),
    m_endY( aParent, m_endYLabel, m_endYCtrl, m_endYUnits ),
    m_angle( aParent, m_angleLabel, m_angleCtrl, m_angleUnits ),
    m_thickness( aParent, m_thicknessLabel, m_thicknessCtrl, m_thicknessUnits, true ),
    m_AngleValidator( 1, &m_AngleValue ),
    m_AngleValue( 0.0 )
{
    m_parent = aParent;
    m_DC = aDC;
    m_item = dynamic_cast<DRAWSEGMENT*>( aItem );
    m_moduleItem = dynamic_cast<EDGE_MODULE*>( aItem );

    m_angle.SetUnits( DEGREES );
    m_AngleValidator.SetRange( -360.0, 360.0 );
    m_angleCtrl->SetValidator( m_AngleValidator );
    m_AngleValidator.SetWindow( m_angleCtrl );

    SetInitialFocus( m_startXCtrl );

    m_StandardButtonsSizerOK->SetDefault();
}


void PCB_BASE_EDIT_FRAME::InstallGraphicItemPropertiesDialog( BOARD_ITEM* aItem, wxDC* aDC )
{
    wxCHECK_RET( aItem != NULL, wxT( "InstallGraphicItemPropertiesDialog() error: NULL item" ) );

#ifdef USE_WX_OVERLAY
    // #1277772 - Draw into dialog converted in refresh request
    aDC = nullptr;
#endif

    m_canvas->SetIgnoreMouseEvents( true );
    DIALOG_GRAPHIC_ITEM_PROPERTIES dlg( this, aItem, aDC );
    dlg.ShowModal();
    m_canvas->MoveCursorToCrossHair();
    m_canvas->SetIgnoreMouseEvents( false );

    if( !aDC )
        m_canvas->Refresh();
}


bool DIALOG_GRAPHIC_ITEM_PROPERTIES::TransferDataToWindow()
{
    if( !m_item )
        return false;

    // Only an arc has a angle parameter. So do not show this parameter for other shapes
    if( m_item->GetShape() != S_ARC )
        m_angle.Show( false );

    // Change texts according to the segment shape:
    switch( m_item->GetShape() )
    {
    case S_CIRCLE:
        SetTitle( _( "Circle Properties" ) );
        m_startXLabel->SetLabel( _( "Center X:" ) );
        m_startYLabel->SetLabel( _( "Center Y:" ) );
        m_endXLabel->SetLabel( _( "Radius:" ) );
        m_endY.Show( false );
        break;

    case S_ARC:
        SetTitle( _( "Arc Properties" ) );
        m_startXLabel->SetLabel( _( "Center X:" ) );
        m_startYLabel->SetLabel( _( "Center Y:" ) );
        m_endXLabel->SetLabel( _( "Start Point X:" ) );
        m_endYLabel->SetLabel( _( "Start Point Y:" ) );

        m_AngleValue = m_item->GetAngle() / 10.0;
        break;

    case S_POLYGON:
        SetTitle( _( "Polygon Properties" ) );
        m_fgUpperLeftGridSizer->Show( false );
        break;

    case S_SEGMENT:
        SetTitle( _( "Line Segment Properties" ) );
        break;

    default:
        break;
    }

    m_startX.SetValue( m_item->GetStart().x );
    m_startY.SetValue( m_item->GetStart().y );

    if(  m_item->GetShape() == S_CIRCLE )
    {
        m_endX.SetValue( m_item->GetRadius() );
    }
    else
    {
        m_endX.SetValue( m_item->GetEnd().x );
        m_endY.SetValue( m_item->GetEnd().y );
    }

    m_thickness.SetValue( m_item->GetWidth() );

    // Configure the layers list selector
    if( m_moduleItem )
        m_LayerSelectionCtrl->SetNotAllowedLayerSet( LSET::InternalCuMask().set( Edge_Cuts ) );
    else
        m_LayerSelectionCtrl->SetNotAllowedLayerSet( LSET::AllCuMask() );

    m_LayerSelectionCtrl->SetLayersHotkeys( false );
    m_LayerSelectionCtrl->SetBoardFrame( m_parent );
    m_LayerSelectionCtrl->Resync();

    if( m_LayerSelectionCtrl->SetLayerSelection( m_item->GetLayer() ) < 0 )
    {
        wxMessageBox( _( "This item was on a forbidden or non-existing layer.\n"
                         "It has been moved to the first allowed layer." ) );
        m_LayerSelectionCtrl->SetSelection( F_SilkS );
    }

    return DIALOG_GRAPHIC_ITEM_PROPERTIES_BASE::TransferDataToWindow();
}


bool DIALOG_GRAPHIC_ITEM_PROPERTIES::TransferDataFromWindow()
{
    if( !DIALOG_GRAPHIC_ITEM_PROPERTIES_BASE::TransferDataFromWindow() )
        return false;

    LAYER_NUM layer = m_LayerSelectionCtrl->GetLayerSelection();

    if( IsCopperLayer( layer ) )
    {
        // An graphic item is put on a copper layer.
        // This is sometimes useful, for instance for microwave applications and net tees.
        // However, because the DRC does not handle graphic items, it can break boards.
        // Therefore a confirmation is required.
        if( !IsOK( this, _( "The graphic item will be on a copper layer.\n"
                            "This is very dangerous because DRC does not handle it.\n"
                            "Are you sure?" ) ) )
            return false;
    }

    BOARD_COMMIT commit( m_parent );
    commit.Modify( m_item );

    wxString msg;

    if( m_DC )
        m_item->Draw( m_parent->GetCanvas(), m_DC, GR_XOR );

    m_item->SetStartX( m_startX.GetValue() );
    m_item->SetStartY( m_startY.GetValue() );

    if( m_item->GetShape() == S_CIRCLE )
    {
        m_item->SetEnd( m_item->GetStart() + wxPoint( m_endX.GetValue(), 0 ) );
    }
    else
    {
        m_item->SetEndX( m_endX.GetValue() );
        m_item->SetEndY( m_endY.GetValue() );
    }

    if( m_moduleItem )
    {
        m_moduleItem->SetStart0( m_moduleItem->GetStart() );
        m_moduleItem->SetEnd0( m_moduleItem->GetEnd() );
    }

    m_item->SetWidth( m_thickness.GetValue() );
    m_item->SetLayer( ToLAYER_ID( layer ) );

    if( m_item->GetShape() == S_ARC )
    {
        m_item->SetAngle( m_AngleValue * 10.0 );
    }

    commit.Push( _( "Modify drawing properties" ) );

    if( m_DC )
        m_item->Draw( m_parent->GetCanvas(), m_DC, GR_OR );

    m_parent->SetMsgPanel( m_item );

    return true;
}


bool DIALOG_GRAPHIC_ITEM_PROPERTIES::Validate()
{
    wxArrayString error_msgs;

    if( !DIALOG_GRAPHIC_ITEM_PROPERTIES_BASE::Validate() )
        return false;

    // Type specific checks.
    switch( m_item->GetShape() )
    {
    case S_ARC:
        // Check angle of arc.
        if( m_angle.GetValue() == 0 )
            error_msgs.Add( _( "The arc angle cannot be zero." ) );
        // Fall through.

    case S_CIRCLE:
        // Check radius.
        if( m_startX.GetValue() == m_endX.GetValue() && m_startY.GetValue() == m_endY.GetValue() )
            error_msgs.Add( _( "The radius must be greater than zero." ) );
        break;

    case S_POLYGON:
        break;

    default:
        // Check start and end are not the same.
        if( m_startX.GetValue() == m_endX.GetValue() && m_startY.GetValue() == m_endY.GetValue() )
            error_msgs.Add( _( "The start and end points cannot be the same." ) );
        break;
    }

    // Check the item thickness. Note the polygon outline thickness is allowed
    // to be set to 0, because if the shape is exactly the polygon, its outline
    // thickness must be 0
    int thickness = m_thickness.GetValue();

    if( m_item->GetShape() == S_POLYGON )
    {
        if( thickness < 0 )
            error_msgs.Add( _( "The polygon outline thickness must be >= 0." ) );
    }
    else
    {
        if( thickness <= 0 )
            error_msgs.Add( _( "The item thickness must be greater than zero." ) );
    }

    if( error_msgs.GetCount() )
    {
        HTML_MESSAGE_BOX dlg( this, _( "Error List" ) );
        dlg.ListSet( error_msgs );
        dlg.ShowModal();
    }

    return error_msgs.GetCount() == 0;
}
