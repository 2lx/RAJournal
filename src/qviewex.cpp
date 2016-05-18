/*
 * QViewEx.cpp
 *
 *  Created on: 04.10.2011
 *      Author: Aim
 */

#include "qviewex.h"
#include <QMouseEvent>

QListViewEx::QListViewEx( QWidget * parent ) : QListView( parent ), m_lastCurRow( -1 )
{

}

void QListViewEx::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{
	emit curIndexChanged();
	m_lastCurRow = currentIndex().row();
	QListView::selectionChanged( selected, deselected );
}

void QListViewEx::mouseMoveEvent( QMouseEvent * event )
{
	if ( event->buttons() & Qt::LeftButton )
	{
		int iNewR = currentIndex().row();
		if ( iNewR != m_lastCurRow )
			emit curIndexChanged();
		m_lastCurRow = iNewR;
	}
	QListView::mouseMoveEvent( event );
}

/******************************************************************************
 * TableView
 ******************************************************************************/

QTableViewEx::QTableViewEx( QWidget * parent ) : QTableView( parent )
{

}

void QTableViewEx::selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected )
{
	int iNewR = currentIndex().row();
	if ( iNewR != m_lastCurRow )
		emit curIndexChanged();
	m_lastCurRow = iNewR;
	QTableView::selectionChanged( selected, deselected );
}

void QTableViewEx::mouseMoveEvent( QMouseEvent * event )
{
    if ( event->buttons() & Qt::LeftButton )
	{
		int iNewR = currentIndex().row();
		if ( iNewR != m_lastCurRow )
			emit curIndexChanged();
		m_lastCurRow = iNewR;
	}
	QTableView::mouseMoveEvent( event );
}


