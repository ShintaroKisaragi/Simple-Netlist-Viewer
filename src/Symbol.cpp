#include "Symbol.h"
#include "Term.h"
#include "Shape.h"
#include "Cell.h"
#define BOLDRED "\033[1m\033[31m"
#define RESET "\033[0m"

namespace Netlist
{
    Symbol::Symbol(Cell *c) : owner_(c)
    {
        shapes_ = std::vector<Shape *>();
    }

    Symbol::~Symbol()
    {
        for (auto it = shapes_.begin(), end = shapes_.end(); it != end; ++it)
        {
            delete *it;
        }
    }

    Cell *Symbol::getCell() const
    {
        return owner_;
    }

    Box Symbol::getBoundingBox() const
    {
        Box bb = Box();
        for (auto it = shapes_.begin(), end = shapes_.end(); it != end; ++it)
        {
            bb.merge((*it)->getBox());
        }

        return bb;
    }

    Point Symbol::getTermPosition(Term *t) const
    {
        //if(shapes_.empty())
        //    return Point();
        for (auto it = shapes_.begin(), end = shapes_.end(); it != end; ++it)
        {
            TermShape *s = dynamic_cast<TermShape *>(*it);
            if (s != nullptr)
            {
                if (s->getTerm() == t)
                {
                    return Point(s->getX1(), s->getY1());
                }
            }
        }
        //std::cerr << "Terminal/TermShape non trouvé. Renvoi d'un Point nul" << std::endl;
        return Point();
    }

    TermShape* Symbol::getTermShape(Term *t) const
    {
        for (auto it = shapes_.begin(), end = shapes_.end(); it != end; ++it)
        {
            if (TermShape *s = dynamic_cast<TermShape *>(*it))
            {
                if (s->getTerm() == t)
                {
                    return s;
                }
            }
        }
        std::cerr << "Terminal/TermShape non trouvé. Renvoi de nullptr" << std::endl;
        return nullptr;
    }

    void Symbol::add(Shape* s)
    {
        shapes_.push_back(s);
    }

    void Symbol::remove(Shape* s)
    {
        for(auto it=shapes_.begin(), end=shapes_.end(); it!=end; ++it)
        {
            if(*it == s)
            {
                shapes_.erase(it);
                return;
            }
        }
        std::cerr << "Shape introuvable" << std::endl;
    }

    void Symbol::toXml(std::ostream& o) const
    {
        Indentation indent = Indentation();
        ++indent;
        ++indent;
        for(Shape* s:shapes_)
        {
            o << indent;
            s->toXml(o);
        }
    }

    Symbol* Symbol::fromXml(Cell* c, xmlTextReaderPtr p)
    {
        const xmlChar *cur_tag = xmlTextReaderConstLocalName(p);
        const xmlChar *symt = xmlTextReaderConstString(p, (const xmlChar *)"symbol");

        if (cur_tag != symt)
            return nullptr;

        Symbol* sym = c->getSymbol();
        Shape* s;

        while (true)
        {
            int status = xmlTextReaderRead(p);
            cur_tag = xmlTextReaderConstLocalName(p);
            if (status != 1)
            {
                if (status != 0)
                {
                    std::cerr << BOLDRED << "[ERROR] Cell::fromXml(): Unexpected termination of the XML parser." << RESET << std::endl;
                }
                return nullptr;
            }

            switch (xmlTextReaderNodeType(p))
            {
            case XML_READER_TYPE_COMMENT:
            case XML_READER_TYPE_WHITESPACE:
            case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
                continue;
            }

            if (xmlTextReaderConstLocalName(p) == symt && xmlTextReaderNodeType(p) == XML_READER_TYPE_END_ELEMENT)
                return sym; // on vérifie si on atteint la balise </symbol>

            if ((s = Shape::fromXml(sym, p)))
            {
                sym->add(s);
            }
            else
            {
                std::cerr << "Erreur de placement du symbole à la ligne " << xmlTextReaderGetParserLineNumber(p) << std::endl;
            }
        }

        //if((cur_tag != linet)||(cur_tag != linet)||(cur_tag != linet)||(cur_tag != linet)||(cur_tag != linet))
    }
}