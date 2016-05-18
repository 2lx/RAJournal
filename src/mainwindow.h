/*
 * mainwindow.h
 *
 *  Created on: 28.09.2011
 *      Author: Aim
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include "ui_mainwindow.h"

#include "datatypes.h"

QT_BEGIN_NAMESPACE
class QSqlQueryModel;
class QDockWidget;
class QSortFilterProxyModel;
QT_END_NAMESPACE

class JournalModel;
class SolutionModel;
class SolutionEditorFactory;
class BodePlot;
class UdpAlarm;
template < typename T >
class TableModel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow( QWidget *parent = 0 );
	~MainWindow();
	void setUserId( int newId, const QString & strName, bool isAdmin = false );
	void refreshStructure();
	void refreshDepartment();

public slots:
	void refreshAll();
	void changeUser();

private:
	Ui::MainWindow ui;

	TableModel< RASpecialist > * const m_modelSpecialist;
	TableModel< RADepartment > * const m_modelDepartment;
	TableModel< RAMachineModel > * const m_modelMachineModel;
	TableModel< RAMachine > * const m_modelMachine;
	TableModel< RAUnit > * const m_modelUnit;
//	TableModel< RASensor > * const m_modelSensor;
//	TableModel< RADetailModel > * const m_modelDetailModel;

	QSortFilterProxyModel * const m_modelSFJournal;
	QSortFilterProxyModel * const m_modelSFMachine;
	QSortFilterProxyModel * const m_modelSFUnit;
	QSortFilterProxyModel * const m_modelSFSymptom;
	QSortFilterProxyModel * const m_modelSFSolution;

	SolutionEditorFactory * m_solutionEditor;
	QDockWidget * const m_dock;
	BodePlot * const m_plot;
	int m_curUserId;
	bool m_isAdmin;
	int m_prevCurIndex;
	int m_prevJournalID;
	UdpAlarm * m_udpAlarm;

	union Flags {
		unsigned int number;
		struct {
			unsigned bUnit : 1;
			unsigned bMachine : 1;
			unsigned bDepartment : 1;
			unsigned bErrorDT : 1;
			unsigned bSolveDT : 1;
			unsigned bSpecialist : 1;
			unsigned bSymptom : 1;
			unsigned Solution : 1;
		} bits;
	};
	Flags m_enbStruct;
	Flags m_enbUpdate;

	void loadAppSettings();
	void saveAppSettings();
	int getCBIndex( int iRow, int iCol, QAbstractItemModel * const model ) const;
	bool isOpenRow( int indexRow );

private slots:
	void refreshMainView();
	void curRecordChanged();
	void addRecord();
	void deleteRecord();
	void addSolution();
	void deleteSolution();
	void updateErrorDT();
	void updateSolveDT();
	void updateSpecialist( int index );
	void updateSymptom();

	void departmentChanged();
	void machineChanged();
	void unitChanged();
	void symptomChanged();

	void editStructure();
	void editPassword();
	void editNotification();
	void setEndTime();

	void drawGraph();
	void setupRowHighlight();

	void setEditHide();
	void udpSignalReceived();

	void editSettings();
};

#endif  // MAINWINDOW_H
