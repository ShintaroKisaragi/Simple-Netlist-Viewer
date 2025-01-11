#include "Shape.h"
#include "Cell.h"
#define BOLDRED "\033[1m\033[31m"
#define RESET "\033[0m"

namespace Netlist
{
    //==================== Classe Shape ====================
    Shape::Shape(Symbol *symbol) : symbol_(symbol)
    {
    }

    Shape::~Shape()
    {
    }

    Shape *Shape::fromXml(Symbol *owner, xmlTextReaderPtr reader)
    {
        // Factory-like method.
        const xmlChar *boxTag = xmlTextReaderConstString(reader, (const xmlChar *)"box");
        const xmlChar *ellipseTag = xmlTextReaderConstString(reader, (const xmlChar *)"ellipse");
        const xmlChar *arcTag = xmlTextReaderConstString(reader, (const xmlChar *)"arc");
        const xmlChar *lineTag = xmlTextReaderConstString(reader, (const xmlChar *)"line");
        const xmlChar *termTag = xmlTextReaderConstString(reader, (const xmlChar *)"term");
        const xmlChar *nodeName = xmlTextReaderConstLocalName(reader);

        Shape *shape = NULL;
        if (boxTag == nodeName)
            shape = BoxShape::fromXml(owner, reader);
        if (ellipseTag == nodeName)
            shape = EllipseShape::fromXml(owner, reader);
        if (arcTag == nodeName)
            shape = ArcShape::fromXml(owner, reader);
        if (lineTag == nodeName)
            shape = LineShape::fromXml(owner, reader);
        if (termTag == nodeName)
            shape = TermShape::fromXml(owner, reader);

        if (shape == NULL)
            std::cerr << BOLDRED << "[ERROR] Unknown or misplaced tag <" << nodeName << "> (line:"
                      << xmlTextReaderGetParserLineNumber(reader) << ")." << RESET << std::endl;

        return shape;
    }

    Symbol *Shape::getSymbol() const
    {
        return symbol_;
    }

    //==================== Classe LineShape ====================

    LineShape::LineShape(Symbol *symbol, int x1, int y1, int x2, int y2) : Shape(symbol), x1_(x1), y1_(y1), x2_(x2), y2_(y2)
    {
    }

    LineShape::~LineShape()
    {
    }

    Shape *LineShape::fromXml(Symbol *symbol, xmlTextReaderPtr p)
    {
        const xmlChar *x1t = xmlTextReaderConstString(p, (const xmlChar *)"x1");
        const xmlChar *y1t = xmlTextReaderConstString(p, (const xmlChar *)"y1");
        const xmlChar *x2t = xmlTextReaderConstString(p, (const xmlChar *)"x2");
        const xmlChar *y2t = xmlTextReaderConstString(p, (const xmlChar *)"y2");

        std::string x1 = xmlCharToString(xmlTextReaderGetAttribute(p, x1t));
        std::string x2 = xmlCharToString(xmlTextReaderGetAttribute(p, x2t));
        std::string y1 = xmlCharToString(xmlTextReaderGetAttribute(p, y1t));
        std::string y2 = xmlCharToString(xmlTextReaderGetAttribute(p, y2t));

        if (x1 == "" || x2 == "" || y1 == "" || y2 == "")
            return nullptr;

        return new LineShape(symbol, stoi(x1), stoi(y1), stoi(x2), stoi(y2));
    }

    void LineShape::toXml(std::ostream &o) const
    {
        o << "<line x1=\"" << x1_ << "\" y1=\"" << y1_ << "\" x2=\"" << x2_ << "\" y2=\"" << y2_ << "\"/>\n";
    }

    

    //==================== Classe BoxShape ====================

    BoxShape::BoxShape(Symbol *symbol, int x1, int y1, int x2, int y2) : Shape(symbol)
    {
        box_ = new Box(x1, y1, x2, y2);
    }

    BoxShape::~BoxShape()
    {
        delete box_;
    }

    Shape *BoxShape::fromXml(Symbol *symbol, xmlTextReaderPtr p)
    {
        const xmlChar *x1t = xmlTextReaderConstString(p, (const xmlChar *)"x1");
        const xmlChar *y1t = xmlTextReaderConstString(p, (const xmlChar *)"y1");
        const xmlChar *x2t = xmlTextReaderConstString(p, (const xmlChar *)"x2");
        const xmlChar *y2t = xmlTextReaderConstString(p, (const xmlChar *)"y2");

        std::string x1 = xmlCharToString(xmlTextReaderGetAttribute(p, x1t));
        std::string x2 = xmlCharToString(xmlTextReaderGetAttribute(p, x2t));
        std::string y1 = xmlCharToString(xmlTextReaderGetAttribute(p, y1t));
        std::string y2 = xmlCharToString(xmlTextReaderGetAttribute(p, y2t));

        if (x1 == "" || x2 == "" || y1 == "" || y2 == "")
            return nullptr;

        return new BoxShape(symbol, stoi(x1), stoi(y1), stoi(x2), stoi(y2));
    }

    void BoxShape::toXml(std::ostream &o) const
    {
        o << "<box x1=\"" << box_->getX1() << "\" y1=\"" << box_->getY1() << "\" x2=\"" << box_->getX2() << "\" y2=\"" << box_->getY2() << "\"/>\n";
    }

    //==================== Classe EllipseShape ====================

    EllipseShape::EllipseShape(Symbol *symbol, int x1, int y1, int x2, int y2) : Shape(symbol)
    {
        box_ = new Box(x1, y1, x2, y2);
    }

    EllipseShape::~EllipseShape()
    {
        delete box_;
    }

    Shape *EllipseShape::fromXml(Symbol *symbol, xmlTextReaderPtr p)
    {
        const xmlChar *x1t = xmlTextReaderConstString(p, (const xmlChar *)"x1");
        const xmlChar *y1t = xmlTextReaderConstString(p, (const xmlChar *)"y1");
        const xmlChar *x2t = xmlTextReaderConstString(p, (const xmlChar *)"x2");
        const xmlChar *y2t = xmlTextReaderConstString(p, (const xmlChar *)"y2");

        std::string x1 = xmlCharToString(xmlTextReaderGetAttribute(p, x1t));
        std::string x2 = xmlCharToString(xmlTextReaderGetAttribute(p, x2t));
        std::string y1 = xmlCharToString(xmlTextReaderGetAttribute(p, y1t));
        std::string y2 = xmlCharToString(xmlTextReaderGetAttribute(p, y2t));

        if (x1 == "" || x2 == "" || y1 == "" || y2 == "")
            return nullptr;

        return new EllipseShape(symbol, stoi(x1), stoi(y1), stoi(x2), stoi(y2));
    }

    void EllipseShape::toXml(std::ostream &o) const
    {
        o << "<ellipse x1=\"" << box_->getX1() << "\" y1=\"" << box_->getY1() << "\" x2=\"" << box_->getX2() << "\" y2=\"" << box_->getY2() << "\"/>\n";
    }

    //==================== Classe ArcShape ====================

    ArcShape::ArcShape(Symbol *symbol, int x1, int y1, int x2, int y2, int start, int span) : Shape(symbol), start_(start), span_(span)
    {
        box_ = new Box(x1, y1, x2, y2);
    }

    ArcShape::~ArcShape()
    {
        delete box_;
    }

    Shape *ArcShape::fromXml(Symbol *symbol, xmlTextReaderPtr p)
    {
        const xmlChar *x1t = xmlTextReaderConstString(p, (const xmlChar *)"x1");
        const xmlChar *y1t = xmlTextReaderConstString(p, (const xmlChar *)"y1");
        const xmlChar *x2t = xmlTextReaderConstString(p, (const xmlChar *)"x2");
        const xmlChar *y2t = xmlTextReaderConstString(p, (const xmlChar *)"y2");
        const xmlChar *startt = xmlTextReaderConstString(p, (const xmlChar *)"start");
        const xmlChar *spant = xmlTextReaderConstString(p, (const xmlChar *)"span");

        std::string x1 = xmlCharToString(xmlTextReaderGetAttribute(p, x1t));
        std::string x2 = xmlCharToString(xmlTextReaderGetAttribute(p, x2t));
        std::string y1 = xmlCharToString(xmlTextReaderGetAttribute(p, y1t));
        std::string y2 = xmlCharToString(xmlTextReaderGetAttribute(p, y2t));
        std::string start = xmlCharToString(xmlTextReaderGetAttribute(p, startt));
        std::string span = xmlCharToString(xmlTextReaderGetAttribute(p, spant));

        if (x1 == "" || x2 == "" || y1 == "" || y2 == "" || start == "" || span == "")
            return nullptr;

        return new ArcShape(symbol, stoi(x1), stoi(y1), stoi(x2), stoi(y2), stoi(start), stoi(span));
    }

    void ArcShape::toXml(std::ostream &o) const
    {
        o << "<arc x1=\"" << box_->getX1() << "\" y1=\"" << box_->getY1() << "\" x2=\"" << box_->getX2()
          << "\" y2=\"" << box_->getY2() << "\" start=\"" << start_ << "\" span=\"" << span_ << "\"/>\n";
    }

    //==================== Classe TermShape ====================

    TermShape::TermShape(Symbol *symbol, int x1, int y1, std::string term, NameAlign align) : Shape(symbol), x1_(x1), y1_(y1), align_(align)
    {
        term_ = (getSymbol()->getCell())->getTerm(term);
    }

    TermShape::~TermShape()
    {
    }

    TermShape::NameAlign TermShape::strToAlign(std::string s)
    {
        if (s == "top_left")
            return TopLeft;
        if (s == "top_right")
            return TopRight;
        if (s == "bottom_left")
            return BottomLeft;
        if (s == "bottom_right")
            return BottomRight;
        std::cerr << "Alignement \"" << s << "\" inconnu. Alignement en haut à gauche par défaut." << std::endl;
        return TopLeft;
    }

    std::string TermShape::alignToStr(NameAlign align)
    {
        switch (align)
        {
            case TopLeft:
                return "top_left";

            case TopRight:
                return "top_right";

            case BottomLeft:
                return "bottom_left";

            case BottomRight:
                return "bottom_right";
        }
        return "top_left";
    }

    Shape *TermShape::fromXml(Symbol *symbol, xmlTextReaderPtr p)
    {
        const xmlChar *x1t = xmlTextReaderConstString(p, (const xmlChar *)"x1");
        const xmlChar *y1t = xmlTextReaderConstString(p, (const xmlChar *)"y1");
        const xmlChar *namet = xmlTextReaderConstString(p, (const xmlChar *)"name");
        const xmlChar *alignt = xmlTextReaderConstString(p, (const xmlChar *)"align");

        std::string x1 = xmlCharToString(xmlTextReaderGetAttribute(p, x1t));
        std::string y1 = xmlCharToString(xmlTextReaderGetAttribute(p, y1t));
        std::string name = xmlCharToString(xmlTextReaderGetAttribute(p, namet));
        std::string align = xmlCharToString(xmlTextReaderGetAttribute(p, alignt));

        if (x1 == "" || y1 == "" || name == "" || align == "")
            return nullptr;

        return new TermShape(symbol, stoi(x1), stoi(y1), name, strToAlign(align));
    }

    void TermShape::toXml(std::ostream &o) const
    {
        o << "<term name=\"" << term_->getName() << "\" x1=\"" << x1_ << "\" y1=\"" << y1_ << "\" align=\"" << alignToStr(align_) << "\"/>\n";
    }
}