/*
 * dlgstructure.cpp
 *
 *  Created on: 29.09.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "dlgstructure.h"

#include "datatypes.h"
//#include "tablemodel.h"
#include "relationalmodel.h"

DlgStructure::DlgStructure( QWidget * parent, bool /*isAdmin */)
	: QDialog( parent )
	, m_lstModels( QList< QSqlRelationalTableModel * >()
		<< new RelationalModel<RADepartment>( this )
		<< new RelationalModel<RAMachineModel>( this )
		<< new RelationalModel<RAMachine>( this )
		<< new RelationalModel<RAUnit>( this )
		<< new RelationalModel<RASensor>( this )
		<< new RelationalModel<RADetailModel>( this ) )
	, m_proxyModel( new QSortFilterProxyModel( this ) )
{
	ui.setupUi( this );

	ui.lwTableName->addItems( QStringList() << "Цех" << "Модель станка" << "Станок" << "Узел"
					<< "Деталь" << "Модель детали" );
	ui.tvTable->setItemDelegate( new QSqlRelationalDelegate( ui.tvTable ) );

	for ( int i = 0; i < m_lstModels.count(); i++ )
		m_lstModels.at( i )->select();

	connect( ui.btnSave, SIGNAL( clicked() ), this, SLOT( saveChanges() ) );
	connect( ui.btnClose, SIGNAL( clicked() ), this, SLOT( close() ) );
	connect( ui.btnInsertRecord, SIGNAL( clicked() ), this, SLOT( insertRecord() ) );
	connect( ui.btnDeleteRecord, SIGNAL( clicked() ), this,	SLOT( deleteRecord() ) );
	connect( ui.lwTableName, SIGNAL( currentItemChanged( QListWidgetItem * , QListWidgetItem * ) ),
			this, SLOT( tableChanged( QListWidgetItem * , QListWidgetItem * ) ) );

	connect( ui.cbFilter1, SIGNAL( currentIndexChanged( int ) ), this,	SLOT( filter1Change( int ) ) );
	connect( ui.cbFilter2, SIGNAL( currentIndexChanged( int ) ), this,	SLOT( filter2Change( int ) ) );

//	ui.tvTable->setModel( m_proxyModel );
	loadAppSettings();
	ui.lwTableName->setCurrentRow( 0 );
}

void DlgStructure::loadAppSettings()
{
	QSettings appSettings( qApp->applicationName() );

	resize( appSettings.value( "Structure.Width", width() ).toInt(),
			appSettings.value( "Structure.Height", height() ).toInt() );
}

void DlgStructure::loadTableWidths(QSqlTableModel * model)
{
	QSettings appSettings( qApp->applicationName() );
	const QString strName = model->tableName() + "ColumnsWidth";
	const int colCount = model->columnCount();

	appSettings.beginGroup( strName );
	for ( int i = 0; i < colCount; i++ )
		ui.tvTable->setColumnWidth( i, appSettings.value( QString::number( i ), 120 ).toInt() );
	appSettings.endGroup();
}

void DlgStructure::saveTableWidths( QSqlTableModel * model )
{
	QSettings appSettings( qApp->applicationName() );
	const QString strName = model->tableName() + "ColumnsWidth";
	const int colCount = model->columnCount();

	appSettings.beginGroup( strName );
	for ( int i = 0; i < colCount; i++ )
		appSettings.setValue( QString::number( i ), ui.tvTable->columnWidth( i ) );
	appSettings.endGroup();
}

DlgStructure::~DlgStructure()
{
	QSettings appSettings( qApp->applicationName() );

	appSettings.setValue( "Structure.Width", normalGeometry().width() );
	appSettings.setValue( "Structure.Height", normalGeometry().height() );

	switchSaveTableWidths( ui.lwTableName->currentRow() );
	qDeleteAll( m_lstModels );
}

void DlgStructure::switchSaveTableWidths( int index )
{
	if ( index < 0 || index > m_lstModels.count() - 1 )
		return;
	saveTableWidths( m_lstModels.at( index ) );
}

void DlgStructure::tableChanged( QListWidgetItem * current, QListWidgetItem * previous )
{
//	m_proxyModel->setFilterRegExp( QString() );
	switchSaveTableWidths( ui.lwTableName->row( previous ) );

	int index = ui.lwTableName->row( current );
	if ( index < 0 || index > m_lstModels.count() - 1 )
		return;

	ui.tvTable->setModel( m_lstModels.at( index ) );
//	m_proxyModel->setSourceModel( m_lstModels.at( index ) );
	ui.tvTable->hideColumn( 0 );
	switch ( index )
	{
		case mdlSensor: // Sensor
		{
			ui.gbxFilter->setEnabled( true );
			ui.lblFilter1->setWindowTitle( "Модель:" );
			ui.lblFilter2->setWindowTitle( "Узел:" );
			m_modelFilter1 = new RelationalModel< RAMachineModel >( ui.cbFilter1 );
			ui.cbFilter1->setModel( m_modelFilter1 );
			ui.cbFilter1->setModelColumn( mmName );
			m_modelFilter2 = new RelationalModel< RAUnit >( ui.cbFilter2 );
			m_modelFilter1->select();
			m_modelFilter2->select();
			m_modelFilter2->setFilter( QString( "unModelID = %1" ).arg( 0 ) );
			ui.cbFilter2->setModel( m_modelFilter2 );
			ui.cbFilter2->setModelColumn( unShortName );
			m_proxyModel->setFilterKeyColumn( seUnitID );
			m_proxyModel->setDynamicSortFilter( true );
			break;
		}
		default:
		{
			ui.gbxFilter->setEnabled( false );
			break;
		}
	}

	loadTableWidths( m_lstModels.at( index ) );
}

void DlgStructure::saveChanges()
{
	for ( int i = 0; i < m_lstModels.count(); i++ )
		m_lstModels.at( i )->submitAll();

	close();
}

void DlgStructure::insertRecord()
{
	QSqlRelationalTableModel * model = m_lstModels.at( ui.lwTableName->currentRow() );
	int row = model->rowCount();
	model->insertRow( row );

	switch ( ui.lwTableName->currentRow() )
	{
		case mdlSensor:
		{
			int iUnId = m_modelFilter2->index( ui.cbFilter2->currentIndex(), unID ).data().toInt();
			model->setData( model->index( row, seUnitID ), iUnId );
			model->setData( model->index( row, seTypeID ), 1 );
			break;
		}
		default:
			break;
	}
}

void DlgStructure::deleteRecord()
{
	if ( !ui.tvTable->currentIndex().isValid() )
		return;

	const int rowTable = ui.lwTableName->currentRow();
	const int row = ui.tvTable->currentIndex().row();
	QSqlRelationalTableModel * const model = m_lstModels.at( rowTable );
	model->removeRow( row );
}

void DlgStructure::openAndAddSensor()
{
	ui.lwTableName->setCurrentRow( mdlSensor );
	ui.lwTableName->setEnabled( false );
//	ui.cbFilter1->setCurrentIndex( 0 );

//	insertRecord();
//	ui.tvTable->setCurrentIndex( ui.tvTable->model()->index( ui.tvTable->model()->rowCount() - 1, 0 ) );
//	ui.tvTable->scrollToBottom();
}

void DlgStructure::openAndAddDetailModel()
{
	ui.lwTableName->setCurrentRow( mdlDetailModel );
	ui.lwTableName->setEnabled( false );

	insertRecord();
	ui.tvTable->setCurrentIndex( ui.tvTable->model()->index( ui.tvTable->model()->rowCount() - 1, 0 ) );
	ui.tvTable->scrollToBottom();
}

void DlgStructure::filter1Change( int row )
{
	switch ( ui.lwTableName->currentRow() )
	{
		case mdlSensor:
		{
			int iModId = m_modelFilter1->index( row, mmID ).data().toInt();
			if ( iModId >= 0 )
				m_modelFilter2->setFilter( QString( "unModelID = %1" ).arg( iModId ) );
			break;
		}
		default:
			break;
	}
}

void DlgStructure::filter2Change( int row )
{
	switch ( ui.lwTableName->currentRow() )
	{
		case mdlSensor:
		{
			int iUnId = m_modelFilter2->index( row, unID ).data().toInt();
	//		QMessageBox::about( 0, "", QString::number( m_proxyModel->filterKeyColumn() ) );
			m_proxyModel->setFilterKeyColumn( seUnitID );
	//		QMessageBox::about( 0, "", QString::number( m_proxyModel->filterKeyColumn() ) );
			if ( iUnId > 0 )
				m_proxyModel->setFilterRegExp( "\\b" + ui.cbFilter2->currentText() + "\\b" );
			else m_proxyModel->setFilterRegExp( QString() );
			break;
		}
		default:
			break;
	}
}
