/*
 * database.h
 *
 *  Created on: 28.09.2011
 *      Author: Aim
 */

#ifndef DATABASE_H
#define DATABASE_H

#include "journalmodel.h"
#include "solutionmodel.h"
#include "datatypes.h"
//#include "tablemodel.h"

class RADB
{
public:
	static bool setPSQLConnection( const QString & host, const QString & login, const QString & password );
	static void discardPSQLConnection();
	static QString loginQuery();
	static QStringList lstDetailMarks;

	class Symptom
	{
	private:
		static QSqlQueryModel m_model;

	public:
		static QString sqlSelectAll;
		static QString strZeroString;
		static bool refresh();
        static QSqlQueryModel * model() { return &m_model; }
		static int insertUnique( const QString & text, int unitID );
		static int updateUnique( int syID, const QString & text, int unitID );
		static void deleteOne( int symptomID );
		static void deleteQuery();

	private:
		static int p_needDelete;
	};

	class Solution
	{
	public:
		static QString sqlSelectAllBySymptom;
		static QString strZeroString;
		static int insertUnique( const QString & text, int syID, int seID );
		static int updateUnique( int soID, const QString & text, int syID, int seID );
		static void deleteAlone( int soID );
		static void deleteQuery();

	private:
		static int p_needDelete;
	};

	class SolutionGroup
	{
	private:
		static SolutionModel m_model;

	public:
		static QString sqlSelectAllByJournal;
		static bool refresh( int idJournal );
        static SolutionModel *  model() { return &m_model; }
		static int insertNew( int joID, int syID );
		static bool updateType( int sgID, int newType );
		static bool updateDetailModel( int sgID, int newId );
		static bool updateSolution( int sgID, int solID, int senID, int symID, const QString & newText );
		static void deleteOne( int sgID, int soID );
	};

	class Journal
	{
	private:
		static JournalModel m_model;

	public:
		static QString sqlSelectAll;
		static QString sqlSelectAllAfterDT;
		static bool refresh( const QDateTime & dt );
        static JournalModel * model() { return &m_model; }
		static void insertOneByUser( int userId );
		static void insertOneByMachine( int machineId );
		static void deleteOne( int journalID, int symptomID );
		static bool updateErrorDT( int joNumber, const QDateTime & errorDT );
		static bool updateSolveDT( int joNumber, const QDateTime & solveDT );
		static bool updateSpecialist( int joNumber, int indexSp );
		static bool updateMachine( int joNumber, int indexMach );
		static bool updateSymptom( int joNumber, int symptomID, int unitID, int machineID, const QString & symptom );
	};

//	class Notification
//	{
//	public:
//		static QString sqlSelectAll;
//		static QString sqlSelectBySensor;
//		static void updateOne( int noID, const QString & sms, int machineID, int profID );
//	};
};

#endif // DATABASE_H
