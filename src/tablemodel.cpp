/*
 * database.cpp
 *
 *  Created on: 15.11.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "tablemodel.h"

#include "datatypes.h"

template< typename T >
bool TableModel< T >::deleteRecord( int id )
{
	return boolQuery( QString( "DELETE FROM %1 WHERE %2 = %3" ).arg( m_tableName ).arg( m_fieldNames.at( 0 ) ).arg( id ) );
}

template< typename T >
bool TableModel< T >::updateRecord( int idRecord, QMap< T, QVariant > map )
{
	QString strSql = "";
	for ( int i = 0; i < map.count(); i++ )
	{
		switch ( map.values().at( i ).type() )
		{
			case QVariant::Int :
			{
				strSql += m_fieldNames.at( int( map.keys().at( i ) ) ) + " = " + map.values().at( i ).toString();
				break;
			}
			default:
			{
				strSql += m_fieldNames.at( int( map.keys().at( i ) ) ) + " = '" + map.values().at( i ).toString() + "'";
				break;
			}
		}
		if ( i < map.count() - 1 ) strSql += ",";
	}

	return boolQuery( QString( "UPDATE %1 SET %2 WHERE %4 = %5" )
		.arg( m_tableName, strSql, m_fieldNames.at( 0 ) ).arg( idRecord ) );
}

template< typename T >
bool TableModel< T >::refresh()
{
	return boolQuery( QString( "SELECT %1 FROM %2 ORDER BY %3" )
		.arg( m_fieldNames.join( "," ) ).arg( m_tableName ).arg( m_fieldNames.at( 0 ) ) );
}

template<>
bool TableModel< RASpecialist >::refresh()
{
	return boolQuery( "SELECT * FROM v_specialist" );
}

template<>
int TableModel< RADepartment >::insertRecord()
{
	return intQuery( "INSERT INTO department (dpNumber, dpName) VALUES( 0, 'Новый цех' ) RETURNING dpID" );
}

template<>
int TableModel< RAMachineModel >::insertRecord()
{
	return intQuery( "INSERT INTO machinemodel (mmName) VALUES( 'Новая модель' ) RETURNING mmID" );
}

template<>
int TableModel< RAMachine >::insertRecord( int idParent )
{
	return intQuery( QString( "INSERT INTO machine (maDepartmentID, maFullName, maShortName, maImportance) "
			" VALUES( %1, 'Новый станок', 'Новый станок', 5 ) RETURNING maID" ).arg( idParent ) );
}

template<>
int TableModel< RAUnit >::insertRecord( int idParent )
{
	return intQuery( QString( "INSERT INTO unit (unModelID, unFullName, unShortName) "
			" VALUES( %1, 'Новый узел', 'Новый узел' ) RETURNING unID" ).arg( idParent ) );
}

template<>
int TableModel< RASensor >::insertRecord( int idParent )
{
	return intQuery( QString( "INSERT INTO sensor (seUnitID, seFullName, seShortName, seType, seProgramName, seSchemeName) "
			" VALUES( %1, 'Новая деталь', 'Новая деталь', 1, '', '' ) RETURNING seID" ).arg( idParent ) );
}

template<>
int TableModel< RADetailModel >::insertRecord()
{
	return intQuery( "INSERT INTO detailModel (dmName, dmType) VALUES( 'Новая марка', 0 ) RETURNING dmID" );
}


// explicit static initialization !
template<>
QString TableModel< RASpecialist >::m_tableName = "specialist";
template<>
QString TableModel< RADepartment >::m_tableName = "department";
template<>
QString TableModel< RAMachineModel >::m_tableName = "machinemodel";
template<>
QString TableModel< RAMachine >::m_tableName = "machine";
template<>
QString TableModel< RAUnit >::m_tableName = "unit";
template<>
QString TableModel< RASensor >::m_tableName = "sensor";
template<>
QString TableModel< RADetailModel >::m_tableName = "detailmodel";

template<>
QStringList TableModel< RASpecialist >::m_fieldNames = QStringList() << "dpID" << "dpShortName" << "dpFullName";
template<>
QStringList TableModel< RADepartment >::m_fieldNames = QStringList() << "dpID" << "dpShortName" << "dpFullName";
template<>
QStringList TableModel< RAMachineModel >::m_fieldNames = QStringList() << "mmID" << "mmName";
template<>
QStringList TableModel< RAMachine >::m_fieldNames = QStringList() << "maID" << "maShortName" << "maFullName"
		<< "maDepartmentID" << "maModelID";
template<>
QStringList TableModel< RAUnit >::m_fieldNames = QStringList() <<  "unID" << "unShortName" << "unFullName" << "unModelID";
template<>
QStringList TableModel< RASensor >::m_fieldNames = QStringList() << "seID" << "seShortName" << "seFullName"
		<< "seUnitID" << "seTypeID" << "seProgramName" << "seSchemeName";
template<>
QStringList TableModel< RADetailModel >::m_fieldNames = QStringList() << "dmID" << "dmName" << "dmTypeID";

// explicit instantiation !
template class TableModel< RASpecialist >;
template class TableModel< RADepartment >;
template class TableModel< RAMachineModel >;
template class TableModel< RAMachine >;
template class TableModel< RAUnit >;
template class TableModel< RASensor >;
template class TableModel< RADetailModel >;

