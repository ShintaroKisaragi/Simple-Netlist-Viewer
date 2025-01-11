#pragma once

#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QRect>
#include <QPoint>
class QPainter;
#include "Box.h"
#include <string>

namespace Netlist
{

    class Cell;
    class NodeTerm;

    class CellWidget : public QWidget
    {
        Q_OBJECT;

    public:
        CellWidget(QWidget *parent = NULL);
        virtual ~CellWidget();
        void setCell(Cell *);
        inline Cell *getCell() const;
        inline QRect boxToScreenRect(const Box &) const;
        inline QPoint pointToScreenPoint(const Point &) const;
        inline Box screenRectToBox(const QRect &) const;
        inline Point screenPointToPoint(const QPoint &) const;
        virtual QSize minimumSizeHint() const;
        virtual void resizeEvent(QResizeEvent *);

    protected:
        virtual void paintEvent(QPaintEvent *);
        virtual void keyPressEvent(QKeyEvent *);

    private:
        void goUp()
        {
            viewport_.translate(Point(0, 20));
            repaint ();
        }
        void goDown()
        {
            viewport_.translate(Point(0, -20));
            repaint ();
        }
        void goLeft()
        {
            viewport_.translate(Point(-20, 0));
            repaint ();
        }
        void goRight()
        {
            viewport_.translate(Point(20, 0));
            repaint ();
        }
        int xToScreenX(int x) {return x - viewport_.getX1();}
        int yToScreenY(int y) {return viewport_.getY2() - y;}
        int screenXToX(int x) {return x + viewport_.getX1();}
        int ScreenYToY(int y) {return viewport_.getY2() + y;}
        Cell *cell_;
        Box viewport_;
        std::vector<std::string> elementNames;
        int scaleFactor;
                                    //Line,Term,Inst,Bkgd
        unsigned int colors[3][4] = {{0xFF,0x84,0x14,0x00}, //Red
                                     {0xFF,0x00,0x84,0x14}, //Green
                                     {0xFF,0x14,0x00,0x84}};//Blue
                                     
        int selectColor = 0; //0 => Red; 1 => Green; 2 => Blue
        int selectElement = 0; //0 => Lines; 1 => Terms; 2 => Instances; 3 => Background

    };

    inline Cell *CellWidget::getCell() const { return cell_; }

} // Netlist namespace.
