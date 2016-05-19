/*
 * solutionmodel.h
 *
 *  Created on: 03.11.2011
 *      Author: Aim
 */

#ifndef SOLUTIONMODEL_H
#define SOLUTIONMODEL_H

#include <QSqlQueryModel>
#include <QComboBox>
#include <QItemEditorFactory>

#include "datatypes.h"

template < typename T >
class RelationalModel;

class SolutionModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    SolutionModel( QObject *parent = 0);

    enum Columns {
        clID = 0, clType, clSolutionID, clText, clSensorID, clSensorName, clSymptomID, clJournalID, clDetailModelID, clDModelName,
        clColumnCount
    };
    static const QStringList m_lstTypes;

    virtual int	columnCount ( const QModelIndex & /*parent = QModelIndex() */) const { return clColumnCount; }
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    void setUnitID( int newID ) { m_unitID = newID; }

private:
    int m_unitID;
};

struct TSolutionType { QString value; int index; };
Q_DECLARE_METATYPE ( TSolutionType )

struct TSolutionText { QString value; int index; };
Q_DECLARE_METATYPE ( TSolutionText )

struct TSensor { QString value; int index; };
Q_DECLARE_METATYPE ( TSensor )

struct TDetailModel { QString value; int index; };
Q_DECLARE_METATYPE ( TDetailModel )

template < typename T >		// template classes not supported by Q_OBJECT
class SQLColumnProperty : public QComboBox
{
public:
    SQLColumnProperty( QWidget * parent ) : QComboBox( parent ) { }

    T property() const
    {
        T tc = { currentText(), this->itemData( currentIndex(), Qt::UserRole ).toInt() };
        return tc;
    }

    void setProperty( T prop )
    {
        int iInd = findData( prop.value, int( Qt::DisplayRole ) );
        setCurrentIndex( qMax( iInd, 0 ) );
    }
};

class SQLSolutionTypeEditor : public SQLColumnProperty< TSolutionType >
{
    Q_OBJECT
    Q_PROPERTY( TSolutionType sqlProperty READ property WRITE setProperty USER true )
public:
    SQLSolutionTypeEditor( QWidget * parent ) : SQLColumnProperty< TSolutionType >( parent ) { }
};

class SQLSolutionTextEditor : public SQLColumnProperty< TSolutionText >
{
    Q_OBJECT
    Q_PROPERTY( TSolutionText sqlProperty READ property WRITE setProperty USER true )
public:
    SQLSolutionTextEditor( QWidget * parent ) : SQLColumnProperty< TSolutionText >( parent )
    {
        setEditable( true );
    }

    void setProperty( TSolutionText prop )
    {
        setEditText( prop.value );
        //		int iInd = findData( prop.value, int( Qt::DisplayRole ) );
        //		setCurrentIndex( qMax( iInd, 0 ) );
    }
};

class SQLSensorEditor : public SQLColumnProperty< TSensor >
{
    Q_OBJECT
    Q_PROPERTY( TSensor sqlProperty READ property WRITE setProperty USER true )
public:
    SQLSensorEditor( QWidget * parent ) : SQLColumnProperty< TSensor >( parent ) { }
};

class SQLDetailModelEditor : public SQLColumnProperty< TDetailModel >
{
    Q_OBJECT
    Q_PROPERTY( TDetailModel sqlProperty READ property WRITE setProperty USER true )
public:
    SQLDetailModelEditor( QWidget * parent ) : SQLColumnProperty< TDetailModel >( parent ) { }
};

class SolutionEditorFactory : public QItemEditorFactory
{
public:
    explicit SolutionEditorFactory( const QItemEditorFactory * standardFactory );
    virtual QWidget * createEditor( QVariant::Type type, QWidget* parent ) const;

    void setSymptomID( int newID ) { m_symptomID = newID; }
    void setUnitID( int newID ) { m_unitID = newID; }

private:
    const QItemEditorFactory * p_standardFactory;
    int m_symptomID, m_unitID;
};

#endif // SOLUTIONMODEL_H
