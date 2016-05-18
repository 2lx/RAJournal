/*
 * dlgpassword.cpp
 *
 *  Created on: 29.09.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "dlgpassword.h"

#include "relationalmodel.h"

DlgPassword::DlgPassword( QWidget * parent )
	: QDialog( parent ),
	  m_modelSpec( new RelationalModel< RASpecialist >( this ) ),
	  m_mapper( new QDataWidgetMapper( this ) )
{
	ui.setupUi( this );

	m_modelSpec->select();
	ui.cbUser->setModel( m_modelSpec );
	ui.cbUser->setModelColumn( spFIO );

	m_mapper->setModel( m_modelSpec );
	m_mapper->setItemDelegate( new QSqlRelationalDelegate( this ) );
	m_mapper->addMapping( ui.edPassword, spPassword );

	connect( ui.cbUser, SIGNAL( currentIndexChanged( int ) ), m_mapper, SLOT( setCurrentIndex( int ) ) );
	connect( ui.btnSave, SIGNAL( clicked() ), this, SLOT( savePass() ) );
	connect( ui.btnClose, SIGNAL( clicked() ), this, SLOT( close() ) );

	m_mapper->toFirst();
}

void DlgPassword::savePass()
{
	m_modelSpec->submitAll();
	close();
}
