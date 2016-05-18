/*
 * main.cpp
 *
 *  Created on: 25.09.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "dlglogin.h"
#include "database.h"
#include "mainwindow.h"

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv );
	app.addLibraryPath( app.applicationDirPath() + "/plugins" );

    QTextCodec* codec = QTextCodec::codecForName( "UTF-8" );
	QTextCodec::setCodecForCStrings( codec );

	app.setApplicationName( "RAJournal" );

	MainWindow w;
	w.hide();

	DlgLogin dlgLogin( 0 );
	if ( dlgLogin.exec() == QDialog::Accepted )
	{
		w.setUserId( dlgLogin.userId(), dlgLogin.userName(), dlgLogin.isAdmin() );
		w.refreshAll();
		w.show();
		return app.exec();
	}
	else return 0;
}
