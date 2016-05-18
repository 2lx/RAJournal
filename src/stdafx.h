/*
 * stdafx.h
 *
 *  Created on: 28.09.2011
 *      Author: Aim
 */

#include <QtCore>
#include <QtGui>
#include <QtSql>
#include <QtNetwork>

#if defined( Q_WS_X11)
#include "/usr/include/phonon/mediaobject.h"
#include "/usr/include/phonon/audiooutput.h"
#elif defined(Q_WS_WIN)
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>
#endif

//#include <windows.h>
