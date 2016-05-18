/* dlgnotification.h
 *
 *  Created on: 28.10.2011
 *      Author: Aim
 */

#ifndef DLGNOTIFICATION_H
#define DLGNOTIFICATION_H

#include <qdialog.h>
#include "ui_dlgnotification.h"

#include "datatypes.h"

class NotificationModel;
class ProfessionModel;
template < typename T > class RelationalModel;

class DlgNotification : public QDialog
{
	Q_OBJECT

public:
	DlgNotification( QWidget * parent = 0 );
	virtual ~DlgNotification();

private:
	Ui::DlgNotificationClass ui;
	RelationalModel< RANotification > * const m_modelNotification;
	RelationalModel< RAProfession > * const m_modelProfession;

	void loadAppSettings();

private slots:
	void saveChanges();
};

#endif // DLGNOTIFICATION_H
