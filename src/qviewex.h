/*
 * QViewEx.h
 *
 *  Created on: 04.10.2011
 *      Author: Aim
 */

#ifndef QLISTVIEWEX_H_
#define QLISTVIEWEX_H_

#include <QListView>
#include <QTableView>

class QListViewEx : public QListView
{
	Q_OBJECT
public:
	QListViewEx( QWidget * parent = 0 );
	virtual ~QListViewEx() {}

protected:
	virtual void selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );
	virtual void mouseMoveEvent ( QMouseEvent * event );

private:
	int m_lastCurRow;

signals:
	void curIndexChanged();
};

class QTableViewEx : public QTableView
{
	Q_OBJECT
public:
	QTableViewEx( QWidget * parent = 0 );
	virtual ~QTableViewEx() {}

protected:
	virtual void selectionChanged ( const QItemSelection & selected, const QItemSelection & deselected );
	virtual void mouseMoveEvent ( QMouseEvent * event );

private:
	int m_lastCurRow;

signals:
	void curIndexChanged();
};


#endif /* QLISTVIEWEX_H_ */
