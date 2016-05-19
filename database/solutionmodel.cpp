/*
 * solutionmodel.cpp
 *
 *  Created on: 03.11.2011
 *      Author: Aim
 */

#include "stdafx.h"
#include "solutionmodel.h"

#include "relationalmodel.h"
#include "database.h"
#include "dlgstructure.h"

const QStringList SolutionModel::m_lstTypes( QStringList()
                                             << QString::fromLocal8Bit( "Осмотр без действий" )
                                             << QString::fromLocal8Bit( "Поломка и ремонт" )
                                             << QString::fromLocal8Bit( "Поломка и замена" )
                                             << QString::fromLocal8Bit( "Поломка и утилизация" ) );

SolutionModel::SolutionModel( QObject * parent )
    : QSqlQueryModel( parent )
{
    setHeaderData( clID, Qt::Horizontal, QString::fromLocal8Bit( "ID" ) );
    setHeaderData( clType, Qt::Horizontal, QString::fromLocal8Bit( "Тип" ) );
    setHeaderData( clText, Qt::Horizontal, QString::fromLocal8Bit( "Выполненная работа" ) );
    setHeaderData( clSensorName, Qt::Horizontal, QString::fromLocal8Bit( "Деталь" ) );
    setHeaderData( clDModelName, Qt::Horizontal, QString::fromLocal8Bit( "Марка детали" ) );
}

Qt::ItemFlags SolutionModel::flags( const QModelIndex &index ) const
{
    const QString strType = this->index( index.row(), clType ).data().toString();
    switch ( index.column() )
    {
    case clText:
    {
        if ( strType == m_lstTypes.at( 0 ) || strType == m_lstTypes.at( 2 ) || strType == m_lstTypes.at( 3 ) )
            return Qt::ItemIsSelectable;
        else return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
    case clDModelName:
    {
        if ( strType == m_lstTypes.at( 0 ) || strType == m_lstTypes.at( 1 ) || strType == m_lstTypes.at( 2 ) )
            return Qt::ItemIsSelectable;
        else return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
    default:
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }
}

QVariant SolutionModel::data( const QModelIndex & index, int role ) const
{
    QVariant var = QSqlQueryModel::data( index, role );
    if ( role == Qt::EditRole )
    {
        switch ( index.column() )
        {
        case clType:
        {
            TSolutionType st = { m_lstTypes[ var.toInt() - 1 ], clType };
            return QVariant::fromValue< TSolutionType >( st );
        }
        case clText:
        {
            TSolutionText st = { var.toString(), clText };
            return QVariant::fromValue< TSolutionText >( st );
        }
        case clSensorName:
        {
            TSensor se = { var.toString(), clSensorName };
            return QVariant::fromValue< TSensor >( se );
        }
        case clDModelName:
        {
            TDetailModel dm = { var.toString(), clDModelName };
            return QVariant::fromValue< TDetailModel >( dm );
        }
        default:
            return var.toString();
        }
    }
    else if ( role == Qt::DisplayRole )
    {
        switch ( index.column() )
        {
        case clType:
            return m_lstTypes.at( var.toInt() - 1 );
        case clText:
        {
            const QString strType = this->index( index.row(), clType ).data().toString();
            if ( strType == m_lstTypes.at( 0 ) || strType == m_lstTypes.at( 2 ) || strType == m_lstTypes.at( 3 ) )
                return QString();
            else return var.toString();
        }
        case clDModelName:
        {
            const QString strType = this->index( index.row(), clType ).data().toString();
            if ( strType == m_lstTypes.at( 0 ) || strType == m_lstTypes.at( 1 ) || strType == m_lstTypes.at( 2 ) )
                return QString();
            else return var.toString();
        }
        default:
            return var.toString();
        }
    }

    return var.toString();
}

bool SolutionModel::setData( const QModelIndex & index, const QVariant & value, int role )
{
    const int iSGID = this->data( this->index( index.row(), clID ) ).toInt();
    const int iSolID = this->data( this->index( index.row(), clSolutionID ) ).toInt();
    const int iSymID = this->data( this->index( index.row(), clSymptomID ) ).toInt();

    if ( role == Qt::EditRole )
    {
        bool bResult;
        switch ( index.column() )
        {
        case clType:
        {
            const int iType = value.value< TSolutionType >().index + 1;
            bResult = RADB::SolutionGroup::updateType( iSGID, iType );
            if ( iType != 2 )
            {
                QString strSolText;
                if ( iType == 1 ) strSolText = QString::fromLocal8Bit( "Осмотр" );
                else if ( iType == 3 ) strSolText = QString::fromLocal8Bit( "Замена" );
                else if ( iType == 4 ) strSolText = QString::fromLocal8Bit( "Утилизация" );
                const int iSenID = this->data( this->index( index.row(), clSensorID ) ).toInt();
                bResult = RADB::SolutionGroup::updateSolution( iSGID, iSolID, iSenID, iSymID, strSolText );
            }
            break;
        }
        case clSensorName:
        {
            if ( value.value< TSensor >().index < 0 )
            {
                DlgStructure dlg( 0, false );
                dlg.openAndAddSensor();
                dlg.exec();

                return false;
            }
            const QString strSolText = this->data( this->index( index.row(), clText ) ).toString();
            bResult = RADB::SolutionGroup::updateSolution( iSGID, iSolID, value.value< TSensor >().index, iSymID, strSolText );
            break;
        }
        case clText:
        {
            const int iSenID = this->data( this->index( index.row(), clSensorID ) ).toInt();
            bResult = RADB::SolutionGroup::updateSolution( iSGID, iSolID, iSenID, iSymID, value.value< TSolutionText >().value );
            break;
        }
        case clDModelName:
        {
            if ( value.value< TDetailModel >().index < 0 )
            {
                DlgStructure dlg( 0, false );
                dlg.openAndAddDetailModel();
                dlg.exec();
                return false;
            }
            bResult = RADB::SolutionGroup::updateDetailModel( iSGID, value.value< TDetailModel >().index );
            break;
        }
        default:
            return false;
        }
        emit dataChanged( index, index );
        return bResult;
    }

    return true;
}


SolutionEditorFactory::SolutionEditorFactory( const QItemEditorFactory * standardFactory )
    : QItemEditorFactory()
    , p_standardFactory( standardFactory )
{
}

QWidget * SolutionEditorFactory::createEditor( QVariant::Type type, QWidget * parent ) const
{
    if ( type == QVariant::Type( QVariant::fromValue< TSolutionType >( TSolutionType() ).userType() ) )
    {
        SQLSolutionTypeEditor * cb = new SQLSolutionTypeEditor( parent );
        for ( int  i = 0; i < SolutionModel::m_lstTypes.count(); i++ )
            cb->addItem( SolutionModel::m_lstTypes.at( i ), i );
        cb->setFrame( false );

        return cb;
    }
    else if ( type == QVariant::Type( QVariant::fromValue< TSolutionText >( TSolutionText() ).userType() ) )
    {
        SQLSolutionTextEditor * cb = new SQLSolutionTextEditor( parent );
        QSqlQuery query( RADB::Solution::sqlSelectAllBySymptom.arg( m_symptomID ) );
        while( query.next() )
            cb->addItem( query.value( 1 ).toString(), query.value( 0 ).toInt() );
        cb->setFrame( false );

        return cb;
    }
    else if ( type == QVariant::Type( QVariant::fromValue< TSensor >( TSensor() ).userType() ) )
    {
        SQLSensorEditor * cb = new SQLSensorEditor( parent );

        cb->addItem( QIcon( ":/add.png" ), "Новая деталь", -1 );

        RelationalModel< RASensor > modelSensor;
        modelSensor.setFilter( QString( "seUnitID = %1" ).arg( m_unitID ) );
        modelSensor.select();

        for ( int i = 0; i < modelSensor.rowCount(); i++ )
            cb->addItem( modelSensor.data( modelSensor.index( i, unShortName ) ).toString(),
                         modelSensor.data( modelSensor.index( i, unID ) ).toInt() );
        cb->setFrame( false );

        return cb;
    }
    else if ( type == QVariant::Type( QVariant::fromValue< TDetailModel >( TDetailModel() ).userType() ) )
    {
        SQLDetailModelEditor * cb = new SQLDetailModelEditor( parent );
        cb->addItem( QIcon( ":/add.png" ), "Новая марка", -1 );
        RelationalModel< RADetailModel > modelDetailModel;
        modelDetailModel.select();

        for ( int i = 0; i < modelDetailModel.rowCount(); i++ )
        {
            const QString strName = modelDetailModel.data( modelDetailModel.index( i, dmName ) ).toString();
            const int iId = modelDetailModel.data( modelDetailModel.index( i, dmID ) ).toInt();
            cb->addItem( strName, iId );
        }
        cb->setFrame( false );

        return cb;
    }

    else return p_standardFactory->createEditor(type, parent);
}

