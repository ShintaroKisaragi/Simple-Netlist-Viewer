#include <QMenuBar>
#include <QWidget>
#include <QApplication>
#include "CellWidget.h"
#include "CellViewer.h"
#include "InstancesWidget.h"
#include "CellsModel.h"

#include "Cell.h"



using namespace std;

namespace Netlist
{
    CellsModel::CellsModel(QObject *parent)
    :QAbstractTableModel(parent), cell_( NULL )
    { }

    CellsModel::~CellsModel()
    { }

    void CellsModel::setCell(Cell* c) 
    {
        emit layoutAboutToBeChanged();
        cell_ = c;
        emit layoutChanged();
    }

    Cell * CellsModel::getModel( int row )
    {
        if ( not cell_ ) return NULL ;
        if ( row >= ( int )cell_->getAllCells().size())return NULL ;
        return cell_->getAllCells()[row];
    }

    int CellsModel::rowCount( const QModelIndex & parent ) const
    { return ( cell_ ) ? cell_->getAllCells().size():0;} //le nombre de toutes les cellules chargées en mémoiore

    int CellsModel::columnCount(const QModelIndex &parent ) const
    { return 1; } //une seule colonne 

    QVariant CellsModel::data(const QModelIndex &index, int role ) const
    {
        if ( not cell_ or not index.isValid ()) return QVariant ();
        if ( role == Qt::DisplayRole ) 
        {
            int row = index.row();// Ligne du tableau demandée
            switch(index.column())// Colonne du tableau demandée
            {
                //Lorsque la vue a besoin d'afficher toutes les cellules dans le tableau :Elle itère sur toutes les lignes, de 0 à rowCount() - 1 (automatiquement).
                case 0:return cell_->getAllCells()[row]->getName().c_str();
            }
        }
        return QVariant();
    }

    QVariant CellsModel::headerData ( int section, Qt::Orientation orientation, int role ) const
    {
        if ( orientation == Qt::Vertical ) return QVariant ();
        if ( role != Qt::DisplayRole ) return QVariant ();
        switch ( section ) 
        {
            case 0: return "Cell";
        }
        return QVariant();
    }

    void CellsModel::updateDatas()
    {
        emit layoutAboutToBeChanged();
        cell_ = Cell::getAllCells().size() > 0 ? Cell::getAllCells()[0] : nullptr;
        emit layoutChanged();
    }

}//End Netlist