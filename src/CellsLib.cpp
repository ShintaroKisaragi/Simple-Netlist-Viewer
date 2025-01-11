#include <QMenuBar>
#include <QResizeEvent>
#include <QFont>
# include <QLabel>
# include <QPushButton>
# include <QHBoxLayout>
# include <QVBoxLayout>
#include <QWidget>
#include <QApplication>
#include <QHeaderView>
#include "CellsLib.h"
#include "CellViewer.h"
#include "Cell.h"


using namespace std;

namespace Netlist
{
    CellsLib::CellsLib(QWidget *parent)
    : QWidget(parent)
    , cellViewer_( NULL )
    , baseModel_(new CellsModel(this))
    , view_(new QTableView(this))
    , load_(new QPushButton(this))
    {
        setAttribute(Qt::WA_QuitOnClose , false );
        setAttribute(Qt::WA_DeleteOnClose , false );//L'objet ne sera pas supprimé lorsqu'il est masqué, il reste en mémoire.
        setContextMenuPolicy(Qt::ActionsContextMenu);

        view_->setShowGrid(false); // Désactive l'affichage de la grille du tableau.
        view_->setAlternatingRowColors(true); // Alterne les couleurs des lignes pour une meilleure lisibilité.
        view_->setSelectionBehavior(QAbstractItemView::SelectRows); // Permet de sélectionner uniquement des lignes entières.
        view_->setSelectionMode(QAbstractItemView::SingleSelection); // Permet de sélectionner une seule ligne à la fois.
        view_->setSortingEnabled(true); // Autorise le tri des colonnes du tableau.
        view_->setModel(baseModel_); // Associe le modèle `CellsModel` au tableau.
        //Personnalisation de l'en-tête du tableau :
        QHeaderView *horizontalHeader = view_->horizontalHeader();
        horizontalHeader->setDefaultAlignment(Qt::AlignHCenter);
        horizontalHeader->setMinimumSectionSize(300);
        horizontalHeader->setStretchLastSection(true);// La dernière colonne occupe tout l'espace restant.

        QHeaderView * verticalHeader = view_->verticalHeader();
        verticalHeader->setVisible( false );
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
        load_->setText("Load");
        connect (load_, SIGNAL(clicked()), this, SLOT(load()));

        QHBoxLayout * hLayout = new QHBoxLayout();
        hLayout->addStretch();
        hLayout->addWidget( load_ );
        QVBoxLayout* vLayout = new QVBoxLayout();
        vLayout->addWidget( view_ );
        vLayout->addLayout( hLayout );
        setLayout( vLayout );

    }

    void CellsLib::setCellViewer(CellViewer* c)
    {
        cellViewer_ = c;
    }

    int CellsLib::getSelectedRow() const
    {
        QModelIndexList selecteds = view_->selectionModel()->selection().indexes();
        if (selecteds.empty()) return -1;
        return selecteds.first().row();//Récupérer la ligne actuellement sélectionnée dans la table.

    }

    void CellsLib::load()
    //CellsLib utilise la méthode setCellViewer pour associer un CellViewer.
    {
        int selectedRow = getSelectedRow();
        if ( selectedRow < 0) return;
        cellViewer_->setCell(baseModel_->getModel(selectedRow));//Met à jour le CellViewer pour afficher cette cellule.

    }
}