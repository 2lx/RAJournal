/*
 * main.cpp
 *
 *  Created on: 24.10.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "mainwindow.h"

int main( int argc, char *argv[] )
{
	QApplication app( argc, argv );
	app.addLibraryPath( app.applicationDirPath() + "/plugins" );

	QTextCodec* codec = QTextCodec::codecForName( "UTF-8" );
	QTextCodec::setCodecForCStrings( codec );

	app.setApplicationName( "IPConServer" );
	QApplication::setQuitOnLastWindowClosed( false );

	MainWindow w;
	w.show();

	return app.exec();
}
