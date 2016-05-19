/*
 * journalmodel.h
 *
 *  Created on: 28.09.2011
 *      Author: Aim
 */

#ifndef JOURNALMODEL_H
#define JOURNALMODEL_H

#include <QSqlQueryModel>
#include <QItemDelegate>

class JournalModel : public QSqlQueryModel
{
    Q_OBJECT

public:
    JournalModel( QObject *parent = 0);

    enum Columns {
        clID = 0, clErrorDT, clSensorName, clSymptomText, clInterval, clSolveDT, clSolutions, clSpecFIO,
        clSpecID, clUnitID, clMachineID, clDepartID, clSymptomID,
        clColumnCount
    };

    virtual int	columnCount ( const QModelIndex & /*parent = QModelIndex() */) const { return clColumnCount; }
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    virtual bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
};


class JournalDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    JournalDelegate( const QString & color, QObject *parent = 0 );

protected:
    virtual void paint ( QPainter * painter, const QStyleOptionViewItem & option,
                         const QModelIndex & index ) const;

private:
    QString p_color;
};

#endif // JOURNALMODEL_H
