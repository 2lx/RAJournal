/*
 * database.cpp
 *
 *  Created on: 15.11.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "relationalmodel.h"

#include "datatypes.h"

/*
 * RelationalTableModel< RANotification >
 */

template<>
RelationalModel< RANotification >::RelationalModel( QObject * parent, QSqlDatabase db )
	: QSqlRelationalTableModel( parent, db ),
	  m_lstEditableFields( QList< RANotification >() << noSms << noMachineID << noProfessionID )
{
	setTable( "notification" );
	setEditStrategy( QSqlTableModel::OnManualSubmit );
	setRelation( noProfessionID, QSqlRelation( "profession", "prID", "prName" ) );
	setRelation( noMachineID, QSqlRelation( "machine", "maID", "maShortName" ) );

	setHeaderData( noID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
	setHeaderData( noSensors, Qt::Horizontal, QString::fromLocal8Bit( "Сигналы" ) );
	setHeaderData( noSms, Qt::Horizontal, QString::fromLocal8Bit( "СМС" ) );
	setHeaderData( noMachineID, Qt::Horizontal, QString::fromLocal8Bit( "Станок" ) );
	setHeaderData( noProfessionID, Qt::Horizontal, QString::fromLocal8Bit( "Профессия" ) );

	setSort( noID, Qt::AscendingOrder );
}

/*
 * RelationalTableModel< RAProfession >
 */

template<>
RelationalModel< RAProfession >::RelationalModel( QObject * parent, QSqlDatabase db )
	: QSqlRelationalTableModel( parent, db ),
	  m_lstEditableFields( QList< RAProfession >() << prPhone )
{
	setTable( "profession" );
	setEditStrategy( QSqlTableModel::OnManualSubmit );

	setHeaderData( prID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
	setHeaderData( prName, Qt::Horizontal, QString::fromLocal8Bit( "Профессия" ) );
	setHeaderData( prPhone, Qt::Horizontal, QString::fromLocal8Bit( "Телефон" ) );

	setSort( prID, Qt::AscendingOrder );
}

/*
 * RelationalTableModel< RASpecialist >
 */

template<>
RelationalModel< RASpecialist >::RelationalModel( QObject * parent, QSqlDatabase db )
	: QSqlRelationalTableModel( parent, db ),
	  m_lstEditableFields( QList< RASpecialist >() << spPassword )
{
	setTable( "specialist" );
	setEditStrategy( QSqlTableModel::OnManualSubmit );
	setRelation( spProfessionID, QSqlRelation( "profession", "prID", "prName" ) );

	setHeaderData( spID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
	setHeaderData( spFIO, Qt::Horizontal, QString::fromLocal8Bit( "ФИО" ) );
	setHeaderData( spPassword, Qt::Horizontal, QString::fromLocal8Bit( "Пароль" ) );
	setHeaderData( spProfessionID, Qt::Horizontal, QString::fromLocal8Bit( "Профессия" ) );

	setSort( spFIO, Qt::AscendingOrder );
}

template<>
QString	RelationalModel< RASpecialist >::selectStatement () const
{
	return "SELECT * FROM v_specialist";
}

template<>
QVariant RelationalModel< RASpecialist >::data( const QModelIndex & index, int role ) const
{
	switch ( index.column() )
	{
		case spPassword:
			return QString();
		default:
			return QSqlRelationalTableModel::data( index, role );
	}
}

template<>
bool RelationalModel< RASpecialist >::setData( const QModelIndex & index, const QVariant & value, int role )
{
	switch ( index.column() )
	{
		case spPassword:
		{
			if ( !value.toString().trimmed().isEmpty() )
			{
				QString strHash = QString::fromAscii( QCryptographicHash::hash(
								value.toString().toAscii(), QCryptographicHash::Md5 ).toHex() );
				return QSqlRelationalTableModel::setData( index, strHash, role );
			}
			else return false;
		}
		default:
			return QSqlRelationalTableModel::setData( index, value, role );
	}
}

/*
 * RelationalTableModel< RADepartment >
 */

template<>
RelationalModel< RADepartment >::RelationalModel( QObject * parent, QSqlDatabase db )
	: QSqlRelationalTableModel( parent, db ),
	  m_lstEditableFields( QList< RADepartment >() << dpShortName << dpFullName )
{
	setTable( "department" );
	setEditStrategy( QSqlTableModel::OnManualSubmit );

	setHeaderData( dpID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
	setHeaderData( dpShortName, Qt::Horizontal, QString::fromLocal8Bit( "Короткое название" ) );
	setHeaderData( dpFullName, Qt::Horizontal, QString::fromLocal8Bit( "Полное название" ) );

	setSort( dpID, Qt::AscendingOrder );
}

/*
 * RelationalTableModel< RAMachineModel >
 */

template<>
RelationalModel< RAMachineModel >::RelationalModel( QObject * parent, QSqlDatabase db )
	: QSqlRelationalTableModel( parent, db ),
	  m_lstEditableFields( QList< RAMachineModel >() << mmName )
{
	setTable( "machinemodel" );
	setEditStrategy( QSqlTableModel::OnManualSubmit );

	setHeaderData( mmID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
	setHeaderData( mmName, Qt::Horizontal, QString::fromLocal8Bit( "Название" ) );

	setSort( mmID, Qt::AscendingOrder );
}

/*
 * RelationalTableModel< RAMachine >
 */

template<>
RelationalModel< RAMachine >::RelationalModel( QObject * parent, QSqlDatabase db )
	: QSqlRelationalTableModel( parent, db ),
	  m_lstEditableFields( QList< RAMachine >() << maShortName << maFullName << maImportance << maModelID << maDepartmentID )
{
	setTable( "machine" );
	setEditStrategy( QSqlTableModel::OnManualSubmit );
	setRelation( maModelID, QSqlRelation( "machinemodel", "mmID", "mmName" ) );
	setRelation( maDepartmentID, QSqlRelation( "department", "dpID", "dpShortName" ) );

	setHeaderData( maID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
	setHeaderData( maShortName, Qt::Horizontal, QString::fromLocal8Bit( "Короткое название" ) );
	setHeaderData( maFullName, Qt::Horizontal, QString::fromLocal8Bit( "Полное название" ) );
	setHeaderData( maImportance, Qt::Horizontal, QString::fromLocal8Bit( "Важность" ) );
	setHeaderData( maDepartmentID, Qt::Horizontal, QString::fromLocal8Bit( "Цех" ) );
	setHeaderData( maModelID, Qt::Horizontal, QString::fromLocal8Bit( "Модель станка" ) );

	setSort( maID, Qt::AscendingOrder );
}

/*
 * RelationalTableModel< RAUnit >
 */

template<>
RelationalModel< RAUnit >::RelationalModel( QObject * parent, QSqlDatabase db )
	: QSqlRelationalTableModel( parent, db ),
	  m_lstEditableFields( QList< RAUnit >() << unShortName << unFullName << unModelID )
{
	setTable( "unit" );
	setEditStrategy( QSqlTableModel::OnManualSubmit );
	setRelation( unModelID, QSqlRelation( "machinemodel", "mmID", "mmName" ) );

	setHeaderData( unID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
	setHeaderData( unShortName, Qt::Horizontal, QString::fromLocal8Bit( "Короткое название" ) );
	setHeaderData( unFullName, Qt::Horizontal, QString::fromLocal8Bit( "Полное название" ) );
	setHeaderData( unModelID, Qt::Horizontal, QString::fromLocal8Bit( "Модель станка" ) );

	setSort( unID, Qt::AscendingOrder );
}

/*
 * RelationalTableModel< RASensor >
 */

template<>
RelationalModel< RASensor >::RelationalModel( QObject * parent, QSqlDatabase db )
	: QSqlRelationalTableModel( parent, db ),
	  m_lstEditableFields( QList< RASensor >() << seShortName << seFullName << seUnitID << seTypeID << seProgramName << seSchemeName )
{
	setTable( "sensor" );
	setEditStrategy( QSqlTableModel::OnManualSubmit );
	setRelation( seTypeID, QSqlRelation( "sensortype", "stID", "stName" ) );
	setRelation( seUnitID, QSqlRelation( "unit", "unID", "unShortName" ) );

	setHeaderData( seID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
	setHeaderData( seShortName, Qt::Horizontal, QString::fromLocal8Bit( "Короткое название" ) );
	setHeaderData( seFullName, Qt::Horizontal, QString::fromLocal8Bit( "Полное название" ) );
	setHeaderData( seUnitID, Qt::Horizontal, QString::fromLocal8Bit( "Узел" ) );
	setHeaderData( seTypeID, Qt::Horizontal, QString::fromLocal8Bit( "Тип" ) );
	setHeaderData( seProgramName, Qt::Horizontal, QString::fromLocal8Bit( "Название в программе" ) );
	setHeaderData( seSchemeName, Qt::Horizontal, QString::fromLocal8Bit( "Название в схеме" ) );

	setSort( seID, Qt::AscendingOrder );
}

/*
 * RelationalTableModel< RADetailModel >
 */

template<>
RelationalModel< RADetailModel >::RelationalModel( QObject * parent, QSqlDatabase db )
	: QSqlRelationalTableModel( parent, db ),
	  m_lstEditableFields( QList< RADetailModel >() << dmName << dmTypeID )
{
	setTable( "detailmodel" );
	setEditStrategy( QSqlTableModel::OnManualSubmit );
	setRelation( dmTypeID, QSqlRelation( "detailmodeltype", "dtID", "dtName" ) );

	setHeaderData( dmID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
	setHeaderData( dmName, Qt::Horizontal, QString::fromLocal8Bit( "Название" ) );
	setHeaderData( dmTypeID, Qt::Horizontal, QString::fromLocal8Bit( "Тип" ) );

	setSort( dmID, Qt::AscendingOrder );
}

// explicit instantiation !
template class RelationalModel< RASpecialist >;
template class RelationalModel< RADepartment >;
template class RelationalModel< RAMachineModel >;
template class RelationalModel< RAMachine >;
template class RelationalModel< RAUnit >;
template class RelationalModel< RASensor >;
template class RelationalModel< RADetailModel >;
