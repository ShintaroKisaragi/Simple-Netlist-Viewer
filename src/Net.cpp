#include "Net.h"
#include "XmlUtil.h"
#include "Line.h"
#define BOLDRED "\033[1m\033[31m"
#define RESET "\033[0m"
#define YELLOW  "\033[33m"

namespace Netlist
{
    std::string newEndl = "\"/>\n";
    Net::Net(Cell *c, const std::string &s, Term::Type t) : owner_(c),
                                                            name_(s),
                                                            type_(t)
    {
        id_ = c->newNetId();
        c->add(this); // ajout du net à sa cell
    }

    Net::~Net() {}

    // accesseurs

    Cell *Net::getCell() const
    {
        return owner_;
    }

    const std::string &Net::getName() const
    {
        return name_;
    }

    unsigned int Net::getId() const
    {
        return id_;
    }

    Term::Type Net::getType() const
    {
        return type_;
    }

    const std::vector<Node *> &Net::getNodes() const
    {
        return nodes_;
    }

    size_t Net::getFreeNodeId() const // permet de réexploiter les emplacements vides si suppression, puis ajout d'un node
    {
        for (size_t i = 0; i < nodes_.size(); ++i)
        {
            if (nodes_[i] == nullptr)
            {
                return i;
            }
        }
        return Node::noid;
    }

    void Net::add(Node *n)
    {
        if (n == nullptr)
            return;

        size_t cur_id = n->getId();

        if (cur_id == Node::noid)
        {
            size_t index = Net::getFreeNodeId();
            if (index == Node::noid) // si nodes n'a pas d'emplacement libre, pushback et changement de l'ID du Node
            {
                n->setId(nodes_.size());
                nodes_.push_back(n);
                cur_id = nodes_.size() - 1;
            }
            else
            {
                n->setId(index); // réutilisation d'un emplacement préalablement laissé libre
                nodes_[index] = n;
                cur_id = index;
            }
            return;
        }
        if (cur_id < nodes_.size())
        {
            if (nodes_[cur_id] != nullptr)
            {
                std::cerr << YELLOW << "Ecrasement d'un terminal" << RESET << std::endl;
                nodes_[cur_id]->setId(Node::noid);
            }
            nodes_[cur_id] = n;
        }
        else
        {
            for (size_t i = nodes_.size(); i < cur_id; ++i)
                nodes_.push_back(nullptr);
            nodes_.push_back(n);
        }
    }

    bool Net::remove(Node *n)
    {
        for (auto it = nodes_.begin(), end = nodes_.end(); it != end; ++it)
        {
            if (*it == n)
            {
                n->setId(Node::noid);
                *it = nullptr;
                return true;
            }
        }
        return false;
    }

    void Net::toXml(std::ostream &o)
    {
        Indentation indent = Indentation();
        ++indent;
        ++indent;
        ++indent;
        for (auto n = nodes_.begin(), n_end = nodes_.end(); n != n_end; n++)
        {
            if (*n == nullptr)
                continue;
            o << indent;
            (*n)->toXml(o);
        }

        for (auto l = lines_.begin(), l_end = lines_.end(); l != l_end; l++)
        {
            if (*l == nullptr)
                continue;
            o << indent;
            (*l)->toXml(o);
        }
    }

    Node *Net::getNode(size_t id) const
    {
        if (id >= nodes_.size())
        {
            std::cerr << "Non existing node. Returining nullptr instead." << std::endl;
            return nullptr;
        }
        return nodes_[id];
    }

    Net *Net::fromXml(Cell *c, xmlTextReaderPtr p)
    {
        Net *n;
        const xmlChar *cur_tag = xmlTextReaderConstLocalName(p);
        const xmlChar *nt = xmlTextReaderConstString(p, (const xmlChar *)"net");
        const xmlChar *ndt = xmlTextReaderConstString(p, (const xmlChar *)"node");
        const xmlChar *lnt = xmlTextReaderConstString(p, (const xmlChar *)"line");
        const xmlChar *name_tag = xmlTextReaderConstString(p, (const xmlChar *)"name");
        const xmlChar *typ = xmlTextReaderConstString(p, (const xmlChar *)"type");

        std::string name = xmlCharToString(xmlTextReaderGetAttribute(p, name_tag));
        std::string type = xmlCharToString(xmlTextReaderGetAttribute(p, typ));

        if ((cur_tag != nt))
            return nullptr;

        if (name == "" || type == "")
            return nullptr;

        if (type == "External")
        {
            n = new Net(c, name, Term::Type::External);
        }
        else
        {
            n = new Net(c, name, Term::Type::Internal); // cas par défaut. Peut "corriger" une faute de frappe, mais peut être source d'incohérences
        }

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

            if (xmlTextReaderConstLocalName(p) == nt && xmlTextReaderNodeType(p) == XML_READER_TYPE_END_ELEMENT)
                return n; // on vérifie si on atteint la balise </net> ==> on return le net le cas écéhant

            if (cur_tag == ndt)
            {
                if (!Node::fromXml(n, p))
                {
                    std::cerr << "Erreur de placement du node à la ligne " << xmlTextReaderGetParserLineNumber(p) << std::endl;
                }
            }
            else if (cur_tag == lnt)
            {
                if (!Line::fromXml(n, p))
                {
                    std::cerr << "Erreur de placement du node à la ligne " << xmlTextReaderGetParserLineNumber(p) << std::endl;
                }
            }
        }
    }

    const std::vector<Line *> &Net::getLines() const
    {
        return lines_;
    }

    void Net::add(Line *line)
    {
        if (line)
            lines_.push_back(line);
    }

    bool Net::remove(Line *line)
    {
        if (line)
        {
            for (auto il = lines_.begin(); il != lines_.end(); ++il)
            {
                if (*il == line)
                {
                    lines_.erase(il);
                    return true;
                }
            }
        }
        return false;
    }
}