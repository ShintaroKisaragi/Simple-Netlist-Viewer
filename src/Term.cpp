
#include "Node.h"
#include "Net.h"
#include "Instance.h"
#include "Term.h"
#include "Cell.h"
#include <vector>
namespace Netlist
{

    Term::Term(Cell *c, const std::string &name, Direction d) : owner_(c),
                                                                name_(name),
                                                                direction_(d),
                                                                type_(External),
                                                                net_(),
                                                                node_(this, Node::noid)
    {
        c->add(this);
    } // l'ajout du term dans le vector du Cell actuel

    Term ::Term(Instance *inst, const Term *modelTerm) : owner_(inst),
                                                         name_(modelTerm->name_),
                                                         direction_(modelTerm->direction_),
                                                         type_(Internal),
                                                         net_(),
                                                         node_(this, Node::noid)
    {
        inst->add(this);
    } // meme chose ici

    Term ::~Term()
    {
        if (this->isExternal())
        {
            this->getCell()->remove(this); //  si c'est un term external, on le supprime du Cell qui le possede
        }
        else
        {
            this->getInstance()->remove(this);
        }
    }

    Cell *Term::getOwnerCell() const
    {
        if (this->isExternal())
        {
            return this->getCell();
        }
        else if (this->isInternal())
        {
            return static_cast<Instance *>(owner_)->getCell(); // ici on utilise la fonction getCell() proprement définit dans classe Instance (sachant qu'une instance est tjrs dans une Cell)
        }
        return nullptr;
    }

    void Term::setNet(Net *n)
    { // il fallait stocker la node du term dans le vector de net
        if (n == NULL)
        { // si n=null cela veut dire que l'on veut ajouter une case vide dans le vector node_ de la classe Net
            if (net_ != nullptr)
                net_->remove(&node_);
            net_ = nullptr;
        }
        else
        {
            if (net_ != nullptr)
                net_->remove(&node_); // si le term était déjà connecté à un autre net, le déconnecter
            net_ = n;
            // node_.setId(net_->getFreeNodeId());
            net_->add(&node_);
        }
    }

    void Term::setNet(std::string name)
    {
        Net *newnet = getOwnerCell()->getNet(name); // récupération du nouveau net
        if (net_ != NULL)
        {
            setNet(newnet);
        }
        else
        {
            std::cout << "Net non trouvé. Abandon de la connexion (net inchangé)" << std::endl;
        }
    }

    void Term::setPosition(const Point &p)
    {
        node_.setPosition(p); // la classe Point est tjrs lié a la classe Node
    }

    void Term::setPosition(int x, int y)
    {
        node_.setPosition(x, y);
    }

    std::string Term::toString(Term::Type t) // Switch sur Type (return final uniquement pour éviter warning à la compilation)
    {
        switch (t)
        {
        case Term::Type::Internal:
            return "Internal";
        case Term::Type::External:
            return "External";
        }
        return "External";
    }

    std::string Term::toString(Term::Direction d) // switch sur Diretion (return final uniquement pour éviter warning à la compilation)
    {
        switch (d)
        {
        case Term::Direction::In:
            return "In";
        case Term::Direction::Out:
            return "Out";
        case Term::Direction::Inout:
            return "Inout";
        case Term::Direction::Tristate:
            return "Tristate";
        case Term::Direction::Transcv:
            return "Transcv";
        case Term::Direction::Unknown:
            return "Unknown";
        }
        return "Unkown";
    }

    Term::Direction Term::toDirection(std::string s) // pas de possibilité de faire de switch : les string ne sont pas des enums
    {
        if (s == "In")
            return Term::Direction::In;
        if (s == "Out")
            return Term::Direction::Out;
        if (s == "Inout")
            return Term::Direction::Inout;
        if (s == "Tristate")
            return Term::Direction::Tristate;
        if (s == "Transcv")
            return Term::Direction::Transcv;
        if (s == "Unknown")
            return Term::Direction::Unknown;
        std::cout << "Direction non reconnue. Mise à Unkown." << std::endl;
        return Term::Direction::Unknown;
    }
    void Term::toXml(std::ostream &o)
    { // ici on détermine la fonction toXml de la classe Term
        o << "<term name=\"" << name_ << "\" direction=\"" << toString(direction_) << "\" x=\"" << (getNode()->getPosition()).getX() << "\" y=\"" << (getNode()->getPosition()).getY() << "\"/>\n";
    }

    Term *Term::fromXml(Cell *c, xmlTextReaderPtr p)
    {
        const xmlChar *cur_tag = xmlTextReaderConstLocalName(p);
        const xmlChar *trm = xmlTextReaderConstString(p, (const xmlChar *)"term");
        const xmlChar *name_tag = xmlTextReaderConstString(p, (const xmlChar *)"name");
        const xmlChar *dir = xmlTextReaderConstString(p, (const xmlChar *)"direction");
        const xmlChar *x_tag = xmlTextReaderConstString(p, (const xmlChar *)"x");
        const xmlChar *y_tag = xmlTextReaderConstString(p, (const xmlChar *)"y");

        if (cur_tag != trm)
            return nullptr;

        std::string name = xmlCharToString(xmlTextReaderGetAttribute(p, name_tag));
        std::string direction = xmlCharToString(xmlTextReaderGetAttribute(p, dir));
        std::string x = xmlCharToString(xmlTextReaderGetAttribute(p, x_tag));
        std::string y = xmlCharToString(xmlTextReaderGetAttribute(p, y_tag));

        if (name == "" || direction == "")
            return nullptr;

        Term *t = new Term(c, name, toDirection(direction));
        t->setPosition(stoi(x), stoi(y));

        return t;
    }
}