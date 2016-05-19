/* dlgstructure.h
 *
 *  Created on: 28.09.2011
 *      Author: Aim
 */

#ifndef DLGSTRUCTURE_H
#define DLGSTRUCTURE_H

#include <QWidget>
#include "ui_dlgstructure.h"

//#include "datatypes.h"

QT_BEGIN_NAMESPACE
class QSqlTableModel;
class QSqlRelationalTableModel;
class QSortFilterProxyModel;
QT_END_NAMESPACE

class DlgStructure : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStructure( QWidget * parent, bool isAdmin );
    ~DlgStructure();

    void openAndAddSensor();
    void openAndAddDetailModel();

private:
    Ui::DlgStructureClass ui;
    QSqlRelationalTableModel * m_modelFilter1;
    QSqlRelationalTableModel * m_modelFilter2;

    enum { mdlDepartment = 0, mdlMachineModel, mdlMachine, mdlUnit, mdlSensor, mdlDetailModel };
    QList< QSqlRelationalTableModel * > m_lstModels;
    QSortFilterProxyModel * m_proxyModel;

    void loadAppSettings();
    void loadTableWidths( QSqlTableModel * model );
    void saveTableWidths( QSqlTableModel * model );
    void switchSaveTableWidths( int index );

private slots:
    void saveChanges();
    void tableChanged( QListWidgetItem * current, QListWidgetItem * previous );
    void insertRecord();
    void deleteRecord();
    void filter1Change( int row );
    void filter2Change( int row );
};

#endif // DLGSTRUCTURE_H
