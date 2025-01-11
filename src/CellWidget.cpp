// -*- explicit-buffer-name: "CellWidget.cpp<M1-MOBJ/8-10>" -*-

#include <QResizeEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QApplication>
#include "CellWidget.h"
#include "Term.h"
#include "Instance.h"
#include "Symbol.h"
#include "Shape.h"
#include "Cell.h"
#include "Line.h"
#include "Node.h"
#include "Net.h"

//using namespace std;
namespace Netlist
{
    std::ostream &operator<<(std::ostream &o, const QRect &rect)
    {
        o << "<QRect x:" << rect.x()
          << " y:" << rect.y()
          << " w:" << rect.width()
          << " h:" << rect.height() << ">";
        return o;
    }

    std::ostream &operator<<(std::ostream &o, const QPoint &p)
    {
        o << "<QRect x:" << p.x() << " y:" << p.y() << ">";
        return o;
    }

    CellWidget::CellWidget(QWidget *parent)
        : QWidget(parent), cell_(NULL), viewport_(0, 0, 1280, 720), elementNames(std::vector<std::string>())
    {
        elementNames.push_back("Lines");
        elementNames.push_back("Terms");
        elementNames.push_back("Instances");
        elementNames.push_back("Background");
        setAttribute(Qt::WA_OpaquePaintEvent);
        setAttribute(Qt::WA_NoSystemBackground);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        
        setFocusPolicy(Qt::StrongFocus);
        setMouseTracking(true);
    }

    CellWidget::~CellWidget()
    {
    }

    void CellWidget::setCell(Cell *cell)
    {
        cell_ = cell;
        scaleFactor = 2;

        repaint();
    }

    QSize CellWidget::minimumSizeHint() const
    {
        return QSize(1280, 720);
    }

    void CellWidget::resizeEvent(QResizeEvent *event)
    {
        viewport_.setX2(viewport_.getX1() + size().width());
        viewport_.setY1(viewport_.getY2() - size().height());
        repaint();
    }

    void CellWidget ::keyPressEvent(QKeyEvent *event)
    {
        event->ignore();
        if (event->modifiers() & (Qt::AltModifier))
        {
            switch (event->key()) // Changement de la couleur choisie
            {
            case Qt::Key_Plus:
                colors[selectColor][selectElement] = (colors[selectColor][selectElement] + 1) % 256;
                repaint();
                break;
            case Qt::Key_Minus:
                colors[selectColor][selectElement] = colors[selectColor][selectElement] <= 0 ? 255 : colors[selectColor][selectElement] - 1;
                repaint();
                break;
            }
        }
        else if (event->modifiers() & Qt::ShiftModifier) // Choix de la couleur à changer
        {
            switch (event->key())
            {
            // Selection de l'élément à colorer
            case Qt::Key_L:
                selectElement = 0; // Lines
                repaint();
                break;
            case Qt::Key_T:
                selectElement = 1; // Terms
                repaint();
                break;
            case Qt::Key_I:
                selectElement = 2; // Instances
                repaint();
                break;
            case Qt::Key_B:
                selectElement = 3; // Background
                repaint();
                break;

            // Sélection de la couleur primaire à ajuster : 0 => Red; 1 => Green; 2 => Blue
            case Qt::Key_Up:
                selectColor = selectColor <= 0 ? 2 : selectColor -1;
                repaint();
                break;
            case Qt::Key_Down:
                selectColor = (selectColor + 1) % 3;
                repaint();
                break;

            default:
                return;
            }
        }
        else if (event->modifiers() & (Qt::ControlModifier)) // Contrôle du zomm
        {
            switch (event->key())
            {
            case Qt::Key_Plus:
                scaleFactor += 1;
                repaint();
                break;
            case Qt::Key_Minus:
                if (scaleFactor > 1)
                    scaleFactor -= 1;
                repaint();
                break;
            default:
                return;
            }
        }
        else
        {
            switch (event->key()) // Contrôle du déplacement
            {
            case Qt::Key_Up:
                goUp();
                break;
            case Qt::Key_Down:
                goDown();
                break;
            case Qt::Key_Left:
                goLeft();
                break;
            case Qt::Key_Right:
                goRight();
                break;

            default:
                return;
            }
        }
        event->accept();
    }

    void CellWidget::paintEvent(QPaintEvent *event)
    {
        //===================== Récupération des couleurs des éléments =====================
        QColor lineColor = QColor(colors[0][0], colors[1][0], colors[2][0]);
        QColor termColor = QColor(colors[0][1], colors[1][1], colors[2][1]);
        QColor instColor = QColor(colors[0][2], colors[1][2], colors[2][2]);
        QColor bkgdColor = QColor(colors[0][3], colors[1][3], colors[2][3]);

        std::vector<std::tuple<int, int, TermShape *>> instTerms; // Vecteur de tous les terms d'instance

        QString cellName = "NULL";
        if (cell_)
        {
            cellName = cell_->getName().c_str();
        }

        //===================== Création des stylos et brosses pour les différents éléments =====================
        QPen linePen;
        linePen.setColor(lineColor);
        linePen.setWidth(scaleFactor);
        linePen.setCapStyle(Qt::RoundCap);
        linePen.setJoinStyle(Qt::RoundJoin);
        QBrush lineBrush(lineColor);

        QPen TermPen;
        TermPen.setColor(termColor);
        TermPen.setWidth(scaleFactor);
        TermPen.setCapStyle(Qt::RoundCap);
        TermPen.setJoinStyle(Qt::RoundJoin);
        QBrush termBrush(termColor);

        QPen instPen;
        instPen.setColor(instColor);
        instPen.setWidth(3 * scaleFactor);
        instPen.setCapStyle(Qt::RoundCap);
        instPen.setJoinStyle(Qt::RoundJoin);

        QPen mainPen;
        mainPen.setColor(QColor(Qt::black));
        QBrush mainBrush((QColor(Qt::white)));
        QFont mainFont("URW Bookman L", 20);
        QFontMetrics mainFontMetric(mainFont); // Permet de créer des rectangles encadrant les (bonne taille, déplaçable, etc...)

        QRect cartoucheDroit = mainFontMetric.boundingRect("Background"); // Background est le plus grand mot parmi Line, Term, Instance, Backgroung
        cartoucheDroit.setWidth(cartoucheDroit.width() + 10);

        QRect cartoucheGauche = mainFontMetric.boundingRect(cellName); // Cartouche du nom, taille variable
        cartoucheGauche.setWidth(cartoucheGauche.width() + 20);
        cartoucheGauche.setHeight(cartoucheGauche.height() + 20);
        cartoucheGauche.translate(0, 20);

        QRect texteDroit = cartoucheDroit.adjusted(0, 0, 0, 10); // Cartouche des informations visuelles, taille fixe (largeur de "Background")
        cartoucheDroit.setHeight((cartoucheDroit.height() + 10) * 5);
        cartoucheDroit.translate(QPoint(size().width() - cartoucheDroit.width(), 20));
        texteDroit.translate(QPoint(size().width() - texteDroit.width(), 20));

        QPainter painter(this);

        painter.setBackground(QBrush(bkgdColor)); // Couleur du fond
        painter.eraseRect(QRect(QPoint(0, 0), size()));

        QFont font("URW Bookman L", 6 * scaleFactor); // taille de tous les textes liés directement à la cell (noms de terminaux, d'instances)
        QFontMetrics fm(font);
        painter.setFont(font);

        //=================================================== Instances ===================================================
        for (Instance *i : cell_->getInstances())
        {
            painter.setPen(instPen);
            painter.setBrush(Qt::NoBrush);
            int x0 = i->getPosition().getX();
            int y0 = i->getPosition().getY();
            QString inst_name = i->getName().c_str();
            QRect textBox = fm.boundingRect(inst_name);
            textBox.translate(QPoint(xToScreenX(x0 * scaleFactor), yToScreenY(y0 * scaleFactor) + textBox.height() + 5));
            painter.drawText(textBox, inst_name);

            for (Shape *s : i->getMasterCell()->getSymbol()->getShapes())
            {
                //=================================================== Récupération des terminaux d'instances ===================================================
                if (TermShape *t_shape = dynamic_cast<TermShape *>(s))
                {
                    i->getTerm(t_shape->getTerm()->getName())->setPosition(x0 + t_shape->getBox().getX1(), y0 + t_shape->getBox().getY1()); // mise à jour de la position du term d'instance dans le dessin global (absolue)
                    instTerms.push_back({x0, y0, t_shape});                                                                                 // stockage des coordonnées et du nom pour dessin ultérieur (par dessus les lignes de connexion)
                }
                //=================================================== Instances ===================================================
                else if (dynamic_cast<LineShape *>(s))
                {
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(instPen);
                    int trueX1 = xToScreenX((x0 + s->getBox().getX1()) * scaleFactor);
                    int trueY1 = yToScreenY((y0 + s->getBox().getY1()) * scaleFactor);
                    int trueX2 = xToScreenX((x0 + s->getBox().getX2()) * scaleFactor);
                    int trueY2 = yToScreenY((y0 + s->getBox().getY2()) * scaleFactor);
                    painter.drawLine(trueX1, trueY1, trueX2, trueY2);
                }
                else if (dynamic_cast<BoxShape *>(s))
                {
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(instPen);
                    int trueX = xToScreenX((x0 + s->getBox().getX1()) * scaleFactor);
                    int trueY = yToScreenY((y0 + s->getBox().getY2()) * scaleFactor);
                    int trueWidth = s->getBox().getWidth() * scaleFactor;
                    int trueHeight = s->getBox().getHeight() * scaleFactor;
                    painter.drawRect(trueX, trueY, trueWidth, trueHeight);
                }
                else if (dynamic_cast<EllipseShape *>(s))
                {
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(instPen);
                    int trueX = xToScreenX((x0 + s->getBox().getX1()) * scaleFactor);
                    int trueY = yToScreenY((y0 + s->getBox().getY2()) * scaleFactor);
                    int trueWidth = s->getBox().getWidth() * scaleFactor;
                    int trueHeight = s->getBox().getHeight() * scaleFactor;
                    painter.drawEllipse(trueX, trueY, trueWidth, trueHeight);
                }
                else if (ArcShape *as = dynamic_cast<ArcShape *>(s))
                {
                    painter.setBrush(Qt::NoBrush);
                    painter.setPen(instPen);
                    int trueX = xToScreenX((x0 + s->getBox().getX1()) * scaleFactor);
                    int trueY = yToScreenY((y0 + s->getBox().getY2()) * scaleFactor);
                    int trueWidth = s->getBox().getWidth() * scaleFactor;
                    int trueHeight = s->getBox().getHeight() * scaleFactor;
                    painter.drawArc(trueX, trueY, trueWidth, trueHeight, as->getStart() * 16, as->getSpan() * 16);
                }
            }
        }
        //=================================================== Lines (connexions) ===================================================

        for (Net *n : cell_->getNets())
        {
            n->getLines();
            painter.setBrush(Qt::NoBrush);
            painter.setPen(linePen);
            for (Line *l : n->getLines())
            {
                int trueX1 = xToScreenX(l->getSource()->getPosition().getX() * scaleFactor);
                int trueY1 = yToScreenY(l->getSource()->getPosition().getY() * scaleFactor);
                int trueX2 = xToScreenX(l->getTarget()->getPosition().getX() * scaleFactor);
                int trueY2 = yToScreenY(l->getTarget()->getPosition().getY() * scaleFactor);
                painter.drawLine(trueX1, trueY1, trueX2, trueY2);
            }

            linePen.setWidth(5 * scaleFactor);
            painter.setPen(linePen);
            for (Node *no : n->getNodes())
            {
                if (no == nullptr)
                    continue;
                if (no->getDegree() > 2)
                {
                    painter.drawPoint(xToScreenX(no->getPosition().getX() * scaleFactor), yToScreenY(no->getPosition().getY() * scaleFactor));
                }
            }
            linePen.setWidth(scaleFactor);
            painter.setPen(linePen);
        }

        //=================================================== Terminaux de cell ===================================================
        painter.setBrush(termBrush);
        painter.setPen(TermPen);

        for (Term *t : cell_->getTerms())
        {
            QPoint points[7];
            int x = t->getPosition().getX();
            int y = t->getPosition().getY();
            QString term_name = t->getName().c_str();
            QRect textBox = fm.boundingRect(term_name);

            switch (t->getDirection())
            {
            case Term::In: //"flèche" vers la droite
                points[0] = QPoint(xToScreenX(x * scaleFactor), yToScreenY(y * scaleFactor));
                points[5] = QPoint(xToScreenX(x * scaleFactor), yToScreenY(y * scaleFactor));
                points[1] = QPoint(xToScreenX((x - 5) * scaleFactor), yToScreenY((y + 5) * scaleFactor));
                points[2] = QPoint(xToScreenX((x - 15) * scaleFactor), yToScreenY((y + 5) * scaleFactor));
                points[3] = QPoint(xToScreenX((x - 15) * scaleFactor), yToScreenY((y - 5) * scaleFactor));
                points[4] = QPoint(xToScreenX((x - 5) * scaleFactor), yToScreenY((y - 5) * scaleFactor));
                painter.drawPolygon(points, 6);
                textBox.translate(points[3]);
                textBox.translate(-textBox.width(), textBox.height());
                painter.drawText(textBox, term_name);
                break;

            case Term::Out: //"flèche" vers la gauche
                points[0] = QPoint(xToScreenX(x * scaleFactor), yToScreenY(y * scaleFactor));
                points[5] = QPoint(xToScreenX(x * scaleFactor), yToScreenY(y * scaleFactor));
                points[1] = QPoint(xToScreenX((x + 5) * scaleFactor), yToScreenY((y + 5) * scaleFactor));
                points[2] = QPoint(xToScreenX((x + 15) * scaleFactor), yToScreenY((y + 5) * scaleFactor));
                points[3] = QPoint(xToScreenX((x + 15) * scaleFactor), yToScreenY((y - 5) * scaleFactor));
                points[4] = QPoint(xToScreenX((x + 5) * scaleFactor), yToScreenY((y - 5) * scaleFactor));
                painter.drawPolygon(points, 6);
                textBox.translate(points[3]);
                textBox.translate(0, textBox.height());
                painter.drawText(textBox, term_name);
                break;

            case Term::Inout: // double "flèche" (losange)
                points[0] = QPoint(xToScreenX((x - 3) * scaleFactor), yToScreenY((y - 6) * scaleFactor));
                points[6] = QPoint(xToScreenX((x - 3) * scaleFactor), yToScreenY((y - 6) * scaleFactor));
                points[1] = QPoint(xToScreenX((x - 9) * scaleFactor), yToScreenY(y * scaleFactor));
                points[2] = QPoint(xToScreenX((x - 3) * scaleFactor), yToScreenY((y + 6) * scaleFactor));
                points[3] = QPoint(xToScreenX((x + 3) * scaleFactor), yToScreenY((y + 6) * scaleFactor));
                points[4] = QPoint(xToScreenX((x + 9) * scaleFactor), yToScreenY((y)*scaleFactor));
                points[5] = QPoint(xToScreenX((x + 3) * scaleFactor), yToScreenY((y - 6) * scaleFactor));
                painter.drawPolygon(points, 7);
                textBox.translate(points[0]);
                textBox.translate(0, textBox.height());
                painter.drawText(textBox, term_name);
                break;

            default:
                textBox.translate(QPoint(xToScreenX(x * scaleFactor), yToScreenY(y * scaleFactor)));
                painter.drawText(textBox, term_name);
                break;
            }
        }

        //===================== Dessin de terminaux d'instance, par dessus les lignes de connexion =====================
        for (auto t : instTerms)
        {
            int x0 = std::get<0>(t);
            int y0 = std::get<1>(t);
            TermShape *t_shape = std::get<2>(t);
            painter.setPen(TermPen);
            QString term_name = t_shape->getTerm()->getName().c_str();
            QRect textBox = fm.boundingRect(term_name);
            int trueX = xToScreenX((x0 + t_shape->getBox().getX1()) * scaleFactor);
            int trueY = yToScreenY((y0 + t_shape->getBox().getY1()) * scaleFactor);
            painter.fillRect(QRectF(trueX - 2.5 * scaleFactor, trueY - 2.5 * scaleFactor, 5 * scaleFactor, 5 * scaleFactor), termBrush);
            textBox.translate(QPoint(trueX, trueY));

            
            switch(t_shape->getAlign())
            {
                case TermShape::TopLeft:
                    textBox.translate(QPoint(-3*scaleFactor-textBox.width(), -3*scaleFactor));
                    break;
                case TermShape::TopRight:
                    textBox.translate(QPoint(3*scaleFactor, -3*scaleFactor));
                    break;
                case TermShape::BottomLeft:
                    textBox.translate(QPoint(-3*scaleFactor-textBox.width(), 3*scaleFactor+textBox.height()));
                    break;
                case TermShape::BottomRight:
                    textBox.translate(QPoint(3*scaleFactor, 3*scaleFactor+textBox.height()));
                    break;
            }
            painter.drawText(textBox, term_name);
        }
        //===================== Zones de texte (rectangles blanc) =====================
        painter.setFont(mainFont);
        painter.setPen(mainPen);
        painter.fillRect(cartoucheGauche, mainBrush);
        painter.fillRect(cartoucheDroit, mainBrush);

        //===================== Nom de la cellule (noir) =====================
        painter.drawText(cartoucheGauche, cellName, QTextOption(Qt::AlignCenter));

        //===================== Niveau de zoom (noir) =====================
        std::string factor = "Zoom : x";
        factor.append(std::to_string(scaleFactor));
        painter.drawText(texteDroit, factor.c_str(), QTextOption(Qt::AlignCenter));

        //===================== Élément sélectionné (sa couleur, sur un rectangle de sa couleur complémentaire) =====================
        mainPen.setColor(QColor(colors[0][selectElement], colors[1][selectElement], colors[2][selectElement]));
        painter.setPen(mainPen);
        texteDroit.translate(0, texteDroit.height());
        painter.fillRect(texteDroit, QBrush(QColor(255 - colors[0][selectElement], 255 - colors[1][selectElement], 255 - colors[2][selectElement])));
        painter.drawText(texteDroit, QString(elementNames[selectElement].c_str()), QTextOption(Qt::AlignCenter));

        //===================== Composante rouge de l'élément sélectionné (gras si sélectionnée) =====================
        mainPen.setColor(Qt::red);
        if (selectColor == 0)
        {
            mainFont.setBold(true);
            painter.setFont(mainFont);
        }
        else
        {
            mainFont.setBold(false);
            painter.setFont(mainFont);
        }
        painter.setPen(mainPen);
        texteDroit.translate(0, texteDroit.height());
        painter.drawText(texteDroit, QString(std::to_string(colors[0][selectElement]).c_str()), QTextOption(Qt::AlignCenter));

        //===================== Composante verte de l'élément sélectionné (gras si sélectionnée) =====================
        mainPen.setColor(Qt::green);
        if (selectColor == 1)
        {
            mainFont.setBold(true);
            painter.setFont(mainFont);
        }
        else
        {
            mainFont.setBold(false);
            painter.setFont(mainFont);
        }
        painter.setPen(mainPen);
        texteDroit.translate(0, texteDroit.height());
        painter.drawText(texteDroit, QString(std::to_string(colors[1][selectElement]).c_str()), QTextOption(Qt::AlignCenter));

        //===================== Composante bleue de l'élément sélectionné (gras si sélectionnée) =====================
        mainPen.setColor(Qt::blue);
        if (selectColor == 2)
        {
            mainFont.setBold(true);
            painter.setFont(mainFont);
        }
        else
        {
            mainFont.setBold(false);
            painter.setFont(mainFont);
        }
        painter.setPen(mainPen);
        texteDroit.translate(0, texteDroit.height());
        painter.drawText(texteDroit, QString(std::to_string(colors[2][selectElement]).c_str()), QTextOption(Qt::AlignCenter));

        //===================== Indications des commandes (texte noir sur bandeau blanc) =====================
        mainFont.setBold(false);
        mainFont.setPixelSize(22);
        painter.setFont(mainFont);
        mainFontMetric = QFontMetrics(mainFont);

        mainPen.setColor(Qt::black);
        painter.setPen(mainPen);
        QString helpText("Alt+/- : Adjust primary color for selected element | Ctrl+/- : Adjust zoom | Shift L/I/T/B : Change selected element | Shift Up/Down : Change selected primary color");
        QRect helpBox = mainFontMetric.boundingRect(helpText);
        helpBox.setWidth(size().width());
        helpBox.translate(0, size().height() - 6);
        painter.fillRect(helpBox, mainBrush);
        painter.drawText(helpBox, helpText, QTextOption(Qt::AlignJustify));
    }
} // Netlist namespace.
