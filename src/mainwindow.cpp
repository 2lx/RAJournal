/*
 * mainwindow.cpp
 *
 *  Created on: 25.09.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "mainwindow.h"

#include "journalmodel.h"
#include "solutionmodel.h"
#include "plotter.h"
#include "database.h"
#include "dlgnotification.h"
#include "dlgstructure.h"
#include "dlgpassword.h"
#include "dlglogin.h"
#include "dlgsettings.h"
#include "tablemodel.h"

#include "udpalarm.h"

MainWindow::MainWindow( QWidget * parent ) : 
	QMainWindow( parent ),
	m_modelSpecialist( new TableModel< RASpecialist >() ),
	m_modelDepartment( new TableModel< RADepartment >() ),
	m_modelMachineModel( new TableModel< RAMachineModel >() ),
	m_modelMachine( new TableModel< RAMachine >() ),
	m_modelUnit( new TableModel< RAUnit >() ),
//	m_modelSensor( new TableModel< RASensor >() ),
//	m_modelDetailModel( new TableModel< RADetailModel >() ),
	m_modelSFJournal( new QSortFilterProxyModel( this ) ),
	m_modelSFMachine( new QSortFilterProxyModel( this ) ),
	m_modelSFUnit( new QSortFilterProxyModel( this ) ),
	m_modelSFSymptom( new QSortFilterProxyModel( this ) ),
	m_modelSFSolution( new QSortFilterProxyModel( this ) ),
	m_dock( new QDockWidget( this ) ),
	m_plot( new BodePlot( m_dock ) ),
	m_curUserId( -1 ),
	m_prevCurIndex( -1 ),
	m_udpAlarm( new UdpAlarm( this ) )
{
// GUI
	ui.setupUi( this );

	m_dock->setFeatures( QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable );
	m_dock->setAllowedAreas( Qt::NoDockWidgetArea );
	m_dock->setFloating( true );
	m_dock->hide();
	m_dock->resize( 400, 400 );
	m_dock->setWidget( m_plot );
	m_dock->setWindowTitle( "Графическое представление" );

    setWindowIcon( QIcon( ":/raicon.png" ) );
	ui.actDelete->setEnabled( false );
	ui.cbFilterDays->setCurrentIndex( 2 );
	m_enbStruct.number = 0xFFFF;
	m_enbUpdate.number = 0xFFFF;

// models
	m_modelSFJournal->setSourceModel( RADB::Journal::model() );
	m_modelSFJournal->setDynamicSortFilter( true );
	ui.tvJournal->setModel( m_modelSFJournal );

	ui.tvJournal->hideColumn( JournalModel::clID );
	ui.tvJournal->hideColumn( JournalModel::clSolveDT );
	ui.tvJournal->hideColumn( JournalModel::clSpecID );
	ui.tvJournal->hideColumn( JournalModel::clUnitID );
	ui.tvJournal->hideColumn( JournalModel::clMachineID );
	ui.tvJournal->hideColumn( JournalModel::clDepartID );
	ui.tvJournal->hideColumn( JournalModel::clSymptomID );

	m_modelSFSolution->setSourceModel( RADB::SolutionGroup::model() );
	m_modelSFSolution->setDynamicSortFilter( true );
	ui.tvSolution->setModel( m_modelSFSolution );

	ui.tvSolution->hideColumn( SolutionModel::clID );
	ui.tvSolution->hideColumn( SolutionModel::clSolutionID );
	ui.tvSolution->hideColumn( SolutionModel::clSensorID );
	ui.tvSolution->hideColumn( SolutionModel::clSymptomID );
	ui.tvSolution->hideColumn( SolutionModel::clJournalID );
	ui.tvSolution->hideColumn( SolutionModel::clDetailModelID );
	ui.tvSolution->hideColumn( SolutionModel::clSensorName );

	m_solutionEditor = new SolutionEditorFactory( QItemEditorFactory::defaultFactory() );
	QItemEditorFactory::setDefaultFactory( m_solutionEditor );

// signals
	connect( m_modelSFJournal, SIGNAL( layoutChanged () ), this, SLOT( setupRowHighlight() ) );
	connect( ui.tvJournal, SIGNAL( curIndexChanged() ), this, SLOT( curRecordChanged() ) );
	connect( ui.btnEditHide, SIGNAL( clicked() ), this, SLOT( setEditHide() ) );
	connect( ui.btnSetEndTime, SIGNAL( clicked() ), this, SLOT( setEndTime() ) );
	connect( ui.actFilter, SIGNAL( triggered( bool ) ), ui.wgtFilter, SLOT( setVisible( bool ) ) );
	connect( RADB::SolutionGroup::model(), SIGNAL( dataChanged( const QModelIndex &, const QModelIndex & ) ), this, SLOT( refreshMainView() ) );

	connect( ui.actAdd, SIGNAL( triggered() ), this, SLOT( addRecord() ) );
	connect( ui.actDelete, SIGNAL( triggered() ), this, SLOT( deleteRecord() ) );
	connect( ui.actRefresh, SIGNAL( triggered() ), this, SLOT( refreshAll() ) );
	connect( ui.btnAddSolution, SIGNAL( clicked() ), this, SLOT( addSolution() ) );
	connect( ui.btnDeleteSolution, SIGNAL( clicked() ), this, SLOT( deleteSolution() ) );

	connect( ui.actStructure, SIGNAL( triggered() ), this, SLOT( editStructure() ) );
	connect( ui.actPassword, SIGNAL( triggered() ), this, SLOT( editPassword() ) );
	connect( ui.actNotification, SIGNAL( triggered() ), this, SLOT( editNotification() ) );
	connect( ui.actPlotter, SIGNAL( triggered() ), this, SLOT( drawGraph() ) );
	connect( ui.actUser, SIGNAL( triggered() ), this, SLOT( changeUser() ) );
	connect( ui.actSettings, SIGNAL( triggered() ), this, SLOT( editSettings() ) );

	connect( ui.cbDepartment, SIGNAL( currentIndexChanged( int ) ), this, SLOT( departmentChanged() ) );
	connect( ui.cbMachine, SIGNAL( currentIndexChanged( int ) ), this, SLOT( machineChanged() ) );
	connect( ui.cbUnit, SIGNAL( currentIndexChanged( int ) ), this, SLOT( unitChanged() ) );
	connect( ui.cbSymptom, SIGNAL( currentIndexChanged( int ) ), this, SLOT( symptomChanged() ) );

	connect( ui.dteError, SIGNAL( editingFinished() ), this, SLOT( updateErrorDT() ) );
	connect( ui.dteSolve, SIGNAL( editingFinished() ), this, SLOT( updateSolveDT() ) );
	connect( ui.cbSpecialist, SIGNAL( currentIndexChanged( int ) ), this, SLOT( updateSpecialist( int ) ) );
	connect( ui.cbSymptom->lineEdit(), SIGNAL( editingFinished() ), this, SLOT( updateSymptom() ) );
	connect( ui.btnSymptomReturn, SIGNAL( clicked() ), this, SLOT( updateSymptom() ) );

// alarm
	connect( ui.actBroadcast, SIGNAL( triggered( bool ) ), m_udpAlarm, SLOT( setupUDPOnline( bool ) ) );
	connect( m_udpAlarm, SIGNAL( eventOccured() ), this, SLOT( refreshMainView() ) );
	connect( m_udpAlarm, SIGNAL( eventOccured() ), this, SLOT( udpSignalReceived() ) );
	connect( m_udpAlarm, SIGNAL( serverWasRespond( bool ) ), ui.actBroadcast, SLOT( setChecked( bool ) ) );

    loadAppSettings();
}

MainWindow::~MainWindow()
{
	if ( m_curUserId >= 0 ) saveAppSettings();
}

void MainWindow::setEditHide()
{
	bool isVis = ui.frmDetails->isVisible();
	ui.frmDetails->setVisible( !isVis );
	ui.btnEditHide->setIcon( QIcon( isVis ? ":/arrow_up.png" : ":/arrow_down.png" ) );
}

void MainWindow::udpSignalReceived()
{
	statusBar()->showMessage( "Получен сигнал с сервера", 1000 );
}

void MainWindow::changeUser()
{
	hide();
	DlgLogin dlgLogin( this );
	if ( dlgLogin.exec() == QDialog::Accepted )
	{
		setUserId( dlgLogin.userId(), dlgLogin.userName(), dlgLogin.isAdmin() );
		refreshAll();
		show();
	}
}

void MainWindow::setUserId( int newId, const QString & strName, bool isAdmin )
{
	m_curUserId = newId;
	setWindowTitle( "Журнал ОПА v1.05. " + QString( isAdmin ? "Администратор" : "Пользователь" ) + ": " + strName);
	ui.actPassword->setEnabled( isAdmin );
	ui.actNotification->setEnabled( isAdmin );
	m_isAdmin = isAdmin;
}

void MainWindow::setEndTime()
{
	ui.dteSolve->setDateTime( QDateTime::currentDateTime() );
	updateSolveDT();
}

void MainWindow::editSettings()
{
	DlgSettings dlgSettings( this );
	if ( dlgSettings.exec() == QDialog::Accepted )
	{
		m_udpAlarm->setHost( dlgSettings.host() );
		m_udpAlarm->setPort( dlgSettings.port() );
		m_udpAlarm->setReconnection( dlgSettings.autoReconnect() );
		if ( ui.actBroadcast->isChecked() )
			m_udpAlarm->startPolling();
	}
}

void MainWindow::refreshAll()
{
	m_enbStruct.number = 0;
	m_enbUpdate.number = 0;
	refreshDepartment();
	refreshStructure();

	m_enbStruct.number = 0xFFFF;
	refreshMainView();
	m_enbUpdate.number = 0xFFFF;
}

void MainWindow::refreshMainView()
{
	QDateTime dtAfter = QDateTime::currentDateTime();
	dtAfter.setTime( QTime( 0, 0, 0 ) );
	switch ( ui.cbFilterDays->currentIndex() )
	{
	case 0: dtAfter = dtAfter.addDays( -1 );
		break;
	case 1:	dtAfter = dtAfter.addDays( -3 );
		break;
	case 2:	dtAfter = dtAfter.addDays( -7 );
		break;
	case 3:	dtAfter = dtAfter.addMonths( -1 );
		break;
	case 4:	dtAfter = dtAfter.addMonths( -3 );
		break;
	case 5:	dtAfter = dtAfter.addYears( -1 );
		break;
	case 6:	dtAfter = QDateTime::fromMSecsSinceEpoch( 0 );
		break;
	}

	RADB::Journal::refresh( dtAfter );

	statusBar()->showMessage( "Фильтр: записи с " + dtAfter.toString( "yyyy-MM-dd" ), 5000 );

	setupRowHighlight();

	int iInd = -1;
	for ( int i = 0; i < m_modelSFJournal->rowCount(); i++ )
		if ( m_modelSFJournal->data( m_modelSFJournal->index( i, 0 ) ).toInt() == m_prevJournalID )
		{
			iInd = i;
			break;
		}
	ui.tvJournal->setCurrentIndex( m_modelSFJournal->index( iInd, 0 ) );
}

void MainWindow::refreshDepartment()
{
	m_modelSpecialist->refresh();
	ui.cbSpecialist->setModel( m_modelSpecialist );
	ui.cbSpecialist->setModelColumn( 1 );

	m_modelDepartment->refresh();
	ui.cbDepartment->setModel( m_modelDepartment );
	ui.cbDepartment->setModelColumn( 1 );

	m_modelMachine->refresh();
	m_modelSFMachine->setSourceModel( m_modelMachine );
	m_modelSFMachine->setDynamicSortFilter( true );
	m_modelSFMachine->setFilterKeyColumn( 3 );
	ui.cbMachine->setModel( m_modelSFMachine );
	ui.cbMachine->setModelColumn( 1 );
}

void MainWindow::refreshStructure()
{
	m_modelUnit->refresh();
	m_modelSFUnit->setSourceModel( m_modelUnit );
	m_modelSFUnit->setDynamicSortFilter( true );
	m_modelSFUnit->setFilterKeyColumn( unModelID );
	ui.cbUnit->setModel( m_modelSFUnit );
	ui.cbUnit->setModelColumn( unShortName );

	RADB::Symptom::refresh();
	m_modelSFSymptom->setSourceModel( RADB::Symptom::model() );
	m_modelSFSymptom->setDynamicSortFilter( true );
	m_modelSFSymptom->setFilterKeyColumn( 2 );
	ui.cbSymptom->setModel( m_modelSFSymptom );
	ui.cbSymptom->setModelColumn( 1 );
}

bool MainWindow::isOpenRow( int indexRow )
{
	QModelIndex indTemp = m_modelSFJournal->index( indexRow, JournalModel::clSymptomID );
	int iSyID =  m_modelSFJournal->data( indTemp ).toInt();
	indTemp = m_modelSFJournal->index( indexRow, JournalModel::clSymptomText );
	QString strSymptom =  m_modelSFJournal->data( indTemp ).toString().trimmed();

	indTemp = m_modelSFJournal->index( indexRow, JournalModel::clErrorDT );
	QDateTime dtError =  m_modelSFJournal->data( indTemp ).toDateTime();
	indTemp = m_modelSFJournal->index( indexRow, JournalModel::clSolveDT );
	QDateTime dtSolve =  m_modelSFJournal->data( indTemp ).toDateTime();

	indTemp = m_modelSFJournal->index( indexRow, JournalModel::clSpecID );
	int spID =  m_modelSFJournal->data( indTemp ).toInt();

	return ( !iSyID || dtSolve < dtError || ( strSymptom == RADB::Symptom::strZeroString ) || !spID );
}

void MainWindow::setupRowHighlight()
{
	for ( int i = 0; i < m_modelSFJournal->rowCount(); i++ )
	{
		QModelIndex indTemp = m_modelSFJournal->index( i, JournalModel::clSpecID );
		int iSpecID =  m_modelSFJournal->data( indTemp ).toInt();

		if ( isOpenRow( i ) ) // red
		{
			JournalDelegate * delegate = new JournalDelegate( "#f9c5b9", m_modelSFJournal );
			ui.tvJournal->setItemDelegateForRow( i, delegate );
		}
		else if ( iSpecID == m_curUserId && !m_isAdmin)	// green
		{
			JournalDelegate * delegate = new JournalDelegate( "#8effa6", m_modelSFJournal );
			ui.tvJournal->setItemDelegateForRow( i, delegate );
		}
		else ui.tvJournal->setItemDelegateForRow( i, new JournalDelegate( "#ffffff", m_modelSFJournal ) );
	}
}

void MainWindow::departmentChanged()
{
	if ( !m_enbStruct.bits.bDepartment ) return;

	QString strIndDep = m_modelDepartment->data( ui.cbDepartment->currentIndex(), dpID ).toString();
	if ( strIndDep.isEmpty() ) strIndDep = "-1";
	m_modelSFMachine->setFilterRegExp( "\\b" + strIndDep + "\\b" );
}

void MainWindow::machineChanged()
{
	if ( !m_enbStruct.bits.bMachine ) return;

	QModelIndex indMach = m_modelSFMachine->index( ui.cbMachine->currentIndex(), 4 );
	QString strIndMach = m_modelSFMachine->data( indMach ).toString();
	if ( strIndMach.isEmpty() ) strIndMach = "-1";
	m_modelSFUnit->setFilterRegExp( "\\b" + strIndMach + "\\b" );
}

void MainWindow::unitChanged()
{
	if ( !m_enbStruct.bits.bUnit ) return;

	QString strSymptom = ui.cbSymptom->currentText();
	QModelIndex indUnit = m_modelSFUnit->index( ui.cbUnit->currentIndex(), 0 );
	QString strIndUnit = m_modelSFUnit->data( indUnit ).toString();
	if ( strIndUnit.isEmpty() ) strIndUnit = "-1";
	m_modelSFSymptom->setFilterRegExp( "\\b" + strIndUnit + "\\b" );
	ui.cbSymptom->setEditText( strSymptom );
}

void MainWindow::symptomChanged()
{
	if ( !m_enbStruct.bits.bSymptom ) return;
//	static int st = 0; st++; statusBar()->showMessage( QString::number( st ), 2000 );
}

int MainWindow::getCBIndex( int iRow, int iCol, QAbstractItemModel * const model ) const
{
	QModelIndex indTemp = m_modelSFJournal->index( iRow, iCol );
	for ( int i = 0; i < model->rowCount(); i++ )
	{
		QModelIndex indNode = model->index( i, 0 );
		if ( model->data( indNode ).toInt() == m_modelSFJournal->data( indTemp ).toInt() )
			return i;
	}
	return -1;
}

void MainWindow::curRecordChanged()
{
	QModelIndex index = ui.tvJournal->currentIndex();
	if ( index.row() < 0 || index.row() > m_modelSFJournal->rowCount() - 1 || !index.isValid() )
	{
		ui.frmDetails->setEnabled( false );
		ui.gbProblem->setEnabled( false );
		return;
	}
	ui.frmDetails->setEnabled( true );
	ui.gbProblem->setEnabled( true );
	m_enbUpdate.number = 0;

	m_prevCurIndex = ui.tvJournal->currentIndex().row();

	ui.actDelete->setEnabled( m_isAdmin );
	ui.cbSpecialist->setEnabled( m_isAdmin );

	QModelIndex indSpecId = m_modelSFJournal->index( index.row(), JournalModel::clSpecID );
	bool isEditEnable = m_modelSFJournal->data( indSpecId ).toInt() == m_curUserId || m_isAdmin || isOpenRow( index.row() );
	ui.dteError->setEnabled( isEditEnable );
	ui.btnSetEndTime->setEnabled( isEditEnable );
	ui.dteSolve->setEnabled( isEditEnable );
	ui.cbDepartment->setEnabled( isEditEnable );
	ui.cbMachine->setEnabled( isEditEnable );
	ui.cbUnit->setEnabled( isEditEnable );
	ui.cbSymptom->setEnabled( isEditEnable );
	ui.tvSolution->setEnabled( isEditEnable );

// setup SOLUTION
	m_prevJournalID = m_modelSFJournal->data( m_modelSFJournal->index( index.row(), JournalModel::clID ) ).toInt();
	RADB::SolutionGroup::refresh( m_prevJournalID );

// setup SPECIALIST
	if ( m_modelSFJournal->data( indSpecId ).toInt() != 0 )
		ui.cbSpecialist->setCurrentIndex( getCBIndex( index.row(), JournalModel::clSpecID, m_modelSpecialist ) );
	else
		for ( int i = 0; i < m_modelSpecialist->rowCount(); i++ )
		{
			if ( m_modelSpecialist->data( i, spID ).toInt() == m_curUserId )
			{
				ui.cbSpecialist->setCurrentIndex( i );
				break;
			}
		}

// setup STRUCTURE
	m_enbStruct.number = 0;
	ui.cbDepartment->setCurrentIndex( getCBIndex( index.row(), JournalModel::clDepartID, m_modelDepartment ) );
	m_enbStruct.bits.bDepartment = 1;
	departmentChanged();	// ! if index was not changed after setCurrentIndex()

	ui.cbMachine->setCurrentIndex( getCBIndex( index.row(), JournalModel::clMachineID, m_modelSFMachine ) );
	m_enbStruct.bits.bMachine = 1;
	machineChanged();

	ui.cbUnit->setCurrentIndex( getCBIndex( index.row(), JournalModel::clUnitID, m_modelSFUnit ) );
	m_enbStruct.bits.bUnit = 1;
	unitChanged();

	ui.cbSymptom->setCurrentIndex( getCBIndex( index.row(), JournalModel::clSymptomID, m_modelSFSymptom ) );
	m_enbStruct.bits.bSymptom = 1;
	symptomChanged();

// setup DT ERROR
	QModelIndex indErrorDT = m_modelSFJournal->index( index.row(), JournalModel::clErrorDT );
	ui.dteError->setDateTime( m_modelSFJournal->data( indErrorDT ).toDateTime() );

// setup DT SOLVE
	QModelIndex indSolveDT = m_modelSFJournal->index( index.row(), JournalModel::clSolveDT );
	ui.dteSolve->setDateTime( m_modelSFJournal->data( indSolveDT ).toDateTime() );

	m_enbUpdate.number = 0xFFFF;

	QModelIndex indSymptomID = m_modelSFJournal->index( index.row(), JournalModel::clSymptomID );
	m_solutionEditor->setSymptomID( m_modelSFJournal->data( indSymptomID ).toInt() );

	QModelIndex indUnitID = m_modelSFJournal->index( index.row(), JournalModel::clUnitID );
	m_solutionEditor->setUnitID( m_modelSFJournal->data( indUnitID ).toInt() );
}

void MainWindow::addRecord()
{
	RADB::Journal::insertOneByUser( m_curUserId );
	refreshMainView();
	ui.tvJournal->setCurrentIndex( m_modelSFJournal->index( m_modelSFJournal->rowCount() - 1, 0 ) );
	ui.tvJournal->scrollToBottom();
	addSolution();
}

void MainWindow::deleteRecord()
{
	if ( !ui.tvJournal->selectionModel()->selection().count() ) return;
	if ( QMessageBox::question( this, "Внимание", "Удалить выделенную запись?",
			QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
	{
		QModelIndex indTemp = m_modelSFJournal->index( ui.tvJournal->currentIndex().row(), JournalModel::clSymptomID );
		int iSyID = m_modelSFJournal->data( indTemp ).toInt();

		indTemp = m_modelSFJournal->index( ui.tvJournal->currentIndex().row(), JournalModel::clID );
		int iJoID = m_modelSFJournal->data( indTemp ).toInt();
		RADB::Journal::deleteOne( iJoID, iSyID );

		refreshMainView();
	}
}

void MainWindow::addSolution()
{
	QModelIndex indJournal( m_modelSFJournal->index( m_prevCurIndex, JournalModel::clID ) );
	int iJournalID = m_modelSFJournal->data( indJournal ).toInt();

	QModelIndex indSymptom( m_modelSFJournal->index( m_prevCurIndex, JournalModel::clSymptomID ) );
	int iSymptomID = m_modelSFJournal->data( indSymptom ).toInt();

	RADB::SolutionGroup::insertNew( iJournalID, iSymptomID );
	refreshMainView();
}

void MainWindow::deleteSolution()
{
	QModelIndex indSolutionG( m_modelSFSolution->index( ui.tvSolution->currentIndex().row(), SolutionModel::clID ) );
	int iSGID = m_modelSFSolution->data( indSolutionG ).toInt();

	QModelIndex indSolutionID( m_modelSFSolution->index( ui.tvSolution->currentIndex().row(), SolutionModel::clSolutionID ) );
	int iSolID = m_modelSFSolution->data( indSolutionID ).toInt();

	RADB::SolutionGroup::deleteOne( iSGID, iSolID );
	refreshMainView();
}

void MainWindow::updateErrorDT()
{
	if ( !m_enbUpdate.bits.bErrorDT ) return;

	if ( m_modelSFJournal->setData( m_modelSFJournal->index( m_prevCurIndex, JournalModel::clErrorDT ), ui.dteError->dateTime() )
		&& m_modelSFJournal->setData( m_modelSFJournal->index( m_prevCurIndex, JournalModel::clSolveDT ), ui.dteError->dateTime() ) )
		refreshMainView();
}

void MainWindow::updateSolveDT()
{
	if ( !m_enbUpdate.bits.bSolveDT ) return;

	if ( m_modelSFJournal->setData( m_modelSFJournal->index( m_prevCurIndex, JournalModel::clSolveDT ), ui.dteSolve->dateTime() ) )
		refreshMainView();
}

void MainWindow::updateSpecialist( int index )
{
	if ( !m_enbUpdate.bits.bSpecialist ) return;

	int iSPIndex = m_modelSpecialist->data( index, spID ).toInt();
	if ( m_modelSFJournal->setData( m_modelSFJournal->index( m_prevCurIndex, JournalModel::clSpecID ), iSPIndex ) )
		refreshMainView();
}

void MainWindow::updateSymptom()
{
	if ( !m_enbUpdate.bits.bSymptom ) return;

	QModelIndex indJournal( m_modelSFJournal->index( m_prevCurIndex, JournalModel::clID ) );
	int iJournalID = m_modelSFJournal->data( indJournal ).toInt();
	int iMachineID = m_modelSFMachine->data( m_modelSFMachine->index( ui.cbMachine->currentIndex(), 0 ) ).toInt();

	QModelIndex indSymptom( m_modelSFJournal->index( m_prevCurIndex, JournalModel::clSymptomID ) );
	int iSymptomID = m_modelSFJournal->data( indSymptom ).toInt();

	int iUnitID = 0;
	if ( m_modelSFUnit->rowCount() > 0 )
	{
		QModelIndex indUnitId( m_modelSFUnit->index( ui.cbUnit->currentIndex(), 0 ) );
		iUnitID = m_modelSFUnit->data( indUnitId ).toInt();
	}

	if ( RADB::Journal::updateSymptom( iJournalID, iSymptomID, iUnitID, iMachineID, ui.cbSymptom->currentText() ) )
		if ( !ui.cbSymptom->isPopuped() ) 	// ! after resetModel popup widget will hangs up if combobox popuped
			refreshAll();
}

void MainWindow::editStructure()
{
	DlgStructure dlg( this, m_isAdmin );
	dlg.exec();
	refreshAll();
}

void MainWindow::editPassword()
{
	DlgPassword dlg( this );
	dlg.exec();
}

void MainWindow::editNotification()
{
	DlgNotification dlg( this );
	dlg.exec();
}

void MainWindow::drawGraph()
{
	m_plot->clearPlot();
//	p_dock->setWindowTitle( "Гистограмма: " + p_modelJournal->data( p_modelJournal->index( iR, 1 ) ).toString() );
	m_dock->show();
}

void MainWindow::loadAppSettings()
{
	QSettings appSettings( qApp->applicationName() );

	resize( appSettings.value( "MainWindow.Width", width() ).toInt(),
	        appSettings.value( "MainWindow.Height", height() ).toInt() );
	if ( appSettings.value( "MainWindow.Maximized" ).toInt() ) showMaximized();

	bool isFilterVis = appSettings.value( "MainWindow.FilterVisible", false ).toBool();
	ui.actFilter->setChecked( isFilterVis );
	ui.wgtFilter->setVisible( isFilterVis );

	m_udpAlarm->setHost( appSettings.value( "Settings.BCHost", "localhost" ).toString() );
	m_udpAlarm->setPort( appSettings.value( "Settings.BCPort", "45454" ).toInt() );
	m_udpAlarm->setReconnection( appSettings.value( "Settings.BCAutoReconnect", false ).toInt() );

	appSettings.beginGroup( "JournalColumnsWidth" );
	for ( int i = 0; i < JournalModel::clColumnCount; i++ )
		ui.tvJournal->setColumnWidth( i, appSettings.value( QString::number( i ), 120 ).toInt() );
	appSettings.endGroup();

	appSettings.beginGroup( "SolutionColumnsWidth" );
	for ( int i = 0; i < SolutionModel::clColumnCount; i++ )
		ui.tvSolution->setColumnWidth( i, appSettings.value( QString::number( i ), 120 ).toInt() );
	appSettings.endGroup();
}

void MainWindow::saveAppSettings()
{
	QSettings appSettings( qApp->applicationName() );

	appSettings.setValue( "MainWindow.Width", normalGeometry().width() );
	appSettings.setValue( "MainWindow.Height", normalGeometry().height() );
	appSettings.setValue( "MainWindow.Maximized", int( isMaximized() ) );

	appSettings.setValue( "MainWindow.FilterVisible", ( int )ui.actFilter->isChecked() );

	appSettings.beginGroup( "JournalColumnsWidth" );
	for ( int i = 0; i < JournalModel::clColumnCount; i++ )
		appSettings.setValue( QString::number( i ), ui.tvJournal->columnWidth( i ) );
	appSettings.endGroup();

	appSettings.beginGroup( "SolutionColumnsWidth" );
	for ( int i = 0; i < SolutionModel::clColumnCount; i++ )
		appSettings.setValue( QString::number( i ), ui.tvSolution->columnWidth( i ) );
	appSettings.endGroup();
}
