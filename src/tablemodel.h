/*
 * database.h
 *
 *  Created on: 15.11.2011
 *      Author: Aim
 */

#ifndef TABLEMODEL_H
#define TABLEMODEL_H

class QSqlQueryModel;

template < typename T >
class TableModel : public QSqlQueryModel
{
public:
	TableModel( QObject * parent = 0) : QSqlQueryModel( parent ) { }

	inline QVariant data( int row, T col );
	bool refresh();
	inline bool updateField( int idRecord, T col, QString value );
	bool updateRecord( int idRecord, QMap< T, QVariant > map );

	int insertRecord();
	int insertRecord( int idParent );
	bool deleteRecord( int id );

private:
	static QString m_tableName;
	static QStringList m_fieldNames;

	bool boolQuery( const QString & strQuery );
	int intQuery( const QString & strQuery );
};


template< typename T >
inline bool TableModel< T >::boolQuery( const QString & strQuery )
{
	setQuery( strQuery );
	if ( lastError().isValid() )
		QMessageBox::about( 0, "", lastError().databaseText() );
	return !lastError().isValid();
}

template< typename T >
int TableModel< T >::intQuery( const QString & strQuery )
{
	QSqlQuery query( strQuery );
	if ( query.lastError().isValid() )
	{
		QMessageBox::about( 0, "", query.lastError().databaseText() );
		return -1;
	}
	return query.value( 0 ).toInt();
}

template< typename T >
inline QVariant TableModel< T >::data( int row, T col )
{
	return this->QSqlQueryModel::data( index( row, int( col ) ) );
}

template< typename T >
inline bool TableModel< T >::updateField( int idRecord, T col, QString value )
{
	return boolQuery( QString( "UPDATE %1 SET %2 = '%3' WHERE %4 = %5" )
		.arg( m_tableName, m_fieldNames.at( int( col ) ), value, m_fieldNames.at( 0 ) ).arg( idRecord ) );
}

#endif // TABLEMODEL_H
