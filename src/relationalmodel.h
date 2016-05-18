/*
 * database.h
 *
 *  Created on: 15.11.2011
 *      Author: Aim
 */

#ifndef RELATIONALMODEL_H
#define RELATIONALMODEL_H

class QSqlRelationalTableModel;

template < typename T >
class RelationalModel : public QSqlRelationalTableModel
{
private:
	const QList< T > m_lstEditableFields;

protected:
	virtual QString	selectStatement () const
	{
		return QSqlRelationalTableModel::selectStatement();
	}

public:
	RelationalModel( QObject * parent = 0, QSqlDatabase db = QSqlDatabase() );

	virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const
	{
		return QSqlRelationalTableModel::data( index, role );
	}

	virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole )
	{
		return QSqlRelationalTableModel::setData( index, value, role );
	}

	virtual Qt::ItemFlags flags( const QModelIndex &index ) const
	{
		if ( m_lstEditableFields.contains( T( index.column() ) )
				&& data( this->index( index.row(), 0 ) ).toString() != "0" )
			return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
		else return Qt::ItemIsSelectable | Qt::ItemIsEnabled ;
	}
};

#endif // RELATIONALMODEL_H
