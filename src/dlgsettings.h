/* dlgsettings.h
 *
 *  Created on: 28.10.2011
 *      Author: Aim
 */

#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QWidget>
#include "ui_dlgsettings.h"

class DlgSettings : public QDialog
{
	Q_OBJECT

public:
	DlgSettings( QWidget * parent = 0 );
	QString host() const { return ui.leBCHost->text(); }
	int port() const { return ui.spbBCPort->value(); }
	bool autoReconnect() const { return ui.cbBCAutoReconnect->isChecked(); }

private:
	Ui::DlgSettingsClass ui;

private slots:
	void save();
};

#endif // DLGSETTINGS_H
