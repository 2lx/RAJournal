/*
 * udpalarm.cpp
 *
 *  Created on: 07.11.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "udpalarm.h"

#include "database.h"

UdpAlarm::UdpAlarm( QObject * parent ) : QObject( parent ),
	m_udpSocket( new QUdpSocket( this ) ),
    m_media( new Phonon::MediaObject( this ) ),
	m_audioOutput( new Phonon::AudioOutput( Phonon::MusicCategory, this ) ),
	m_lastInsertedDT( QDateTime::fromMSecsSinceEpoch( 0 ) )
{
	Phonon::createPath( m_media, m_audioOutput );

    connect( m_media, SIGNAL( aboutToFinish () ), this, SLOT( replayAlarm() ) );
    connect( &m_timerChkUdpState, SIGNAL( timeout() ), this, SLOT( checkUdpConnection() ) );
	connect( m_udpSocket, SIGNAL( readyRead() ), this, SLOT( pendingDatagrams() ) );

    firstMelody();
    m_timerChkUdpState.setInterval( 40*1000 );
    m_timerChkUdpState.setSingleShot( true );
//	setupUDPOnline( true );
}

void UdpAlarm::startPolling()
{
	m_udpWasConnection = false;
	m_timerChkUdpState.start();
}

void UdpAlarm::setupUDPOnline( bool isOnline )
{
	m_udpWasConnection = false;
	if ( isOnline )
	{
		m_udpSocket->bind( QHostAddress( m_udpHost ), m_udpPort, QUdpSocket::ReuseAddressHint );
	    m_timerChkUdpState.start();
	}
	else m_timerChkUdpState.stop();
}

void UdpAlarm::pendingDatagrams()
{
	m_udpWasConnection = true;
	while ( m_udpSocket->hasPendingDatagrams() )
	{
		QByteArray datagram;
	    datagram.resize( m_udpSocket->pendingDatagramSize() );
	    m_udpSocket->readDatagram( datagram.data(), datagram.size() );

	    QDateTime dt = QDateTime::fromString( QString::fromLatin1( datagram ) , Qt::ISODate );
	    if ( dt.secsTo( QDateTime::currentDateTime() ) < 10*60 && ( dt > m_lastInsertedDT ) )
	    {
	    	m_media->play();
	    	m_lastInsertedDT = dt;
	    	emit eventOccured();

//	    	QSqlQueryModel model;
	    	QDateTime dt = QDateTime::currentDateTime().addDays( -1 );
//	    	model.setQuery( RADB::Journal::sqlSelectAllAfterDT.arg( dt.toString( "yyyy-MM-dd HH:mm:ss" ) ) );

//	    	QString strMachine = model.data( model.index( model.rowCount() - 1, 2 ) ).toString();	// last record in list ordered by Error time
//	    	strMachine = strMachine.left( strMachine.indexOf( ":" ) );


	    	QMessageBox * mb = new QMessageBox( QMessageBox::Information, "Внимание", "Время: <b>" + dt.toString( "HH:mm:ss" )
//	    			+ "</b>\nВызов со станка: <b>" + strMachine + "</b>! \n"
	    			+ "Требуется участие специалиста.", QMessageBox::Ok, 0 );

	    	connect( mb, SIGNAL( finished( int ) ), m_media, SLOT( stop() ) );
	    	connect( mb, SIGNAL( finished( int ) ), this, SLOT( firstMelody() ) );
	    	mb->show();
   		}
	}
	emit signalReceived();
}

void UdpAlarm::firstMelody()
{
    m_media->setCurrentSource( QUrl(QApplication::applicationDirPath() + QDir::separator() + "ghostbusters.mp3") );
}

void UdpAlarm::replayAlarm()
{
    m_media->enqueue( Phonon::MediaSource( QUrl( QApplication::applicationDirPath() + QDir::separator() + "alarm.mp3" ) ) );
}

void UdpAlarm::checkUdpConnection()
{
	emit serverWasRespond( m_udpWasConnection );

	if ( !m_udpWasConnection )
		QMessageBox::warning( 0, "Внимание", "Сервер броадкаст не обнаружен. Возможно: \n - нет доступа к сети; \n - сервер "
				"не запущен;\n - неверно указаны IP и порт сервера в настройках" );

	m_udpWasConnection = false;
	if ( m_udpReconnectEnable )
		m_timerChkUdpState.start();
}
