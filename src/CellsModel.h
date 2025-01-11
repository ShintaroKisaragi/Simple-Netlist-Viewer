#ifndef CELLS_MODEL_H
#define CELLS_MODEL_H

#include <QAbstractTableModel>
#include <QObject>

namespace Netlist
{
    class Cell;
    class CellsModel : public QAbstractTableModel
    {
        Q_OBJECT ;
        public :
                                CellsModel          (QObject *parent=NULL);
                                ~CellsModel         ();
            void                setCell             (Cell*);
                    Cell*       getModel            (int row);
                    int         rowCount            (const QModelIndex& parent=QModelIndex())const;
                    int         columnCount         (const QModelIndex& parent=QModelIndex())const;
                    QVariant    data                (const QModelIndex& index, int role=Qt::DisplayRole)const;
                    QVariant    headerData          ( int section, Qt::Orientation orientation, int role = Qt :: DisplayRole)const;

        public slots :
            void updateDatas();// Slot pour mettre à jour la liste des cellules
        private :
            Cell* cell_;
    };
}
#endif //CELLS_MODEL_H