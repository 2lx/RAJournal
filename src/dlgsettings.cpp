/*
 * dlgsettings.cpp
 *
 *  Created on: 28.10.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "dlgsettings.h"

DlgSettings::DlgSettings( QWidget * parent )
	: QDialog( parent )
{
	ui.setupUi( this );

	connect( ui.btnSave, SIGNAL( clicked() ), this, SLOT( save() ) );
	connect( ui.btnCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

	QSettings appSettings( qApp->applicationName() );

	ui.leBCHost->setText( appSettings.value( "Settings.BCHost", "localhost" ).toString() );
	ui.spbBCPort->setValue( appSettings.value( "Settings.BCPort", "45454" ).toInt() );
	ui.cbBCAutoReconnect->setChecked( appSettings.value( "Settings.BCAutoReconnect", false ).toBool() );
}

void DlgSettings::save()
{
	QSettings appSettings( qApp->applicationName() );

	appSettings.setValue( "Settings.BCHost", ui.leBCHost->text() );
	appSettings.setValue( "Settings.BCPort", ui.spbBCPort->value() );
	appSettings.setValue( "Settings.BCAutoReconnect", ui.cbBCAutoReconnect->isChecked() );

	accept();
}
