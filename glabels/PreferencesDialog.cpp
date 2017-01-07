/*  PreferencesDialog.cpp
 *
 *  Copyright (C) 2016  Jim Evins <evins@snaught.com>
 *
 *  This file is part of gLabels-qt.
 *
 *  gLabels-qt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gLabels-qt is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gLabels-qt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PreferencesDialog.h"


#include "Settings.h"


///
/// Constructor
///
PreferencesDialog::PreferencesDialog( QWidget *parent )
	: QDialog(parent)
{
	setupUi( this );

	switch ( Settings::units().toEnum() )
	{
	case glabels::Units::IN:
		unitsInchesRadio->setChecked( true );
		break;
	case glabels::Units::MM:
		unitsMillimetersRadio->setChecked( true );
		break;
	case glabels::Units::CM:
		unitsCentimetersRadio->setChecked( true );
		break;
	case glabels::Units::PC:
		unitsPicasRadio->setChecked( true );
		break;
	default:
		unitsPointsRadio->setChecked( true );
		break;
	}
}


///
/// Units Radios Changed
///
void PreferencesDialog::onUnitsRadiosChanged()
{
	if ( unitsInchesRadio->isChecked() )
	{
		Settings::setUnits( glabels::Units::in() );
	}
	else if ( unitsMillimetersRadio->isChecked() )
	{
		Settings::setUnits( glabels::Units::mm() );
	}
	else if ( unitsCentimetersRadio->isChecked() )
	{
		Settings::setUnits( glabels::Units::cm() );
	}
	else if ( unitsPicasRadio->isChecked() )
	{
		Settings::setUnits( glabels::Units::pc() );
	}
	else
	{
		Settings::setUnits( glabels::Units::pt() );
	}
}
