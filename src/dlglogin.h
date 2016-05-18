/* dlglogin.h
 *
 *  Created on: 28.09.2011
 *      Author: Aim
 */

#ifndef DLGLOGIN_H
#define DLGLOGIN_H

#include <QWidget>
#include "ui_dlglogin.h"
#include <QSqlQueryModel>

class DlgLogin : public QDialog
{
	Q_OBJECT

public:
	DlgLogin( QWidget * parent = 0 );
	inline int userId() const { return p_userId; }
	inline QString userName() const { return p_userName; }
	inline bool isAdmin() const { return p_isAdmin; }

protected:
	virtual void closeEvent ( QCloseEvent * event );

private:
	Ui::DlgLoginClass ui;
    QSqlQueryModel * p_modelSpec;
	int p_userId;
	QString p_userName;
	bool p_isAdmin;

private slots:
	void connectToDB( bool isAdmin );
	void checkPass();
};

#endif // DLGLOGIN_H
