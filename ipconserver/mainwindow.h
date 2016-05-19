/*
 * mainwindow.h
 *
 *  Created on: 24.10.2011
 *      Author: Aim
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"
#include <QHttp>
#include <QSystemTrayIcon>
#include <QDateTime>
#include <QUdpSocket>

class ControllerThread;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent = 0 );
	~MainWindow();

private:
	Ui::MainWindow ui;	
	QSystemTrayIcon * m_trayIcon;
	ControllerThread * m_thread;
	QHttp m_httpIP;
	QList< QTimer * > m_lstIntervTimers;
	QList< QTimer * > m_lstLampTimers;
	QUdpSocket * m_udpSocket;
	QTimer * m_timerUpd;
	QDateTime m_lastTriggered;
	QTimer * m_timerIPReconnect;
	QTimer * m_timerDBReconnect;
	QTimer * m_timerCheckIPState;

	void loadAppSettings();
	void saveAppSettings();

private slots:
    void iconActivated( QSystemTrayIcon::ActivationReason reason );
    void startScanIP( bool toggled );
    void IPTriggered( const QString & strSensors );
    void IPErrorOccured( const QString & strError );

    void connectDB( bool toggled );
    void resetLamps();
    void resetIntervals();
    void broadcastState();
    void setTimerInterval( int sec );
    void checkIPState();
};

#endif  // MAINWINDOW_H
