/*
 * dlglogin.cpp
 *
 *  Created on: 29.09.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "dlglogin.h"
#include "database.h"

DlgLogin::DlgLogin( QWidget * parent )
	: QDialog( parent ),
	  p_modelSpec( new QSqlQueryModel( this ) )
{
	ui.setupUi( this );

	connect( ui.btnConnect, SIGNAL( toggled( bool ) ), ui.edHost, SLOT( setDisabled( bool ) ) );
	connect( ui.btnApply, SIGNAL( clicked() ), this, SLOT( checkPass() ) );
	connect( ui.btnClose, SIGNAL( clicked() ), this, SLOT( close() ) );
	connect( ui.btnConnect, SIGNAL( toggled( bool ) ), this, SLOT( connectToDB( bool ) ) );

	QSettings appSettings( qApp->applicationName() );
	ui.edHost->setText( appSettings.value( "Login.Host", "localhost" ).toString() );

	connectToDB( false );
    QIcon icon( ":/raicon.png" );
    setWindowIcon( icon );
}

void DlgLogin::closeEvent ( QCloseEvent * event )
{
	QSettings appSettings( qApp->applicationName() );
	appSettings.setValue( "Login.Host", ui.edHost->text() );
	appSettings.setValue( "Login.User", ui.cbUser->currentIndex() );

	QDialog::closeEvent( event );
}

void DlgLogin::connectToDB( bool isAdmin )
{
	if ( !ui.btnConnect->isChecked() )
	{
		RADB::discardPSQLConnection();
		return;
	}

	QString strLogin = isAdmin ? "raadmin" : "rauser";
	QString strPassword = isAdmin ? "redanchoradmin" : "user";

    if ( RADB::setPSQLConnection( ui.edHost->text().trimmed(), strLogin, strPassword ) )
    {
    	p_modelSpec->setQuery( RADB::loginQuery() );
    	if ( p_modelSpec->lastError().isValid() )
    		QMessageBox::about(0, "", p_modelSpec->lastError().databaseText() );

 		ui.cbUser->setModel( p_modelSpec );
		ui.cbUser->setModelColumn( 1 );

		QSettings appSettings( qApp->applicationName() );
		ui.cbUser->setCurrentIndex( appSettings.value( "Login.User", 0 ).toInt() );
    }
	else ui.btnConnect->setChecked( false );
}

void DlgLogin::checkPass()
{
	QSettings appSettings( qApp->applicationName() );
	appSettings.setValue( "Login.Host", ui.edHost->text() );
	appSettings.setValue( "Login.User", ui.cbUser->currentIndex() );

	QString strHash = QString::fromAscii( QCryptographicHash::hash(
			ui.edPassword->text().toAscii(), QCryptographicHash::Md5 ).toHex() );

	if ( strHash == p_modelSpec->record( ui.cbUser->currentIndex() ).value( "spPassword" ).toString() )
	{
		p_userId = p_modelSpec->record( ui.cbUser->currentIndex() ).value( "spID" ).toInt();
		p_userName = ui.cbUser->currentText();

		p_isAdmin = ( p_userId == 7 );
		connectToDB( p_isAdmin );

		accept();
	}
	else
	{
		QMessageBox::warning( this, "Ошибка", "Введите верный пароль" );
		ui.edPassword->setFocus();
		ui.edPassword->selectAll();
	}
}
