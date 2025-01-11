#include "InstancesWidget.h"
#include <QMenuBar>
#include <QResizeEvent>
#include <QFont>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QApplication>
#include <QHeaderView>
#include "CellWidget.h"
#include "CellViewer.h"
#include "InstancesModel.h"

namespace Netlist
{
    InstancesWidget::InstancesWidget(QWidget *parent)
    :QWidget(parent), cellViewer_( NULL ), baseModel_(new InstancesModel(this)), view_(new QTableView(this)), load_(new QPushButton(this))
    {
        setAttribute(Qt::WA_QuitOnClose , false );
        setAttribute(Qt::WA_DeleteOnClose , false );
        setContextMenuPolicy(Qt::ActionsContextMenu);
        view_->setShowGrid(false);
        view_->setAlternatingRowColors(true);
        view_->setSelectionBehavior(QAbstractItemView::SelectRows);
        view_->setSelectionMode(QAbstractItemView::SingleSelection);
        view_->setSortingEnabled(true);
        view_->setModel(baseModel_);
        QHeaderView *horizontalHeader = view_->horizontalHeader();
        horizontalHeader->setDefaultAlignment(Qt::AlignHCenter);
        horizontalHeader->setMinimumSectionSize(300);
        horizontalHeader->setStretchLastSection(true);
        QHeaderView * verticalHeader = view_->verticalHeader();
                load_->setStyleSheet(
            "QPushButton {"
            "  background-color:rgb(76, 132, 175);"
            "  color: white;"
            "  border: 2px solidrgb(104, 76, 175);"
            "  border-radius: 8px;"
            "  padding: 8px;"
            "}"
            "QPushButton:hover {"
            "  background-color: white;"
            "  color:rgb(111, 76, 175);"
            "}");
        verticalHeader->setVisible( false );
        load_->setText("Load");
        connect (load_, SIGNAL(clicked()), this, SLOT(load()));

        QHBoxLayout * hLayout = new QHBoxLayout();
        hLayout->addStretch();
        hLayout->addWidget( load_ );
        hLayout->addStretch();
        QVBoxLayout* vLayout = new QVBoxLayout();
        vLayout->addWidget( view_ );
        vLayout->addLayout( hLayout );
        setLayout( vLayout );
    }

    void InstancesWidget::setCellViewer(CellViewer* c)//Associer un CellViewer au widget pour permettre de charger une master cell dans la fenêtre principale.
    {
        cellViewer_ = c;
    }

    int InstancesWidget::getSelectedRow() const
    {
        QModelIndexList selecteds = view_->selectionModel()->selection().indexes();
        if (selecteds.empty())return -1;
        return selecteds.first().row(); //Récupérer la ligne actuellement sélectionnée dans la table.
    }

    void InstancesWidget::load()
    {
        int selectedRow = getSelectedRow();
        if ( selectedRow < 0) return;
        cellViewer_->setCell(baseModel_->getModel(selectedRow));//pour récupérer la master cell de l'instance sélectionnée.Utilise cellViewer_->setCell() pour mettre à jour le CellViewer avec cette cellule.
    }
}