/*
 * QComboBoxKeyboard.h
 *
 *  Created on: 07.11.2011
 *      Author: Aim 
 */

#ifndef QCOMBOBOXKEYBOARD_H_
#define QCOMBOBOXKEYBOARD_H_

#include <qcombobox.h>

class QComboBoxKeyboard : public QComboBox
{
	Q_OBJECT

public:
    QComboBoxKeyboard( QWidget * /*parent = 0*/ ) { }
	bool isPopuped() const { return m_isShowingPopup; }

	virtual void showPopup()
	{
		m_isShowingPopup = true;
		QComboBox::showPopup();
	}

	virtual void hidePopup()
	{
		m_isShowingPopup = false;
		QComboBox::hidePopup();
	}

protected:
//	virtual void wheelEvent( QWheelEvent * e ) { }

private:
	bool m_isShowingPopup;
};

#endif /* QCOMBOBOXKEYBOARD_H_ */
