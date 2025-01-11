// -*- explicit-buffer-name: "Main.cpp<M1-MOBJ/7>" -*-

#include <string>
#include <sstream>
#include <iostream>
#include <memory>
using namespace std;

#include "Term.h"
#include "Net.h"
#include "Instance.h"
#include "Cell.h"
#include "CellViewer.h"
#include <QApplication>
#include <qboxlayout.h>
using namespace Netlist;

int main(int argc, char *argv[])
{
    Cell* defaultCell = new Cell("Default");    //Default Cell : utilisée pour les instances dont le modèle ne serait pas chargé, et pour lequel le xml serait illisible.
    
    /*Cell::load("vdd");
    Cell::load("gnd");
    Cell::load("TransistorN");
    Cell::load("TransistorP");
    Cell::load("and2");
    Cell *xor2 = Cell::load("xor2");
    Cell *halfadder = Cell::load("halfadder");*/
    //Cell *fulladder = Cell::load("fulladder");

    QApplication *qa = new QApplication(argc, argv);
    CellViewer *viewer = new CellViewer();
    viewer -> setCell(defaultCell); //openCelldialog
    viewer -> show();
    int rvalue = qa -> exec();
    delete qa;
    return rvalue;
    return 0;
   

}
