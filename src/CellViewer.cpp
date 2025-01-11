#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QKeySequence>
#include "InstancesWidget.h"
#include "SaveCellDialog.h"
#include "CellWidget.h" // Classe définie par vous
#include "CellViewer.h"
#include "CellWidget.h"
#include "OpenCellDialog.h"
#include <filesystem>
namespace Netlist
{
    CellViewer::CellViewer(QWidget *parent)
        : QMainWindow(parent), cellWidget_(NULL), saveCellDialog_(NULL), instancesWidget_(NULL), cellsLib_(NULL)
    {
        cellWidget_ = new CellWidget();
        saveCellDialog_ = new SaveCellDialog(this);
        setCentralWidget(cellWidget_);
        QMenu *fileMenu = menuBar()->addMenu("&File");

        // save Cell
        QAction *action = new QAction("&Save As", this);
        action->setStatusTip("Save to disk (rename) the Cell");
        action->setShortcut(QKeySequence("CTRL+S"));
        action->setVisible(true);
        fileMenu->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(saveCell()));

        // Open Cell
        action = new QAction("&Open Cell", this);
        action->setStatusTip("Open a cell Netlist Viewer");
        action->setShortcut(QKeySequence("CTRL+O"));
        action->setVisible(true);
        fileMenu->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(openCell()));

        // Quit
        action = new QAction("&Quit", this);
        action->setStatusTip("Exit the Netlist Viewer");
        action->setShortcut(QKeySequence("CTRL+Q"));
        action->setVisible(true);
        fileMenu->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(close()));

        // Open instance list
        instancesWidget_ = new InstancesWidget();
        instancesWidget_->setCellViewer(this);
        action = new QAction("&Open Instance List", this);
        action->setStatusTip("Open a cell Netlist Viewer");
        action->setShortcut(QKeySequence("CTRL+I"));
        action->setVisible(true);
        fileMenu->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(showInstancesWidget()));

        // Open cell list
        cellsLib_ = new CellsLib();
        cellsLib_->setCellViewer(this); // CellsLib utilise la méthode setCellViewer pour associer un CellViewer.
        action = new QAction("&Open Cell List", this);
        action->setStatusTip("Open a cell Netlist Viewer");
        action->setShortcut(QKeySequence("CTRL+C"));
        action->setVisible(true);
        fileMenu->addAction(action);
        connect(this, SIGNAL(cellLoaded()), cellsLib_->getBaseModel(), SLOT(updateDatas()));
        connect(action, SIGNAL(triggered()), this, SLOT(showCellsLib()));
        emit cellLoaded();
    }
    void CellViewer::saveCell()
    {
        Cell *cell = getCell();
        if (cell == NULL)
            return;
        QString cellName = cell->getName().c_str();
        
        QString CellPath = std::filesystem::current_path().string().c_str();
        if (saveCellDialog_->run(cellName, CellPath))
        {
            cell->setName(cellName.toStdString());
            cell->save(cellName.toStdString(), CellPath.toStdString());
        }
    }
    void CellViewer::openCell()
    {
        QString cellName;

        // Ouvrir la boîte de dialogue pour demander le nom de la cellule
        if (OpenCellDialog::run(cellName))
        {
            // Rechercher la cellule par le nom entré
            Cell *cell = Cell::find(cellName.toStdString());

            if (cell) // Vérifier si la cellule a été trouvée
            {
                setCell(cell); // Mettre à jour le viewer avec la cellule trouvée
            }
            else if ((cell = Cell::load(cellName.toStdString()))) // Si la cellule n'était pas déjà chargée, on essaye de la lire depuis le xml
            {
                setCell(cell);
                emit cellLoaded();
            }
            else
            {
                std::cout << "Cellule \"" << cellName.toStdString() << "\" introuvable. Annulation du chargement." << std::endl;
            }
        }
    }
    CellViewer::~CellViewer()
    {
        // Nettoyer les objets créés dynamiquement
        delete saveCellDialog_; // Détruit la boîte de dialogue de sauvegarde
        delete cellWidget_;     // Détruit le widget central

        // Les autres objets (comme les actions ou les menus) sont détruits automatiquement par Qt grâce à la gestion de la hiérarchie des widgets.
    }

    void CellViewer::setCell(Cell *p)
    {
        if (!p)
        {
            qDebug() << "Attempted to set a null Cell.";
            return;
        }

        // Mettre à jour le CellWidget avec la nouvelle cellule

        cellWidget_->setCell(p);
        if(instancesWidget_)
        {
            instancesWidget_->setCell(p);
        }

        // Mettre à jour le titre de la fenêtre principale avec le nom de la cellule
        setWindowTitle(QString("Cell Viewer - %1").arg(p->getName().c_str()));

        qDebug() << "Cell set to:" << QString::fromStdString(p->getName());
    }

    Cell *CellViewer::getCell() const
    {
        return cellWidget_->getCell();
    }

    void CellViewer::showInstancesWidget()
    {
        if (!instancesWidget_)
            return;               // Vérifier que InstancesWidget est valide
        instancesWidget_->show(); // Afficher la fenêtre
    }

    void CellViewer::showCellsLib()
    {
        cellsLib_ -> show();
    }
}