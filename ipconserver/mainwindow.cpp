/*
 * mainwindow.cpp
 *
 *  Created on: 24.10.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "mainwindow.h"
#include "controllerthread.h"
#include "database.h"
#include "datatypes.h"

MainWindow::MainWindow( QWidget * parent ) : 
    QMainWindow( parent, Qt::Dialog ),
    m_thread( new ControllerThread( this ) ),
    m_udpSocket( new QUdpSocket( this ) ),
    m_timerUpd( new QTimer( this ) ),
    m_lastTriggered( QDateTime::currentDateTime().addYears( -10 ) ),
    m_timerIPReconnect( new QTimer( this ) ),
    m_timerDBReconnect( new QTimer( this ) ),
    m_timerCheckIPState( new QTimer( this ) )
{
    ui.setupUi( this );

    setWindowTitle( "Сервер IP-контроллера" );

    connect( ui.actMinimize, SIGNAL( triggered() ), this, SLOT( hide() ) );
    connect( ui.actRestore, SIGNAL( triggered() ), this, SLOT( showNormal() ) );
    connect( ui.actExit, SIGNAL( triggered() ), qApp, SLOT( quit() ) );

    connect( ui.btnScanIP, SIGNAL( toggled( bool ) ), this, SLOT( startScanIP( bool ) ) );
    connect( ui.spbIPPeriod, SIGNAL( valueChanged( int ) ), m_thread, SLOT( setInterval( int ) ) );
    connect( m_thread, SIGNAL( stateChanged( const QString & ) ), this, SLOT( IPTriggered( const QString & ) ) );
    connect( m_thread, SIGNAL( errorOccured( const QString & ) ), this, SLOT( IPErrorOccured( const QString & ) ) );

    connect( m_timerUpd, SIGNAL( timeout() ), this, SLOT( broadcastState() ) );
    connect( ui.spbBCPeriod, SIGNAL( valueChanged( int ) ), this, SLOT( setTimerInterval( int ) ) );

    connect( ui.btnConnectDB, SIGNAL( toggled( bool ) ), this, SLOT( connectDB( bool ) ) );
    connect( m_timerIPReconnect, SIGNAL( timeout() ), ui.btnScanIP, SLOT( toggle() ) );
    connect( m_timerDBReconnect, SIGNAL( timeout() ), ui.btnConnectDB, SLOT( toggle() ) );

    QMenu * trayIconMenu = new QMenu( this );
    trayIconMenu->addAction( ui.actMinimize );
    trayIconMenu->addAction( ui.actRestore );
    trayIconMenu->addSeparator();
    trayIconMenu->addAction( ui.actExit );

    m_trayIcon = new QSystemTrayIcon( this );
    m_trayIcon->setContextMenu( trayIconMenu );

    QIcon icon( ":/raiconclean.png" );
    m_trayIcon->setIcon( icon );
    m_trayIcon->setToolTip( "Сервер IP-контроллера" );

    QIcon iconWin( ":/raicon.png" );
    setWindowIcon( iconWin );

    connect( m_trayIcon, SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
             this, SLOT( iconActivated( QSystemTrayIcon::ActivationReason ) ) );
    m_trayIcon->show();

    loadAppSettings();
    m_timerUpd->setInterval( ui.spbBCPeriod->value() * 1000 );
    m_timerUpd->start();

    m_timerIPReconnect->setInterval( ui.spbIPOffPeriod->value() * 1000 );
    m_timerDBReconnect->setInterval( ui.spbDBOffPeriod->value()  * 1000 );
    if ( !ui.btnScanIP->isChecked() ) m_timerIPReconnect->start();
    if ( !ui.btnConnectDB->isChecked() ) m_timerDBReconnect->start();

    m_timerCheckIPState->setInterval( 30*1000 );
    m_timerCheckIPState->setSingleShot( false );
    connect( m_timerCheckIPState, SIGNAL( timeout() ), this, SLOT( checkIPState() ) );
}

MainWindow::~MainWindow()
{
    saveAppSettings();
    m_thread->stopScanning();
    m_thread->wait();
    qDeleteAll( m_lstIntervTimers );
    qDeleteAll( m_lstLampTimers );
}

void MainWindow::startScanIP( bool toggled )
{
    m_timerIPReconnect->setInterval( ui.spbIPOffPeriod->value() * 1000 );
    ui.leHostIP->setEnabled( !toggled );

    if ( toggled )
    {
        m_timerIPReconnect->stop();
        m_thread->startScanning( ui.leHostIP->text().trimmed(), ui.spbIPPeriod->value() );
        m_timerCheckIPState->start();
    }
    else
    {
        m_thread->stopScanning();
        m_timerIPReconnect->start();
    }
}

void MainWindow::checkIPState()
{
    ui.btnScanIP->setChecked( false );
}

void MainWindow::IPTriggered( const QString & strSensors )
{
    m_timerCheckIPState->start();
    if ( strSensors.contains( '1' ) )
    {
        QString strSensorsReverse = "00000000";
        for ( int i = 0; i < strSensors.length(); i++ )
            strSensorsReverse[ 7 - i ] = strSensors[ i ];

        QTimer * tmrLamp = new QTimer( this );
        tmrLamp->setSingleShot( true );
        tmrLamp->setInterval( ui.spbLampTime->value()*1000 );
        tmrLamp->setProperty( "Sensors", strSensors );
        connect( tmrLamp, SIGNAL( timeout() ), this, SLOT( resetLamps() ) );

        QTimer * tmrInt = new QTimer( this );
        tmrInt->setSingleShot( true );
        tmrInt->setInterval( ui.spbRepeat->value()*1000 );
        tmrInt->setProperty( "Sensors", strSensors );
        connect( tmrInt, SIGNAL( timeout() ), this, SLOT( resetIntervals() ) );

        QString strMessageTrayTitle = QDateTime::currentDateTime().toString( "dd MMM yyyy HH:mm:ss" );
        QString strMessageTrayText = strMessageTrayTitle + " Датчики: " + strSensorsReverse;
        QString strMessageLog = strMessageTrayText;

        bool isChecked = false;
        for ( int i = 0; i < m_lstIntervTimers.count(); i++ )
            if ( m_lstIntervTimers[ i ]->property( "Sensors" ) == strSensors )
            {
                m_lstIntervTimers[ i ]->start();
                isChecked = true;
                break;
            }
        if ( !isChecked )
        {
            m_lstIntervTimers.append( tmrInt );
            tmrInt->start();
        }

        bool isLamped = false;
        for ( int i = 0; i < m_lstLampTimers.count(); i++ )
            if ( m_lstLampTimers[ i ]->property( "Sensors" ) == strSensors )
            {
                m_lstLampTimers[ i ]->start();
                isLamped = true;
                break;
            }
        if ( !isLamped )
        {
            m_lstLampTimers.append( tmrLamp );
            tmrLamp->start();
        }

        strMessageTrayTitle += isChecked ? " Напоминание" : " Внимание!";

        bool isDBAdded = true;
        if ( !isChecked )
        {
            if ( ui.btnConnectDB->isChecked() )
            {
                QSqlQueryModel model;
                model.setQuery( RADB::Notification::sqlSelectBySensor.arg( strSensors ) );
                if ( model.rowCount() > 0 )
                {
                    int indMach = model.data( model.index( 0, 5 ) ).toInt();

                    RADB::Journal::insertOneByMachine( indMach );
                    m_lastTriggered = QDateTime::currentDateTime();
                }
                else isDBAdded = false;
            }
            else isDBAdded = false;
        }

        QString strStatus = isDBAdded ? ". Добавлена запись в БД" : ". Ошибка добавления записи в БД";
        strMessageLog += isChecked ? ". Напоминание" : strStatus;
        strMessageTrayText += strStatus;

        m_trayIcon->showMessage( strMessageTrayTitle, strMessageTrayText );
        ui.pteLogIP->appendPlainText( strMessageLog );

        m_httpIP.setHost( ui.leHostIP->text().trimmed() );
        m_httpIP.get( "/set?switch=all&state=11111111&mask=" + strSensors );

    }
}

void MainWindow::IPErrorOccured( const QString & strError )
{
    static int errCount = 0;
    errCount++;
    if ( errCount > 4 )
    {
        if ( ui.cbShowMessage->isChecked() )
            m_trayIcon->showMessage( "ОШИБКА", strError );
        ui.btnScanIP->setChecked( false );
        errCount = 0;
    }
}

void MainWindow::resetLamps()
{
    m_httpIP.setHost( ui.leHostIP->text().trimmed() );

    QTimer * tmr = dynamic_cast< QTimer * >( sender() );
    QString strSensors = tmr->property( "Sensors" ).toString();
    m_httpIP.get( "/set?switch=all&state=00000000&mask=" + strSensors );

    m_lstLampTimers.removeOne( tmr );
}

void MainWindow::resetIntervals()
{
    QTimer * tmr = dynamic_cast< QTimer * >( sender() );
    m_lstIntervTimers.removeOne( tmr );
}

void MainWindow::setTimerInterval( int sec )
{
    m_timerUpd->setInterval( sec * 1000 );
}

void MainWindow::broadcastState()
{
    //	QMessageBox::about( 0, "", QString::number( QHostAddress( QHostAddress::Broadcast ).toIPv4Address(), 16 ) );
    statusBar()->showMessage( "Сигнал броадкаст", 2000 );
    QByteArray datagram = m_lastTriggered.toString( Qt::ISODate ).toLatin1();
    m_udpSocket->writeDatagram( datagram.data(), datagram.size(), QHostAddress::Broadcast, ui.spbBCPort->value() );
    //    m_udpSocket->writeDatagram( datagram.data(), datagram.size(), QHostAddress::Any, ui.spbBCPort->value() );
    //    m_udpSocket->writeDatagram( datagram.data(), datagram.size(), QHostAddress( "192.168.1.255" ), ui.spbBCPort->value() );
    //    m_udpSocket->writeDatagram( datagram.data(), datagram.size(), QHostAddress( "192.168.1.0" ), ui.spbBCPort->value() );

}

void MainWindow::connectDB( bool toggled )
{
    ui.leHostDB->setEnabled( !toggled );
    m_timerDBReconnect->setInterval( ui.spbDBOffPeriod->value() * 1000 );

    if ( toggled )
    {
        m_timerDBReconnect->stop();
        if ( !RADB::setPSQLConnection( ui.leHostDB->text().trimmed(), "raadmin", "*********" ) )
            ui.btnConnectDB->setChecked( false );
    }
    else
    {
        RADB::discardPSQLConnection();
        m_timerDBReconnect->start();
    }
}

void MainWindow::iconActivated( QSystemTrayIcon::ActivationReason reason )
{
    switch ( reason )
    {
    case QSystemTrayIcon::Trigger:
        showNormal();
        break;
    case QSystemTrayIcon::DoubleClick:
    case QSystemTrayIcon::MiddleClick:
        m_trayIcon->contextMenu()->popup( m_trayIcon->geometry().center() );
        break;
    default: {}
    }
}

void MainWindow::loadAppSettings()
{
    QSettings appSettings( qApp->applicationName() );

    resize( appSettings.value( "MainWindow.Width", width() ).toInt(),
            appSettings.value( "MainWindow.Height", height() ).toInt() );
    if ( appSettings.value( "MainWindow.isHidden" ).toInt() ) hide();

    ui.leHostIP->setText( appSettings.value( "IP.Host", "localhost" ).toString() );
    ui.spbIPPeriod->setValue( appSettings.value( "IP.Period", 10 ).toInt() );
    ui.btnScanIP->setChecked( appSettings.value( "IP.Scaning", false ).toBool() );
    ui.spbIPOffPeriod->setValue( appSettings.value( "IP.OffPeriod", 15 ).toInt() );

    ui.leHostDB->setText( appSettings.value( "DB.Host", "localhost" ).toString() );
    ui.btnConnectDB->setChecked( appSettings.value( "DB.Connecting", false ).toBool() );
    ui.spbRepeat->setValue( appSettings.value( "DB.RepeatTime", 300 ).toInt() );
    ui.spbDBOffPeriod->setValue( appSettings.value( "DB.OffPeriod", 30 ).toInt() );

    ui.spbBCPort->setValue( appSettings.value( "BC.Port", 45454 ).toInt() );
    ui.spbBCPeriod->setValue( appSettings.value( "BC.Period", 10 ).toInt() );

    ui.cbShowMessage->setChecked( appSettings.value( "ShowMessage", false ).toBool() );
}

void MainWindow::saveAppSettings()
{
    QSettings appSettings( qApp->applicationName() );

    appSettings.setValue( "MainWindow.Width", normalGeometry().width() );
    appSettings.setValue( "MainWindow.Height", normalGeometry().height() );
    appSettings.setValue( "MainWindow.isHidden", int( isHidden() ) );

    appSettings.setValue( "IP.Host", ui.leHostIP->text() );
    appSettings.setValue( "IP.Period", ui.spbIPPeriod->value() );
    appSettings.setValue( "IP.Scaning", ui.btnScanIP->isChecked() );
    appSettings.setValue( "IP.OffPeriod", ui.spbIPOffPeriod->value() );

    appSettings.setValue( "DB.Host", ui.leHostDB->text() );
    appSettings.setValue( "DB.Connecting", ui.btnConnectDB->isChecked() );
    appSettings.setValue( "DB.RepeatTime", ui.spbRepeat->value() );
    appSettings.setValue( "DB.OffPeriod", ui.spbDBOffPeriod->value() );

    appSettings.setValue( "BC.Port", ui.spbBCPort->value() );
    appSettings.setValue( "BC.Period", ui.spbBCPeriod->value() );

    appSettings.setValue( "ShowMessage", ui.cbShowMessage->isChecked() );
}
