#ifndef INSTANCES_WIDGET_H
#define INSTANCES_WIDGET_H

#include <QMainWindow>
#include "InstancesModel.h"
#include "CellViewer.h"
#include <QTableView>
#include <QPushButton>
namespace Netlist
{
    class CellViewer;
class InstancesWidget : public QWidget {

  //Affiche une liste des instances (via QTableView et InstancesModel).
  //Permet à l'utilisateur de charger la master cell d'une instance sélectionnée dans le CellViewer en cliquant sur le bouton Load.
    Q_OBJECT;
  public:
                  InstancesWidget ( QWidget* parent=NULL );
            void  setCellViewer   ( CellViewer* cell);
            int   getSelectedRow  () const;
    inline  void  setCell ( Cell* c){baseModel_->setCell(c);};
  public slots:
            void  load            ();
  private:
            CellViewer*     cellViewer_;
            InstancesModel* baseModel_;
            QTableView*     view_;
            QPushButton*    load_;
};
}
#endif