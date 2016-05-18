/*
 * udpalarm.h
 *
 *  Created on: 07.11.2011
 *      Author: MinchaAV
 */

#ifndef UDPALARM_H_
#define UDPALARM_H_

#include <qobject.h>
#include <QUdpSocket>
#include <QTimer>
#include <QDateTime>
#include <phonon/mediaobject.h>
#include <phonon/audiooutput.h>

class UdpAlarm : public QObject
{
	Q_OBJECT
public:
	UdpAlarm( QObject * parent = 0 );
	inline void setHost( const QString & strHost ) { m_udpHost = strHost; }
	inline void setPort( int iPort ) { m_udpPort = iPort; }
	inline void setReconnection( bool bReenable ) { m_udpReconnectEnable = bReenable; }
	void startPolling();

private:
	QUdpSocket * m_udpSocket;
	Phonon::MediaObject * m_media;
	Phonon::AudioOutput * m_audioOutput;

	int m_udpPort;
	QString m_udpHost;
	QTimer m_timerChkUdpState;
	bool m_udpWasConnection;
	bool m_udpReconnectEnable;
	QDateTime m_lastInsertedDT;

private slots:
	void setupUDPOnline( bool isOnline );
	void pendingDatagrams();
	void checkUdpConnection();
	void replayAlarm();
	void firstMelody();

signals:
	void signalReceived();
	void eventOccured();
	void serverWasRespond( bool wasResponded );
};

#endif /* UDPALARM_H_ */
