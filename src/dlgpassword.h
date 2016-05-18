/* dlgpassword.h
 *
 *  Created on: 04.10.2011
 *      Author: Aim
 */

#ifndef DLGPASSWORD_H
#define DLGPASSWORD_H

#include <QWidget>
#include "ui_dlgpassword.h"

#include "datatypes.h"
#include <QDataWidgetMapper>

template < typename T >
class RelationalModel;

class DlgPassword : public QDialog
{
	Q_OBJECT

public:
	DlgPassword( QWidget * parent = 0 );

private:
	Ui::DlgPasswordClass ui;
    RelationalModel< RASpecialist > * m_modelSpec;
    QDataWidgetMapper * m_mapper;

private slots:
	void savePass();
};

#endif // DLGPASSWORD_H
