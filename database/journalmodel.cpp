/*
 * journalmodel.cpp
 *
 *  Created on: 28.09.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "journalmodel.h"

#include "database.h"

JournalModel::JournalModel( QObject * parent )
    : QSqlQueryModel( parent )
{
	setHeaderData( clID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
	setHeaderData( clErrorDT, Qt::Horizontal, QString::fromLocal8Bit( "Вызов" ) );
	setHeaderData( clSensorName, Qt::Horizontal, QString::fromLocal8Bit( "Оборудование" ) );
	setHeaderData( clSymptomText, Qt::Horizontal, QString::fromLocal8Bit( "Описание проблемы" ) );
	setHeaderData( clInterval, Qt::Horizontal, QString::fromLocal8Bit( "Простой" ) );
	setHeaderData( clSolveDT, Qt::Horizontal, QString::fromLocal8Bit( "Время наладки" ) );
	setHeaderData( clSpecFIO, Qt::Horizontal, QString::fromLocal8Bit( "Специалист" ) );
	setHeaderData( clSolutions, Qt::Horizontal, QString::fromLocal8Bit( "Выполненные работы" ) );
}

Qt::ItemFlags JournalModel::flags( const QModelIndex & /*index*/ ) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


QVariant JournalModel::data( const QModelIndex & index, int role ) const
{
	QVariant var = QSqlQueryModel::data( index, role );
	if ( role == Qt::DisplayRole )
		switch ( index.column() )
		{
		case clErrorDT:
		case clSolveDT:
			return var.toDateTime();
			break;
		case clInterval:
		{
			if ( var.toString().trimmed().startsWith( '-' ) )
				return "Не задано";
			else
			{
				QString strRes = var.toString().replace( "days", "д").replace( "day", "д" );
				strRes.chop( 3 );
				strRes = strRes.replace( ":", "ч " ).replace( "00ч ", "" );
				if ( strRes == "00" ) return "Без простоя";
				strRes.append( "м" );

				return strRes;
			}
		}
		case clSolutions:
			return var.toString().replace( "***", ";\n");
		case clSensorName:
			return var.toString().replace( ": ", ":\n" );
		break;
		default:
			return var.toString();
		}
	return var;
}

bool JournalModel::setData( const QModelIndex & index, const QVariant & value, int /*role*/ )
{
	int iJoID = data( this->index( index.row(), clID ) ).toInt();
	switch ( index.column() )
	{
	case clErrorDT:
		return RADB::Journal::updateErrorDT( iJoID, value.toDateTime() );
	case clSolveDT:
		return RADB::Journal::updateSolveDT( iJoID, value.toDateTime() );
	case clSpecID:
		return RADB::Journal::updateSpecialist( iJoID, value.toInt() );

	default:
		return true;
	}
	return false;
}


JournalDelegate::JournalDelegate( const QString & color, QObject * parent )
    : QItemDelegate( parent ),
      p_color( color )
{
}

void JournalDelegate::paint( QPainter *painter,
	const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	painter->save();

	if ( hasClipping() )
		painter->setClipRect( option.rect );
	QString strColor = p_color;
	painter->setPen( QColor( strColor ) );
	painter->setBrush( QBrush( QColor( strColor ) ) );
	painter->drawRect( option.rect );
	if ( option.state & QStyle::State_Selected )
	{
		QColor colorSel( "#78cbf6" );
		colorSel.setAlpha( 140 );
		painter->setPen( colorSel );
		painter->setBrush( QBrush( colorSel ) );
		painter->drawRect( option.rect );
	}

	painter->setPen( Qt::black );
	painter->setFont( QFont( "Segoe UI", 8 ) );
	QRect textRect( option.rect.x() + 5, option.rect.y() + 3, option.rect.width() - 10, option.rect.height() - 6 ) ;

	QString strText = index.model()->data( index ).toString();
	if ( index.model()->data( index ).type() == QVariant::DateTime )
		strText = index.model()->data( index ).toDateTime().toString( "dd MMM yyyy\nHH:mm" );
	painter->drawText( textRect, Qt::TextWordWrap | Qt::AlignLeft | Qt::AlignVCenter,
			strText );

	painter->restore();
}
