/*
 * controllerthread.cpp
 *
 *  Created on: 24.10.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "controllerthread.h"

ControllerThread::ControllerThread( QObject * parent )
    : QThread( parent ), m_abort( false ), m_mask( "11111111" ), m_state( m_mask )
{
	connect( &m_http, SIGNAL( done( bool ) ), this, SLOT( scanXml( bool ) ) );
}

ControllerThread::~ControllerThread()
{
    m_mutex.lock();
    m_abort = true;
    m_condition.wakeOne();
    m_mutex.unlock();

    wait();
}

void ControllerThread::startScanning( const QString & hostName, int interval )
{
    m_mutex.lock();
    m_http.setHost( hostName );
    m_interval = interval;
    m_abort = false;
    m_mutex.unlock();

    if ( !isRunning() )
    	start( LowestPriority );
    else m_condition.wakeOne();
}

void ControllerThread::setInterval( int newInt )
{
	m_mutex.lock();
    m_interval = newInt;
    m_mutex.unlock();
}

void ControllerThread::stopScanning()
{
	m_abort = true;
    m_http.abort();
    m_condition.wakeAll();
    wait();
}

void ControllerThread::run()
{
	int interTemp;
	while ( !m_abort )
    {
    	m_mutex.lock();

    	m_http.get( "/get?sensor=all" );

    	interTemp = m_interval;
    	m_condition.wait( &m_mutex );
    	m_mutex.unlock();
    	if ( m_abort ) return;
    	msleep( interTemp * 100 );
    }
}

void ControllerThread::scanXml( bool error )
{
	QString strHttpRequest = m_http.readAll();
	if ( !error && !strHttpRequest.trimmed().isEmpty() )
	{
		QXmlStreamReader xml( strHttpRequest );
		xml.readNextStartElement();
		if ( xml.name().toString() != "sl" )
			emit errorOccured( "Неверный формат выходных данных контроллера" );
		else
		{
			xml.readNextStartElement();
			if ( xml.name().toString() != "sensor" || xml.attributes().first().name().toString() != "all" )
				emit errorOccured( "Неверный формат выходных данных контроллера" );
			else
			{
				QString strSensors = xml.attributes().first().value().toString();

				QString strResult = "00000000";
				for ( int i = 0; i < strSensors.length(); i++ )
					if ( strSensors[ i ] == '0' && m_state[ i ] == '1' )
						strResult[ i ] = '1';

//				if ( strResult.contains( '1' ) )
					emit stateChanged( strResult );
				m_state = strSensors;
			}
		}
	}
	else if ( !m_abort )
		emit errorOccured( "Контроллер не отвечает. Возможно указан неверный IP-адрес" );

	m_condition.wakeOne();
}

