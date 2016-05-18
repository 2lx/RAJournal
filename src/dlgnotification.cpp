/*
 * dlgnotification.cpp
 *
 *  Created on: 28.10.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "dlgnotification.h"

#include "relationalmodel.h"

DlgNotification::DlgNotification( QWidget * parent )
	: QDialog( parent ),
	m_modelNotification( new RelationalModel< RANotification >( this ) ),
	m_modelProfession( new RelationalModel< RAProfession >( this ) )
{
	ui.setupUi( this );

	m_modelNotification->select();
	m_modelProfession->select();

	ui.tvNotification->setModel( m_modelNotification );
	ui.tvNotification->setItemDelegate( new QSqlRelationalDelegate( ui.tvNotification ) );
	ui.tvNotification->hideColumn( noID );

	ui.tvProfession->setModel( m_modelProfession );
	ui.tvProfession->setItemDelegate( new QSqlRelationalDelegate( ui.tvProfession ) );
	ui.tvProfession->hideColumn( prID );

	connect( ui.btnSave, SIGNAL( clicked() ), this, SLOT( saveChanges() ) );
	connect( ui.btnClose, SIGNAL( clicked() ), this, SLOT( close() ) );

	loadAppSettings();
}

void DlgNotification::loadAppSettings()
{
	QSettings appSettings( qApp->applicationName() );

	resize( appSettings.value( "Notification.Width", width() ).toInt(),
	        appSettings.value( "Notification.Height", height() ).toInt() );

	appSettings.beginGroup( "NotificationColumnsWidth" );
	for ( int i = 0; i < noColumnCount; i++ )
		ui.tvNotification->setColumnWidth( i, appSettings.value( QString::number( i ), 120 ).toInt() );
	appSettings.endGroup();

	appSettings.beginGroup( "ProfessionColumnsWidth" );
	for ( int i = 0; i < prColumnCount; i++ )
		ui.tvProfession->setColumnWidth( i, appSettings.value( QString::number( i ), 120 ).toInt() );
	appSettings.endGroup();
}

DlgNotification::~DlgNotification()
{
	QSettings appSettings( qApp->applicationName() );

	appSettings.setValue( "Notification.Width", normalGeometry().width() );
	appSettings.setValue( "Notification.Height", normalGeometry().height() );

	appSettings.beginGroup( "NotificationColumnsWidth" );
	for ( int i = 0; i < noColumnCount; i++ )
		appSettings.setValue( QString::number( i ), ui.tvNotification->columnWidth( i ) );
	appSettings.endGroup();

	appSettings.beginGroup( "ProfessionColumnsWidth" );
	for ( int i = 0; i < prColumnCount; i++ )
		appSettings.setValue( QString::number( i ), ui.tvProfession->columnWidth( i ) );
	appSettings.endGroup();
}

void DlgNotification::saveChanges()
{
	m_modelNotification->submitAll();
	m_modelProfession->submitAll();
	close();
}
