#ifndef INSTANCES_MODEL_H
#define INSTANCES_MODEL_H

#include <QAbstractTableModel>
#include <QObject>

namespace Netlist
{
    class Cell;

    class InstancesModel : public QAbstractTableModel
    {
        //Stocke les données
        //Gère la structure des données
        Q_OBJECT ;
        public :
            InstancesModel(QObject *parent=NULL);
            ~InstancesModel();

            void setCell(Cell*);
            Cell *getModel(int row)const;
            int rowCount(const QModelIndex & parent)const;
            int columnCount(const QModelIndex & parent)const;

            QVariant data(const QModelIndex& index, int role=Qt::DisplayRole)const;
            QVariant headerData( int section, Qt::Orientation orientation, int role = Qt :: DisplayRole)const;
        private :
            Cell * cell_;
    };
}
#endif //INSTANCES_MODEL_H