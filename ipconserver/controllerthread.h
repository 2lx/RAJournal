/*
 * controllerthread.h
 *
 *  Created on: 24.10.2011
 *      Author: Aim
 */

#ifndef CONTROLLERTHREAD_H
#define CONTROLLERTHREAD_H

#include <QThread>
#include <QHttp>
#include <QMutex>
#include <QWaitCondition>

class ControllerThread : public QThread
{
    Q_OBJECT

public:
    ControllerThread( QObject *parent = 0 );
    ~ControllerThread();

    void startScanning( const QString & hostName, int interval );
    void stopScanning();
    void run();

signals:
    void newFortune(const QString &fortune);
    void error(int socketError, const QString &message);

private:
    QHttp m_http;
    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_abort;
    QString m_mask, m_state;
    int m_interval;

signals:
    void stateChanged( const QString & strState );
    void errorOccured( const QString & strError );

public slots:
    void setInterval( int newInt );
 
private slots:
    void scanXml( bool error );
};

#endif // CONTROLLERTHREAD_H
