#pragma once
#include "Cell.h"
#include <QMainWindow>
#include "CellWidget.h"
#include "SaveCellDialog.h"
#include "CellsLib.h"

namespace Netlist
{
    class InstancesWidget;

    class CellViewer : public QMainWindow
    {
        Q_OBJECT;

    public:
        CellViewer(QWidget *parent = NULL);
        virtual ~CellViewer();
        Cell *getCell() const;
        inline CellsLib *getCellsLib() { return cellsLib_; }; // TME9+.
    public slots:
        void setCell(Cell *);
        void saveCell();
        void openCell();
        void showCellsLib();        // TME9+.
        void showInstancesWidget(); // TME9+.

    signals:
        void cellLoaded(); // Signal émis lorsque la cellule est chargée pour la premiere fois.(Si la cellule n'était pas déjà chargée, on essaye de la lire depuis le xml)
    private:
        CellWidget *cellWidget_;
        SaveCellDialog *saveCellDialog_;
        InstancesWidget *instancesWidget_; // TME9+.
        CellsLib *cellsLib_; // TME9+.
    };
}
