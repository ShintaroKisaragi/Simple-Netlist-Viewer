#pragma once
#include <QMainWindow>
#include "InstancesModel.h"

#include <QTableView>
#include <QPushButton>
#include "CellsModel.h"
namespace Netlist
{
    class CellViewer;
    class CellsLib : public QWidget
    {

        Q_OBJECT;

    public:
        CellsLib(QWidget *parent = NULL);
        void setCellViewer(CellViewer *);
        int getSelectedRow() const;
        inline CellsModel *getBaseModel() { return baseModel_; };
    public slots:
        void load();

    private:
        CellViewer *cellViewer_;
        CellsModel *baseModel_;
        QTableView *view_;
        QPushButton *load_;
    };
}