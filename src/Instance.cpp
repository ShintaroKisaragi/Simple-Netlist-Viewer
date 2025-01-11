#include "Instance.h"
#include "Cell.h"
#include "Term.h"
#include "Net.h"
#define BOLDRED "\033[1m\033[31m"
#define RESET "\033[0m"
#define YELLOW  "\033[33m"

namespace Netlist
{

    Instance::Instance(Cell *owner, Cell *model, const std::string &name) : owner_(owner),
                                                                            masterCell_(model),
                                                                            name_(name)
    {
        for (auto it = (masterCell_->getTerms()).begin(), end_terms = (masterCell_->getTerms()).end(); it != end_terms; ++it)
        {
            new Term(this, *it);
        }
        position_ = Point(); // init de la position à 0;0, changé plus tard si besoin
        owner->add(this);
    }

    // Accesseurs
    Instance::~Instance()
    {
        for (Term *t : terms_)
            delete t;
        owner_->remove(this);
    }

    const std::string &Instance::getName() const
    {
        return name_;
    }

    Cell *Instance::getMasterCell() const
    {
        return masterCell_;
    }

    Cell *Instance::getCell() const
    {
        return owner_;
    }

    const std::vector<Term *> &Instance::getTerms() const
    {
        return terms_;
    }

    Term *Instance::getTerm(const std::string &name) const
    {
        for (auto it = terms_.begin(), end = terms_.end(); it != end; ++it) // parsing des terms de l'instance
        {
            if ((*it)->getName() == name)
            {
                return *it;
            }
        }
        return nullptr; // si pas trouvé, nullptr
    }

    Point Instance::getPosition() const
    {
        return position_;
    }

    // Modifieurs

    bool Instance::connect(const std::string &name, Net *n)
    {
        for (auto it = terms_.begin(), end = terms_.end(); it != end; ++it)
        {
            if ((*it)->getName() == name)
            {
                (*it)->setNet(n); // besoin uniquement de changer le net car instance créée à partir d'une cell ==> nodes déjà présents
                return true;
            }
        }
        return false;
    }

    void Instance::add(Term *t)
    {
        terms_.push_back(t); // pas de leak sur terms_ ==> on erase au remove
    }

    void Instance::remove(Term *t)
    {
        for (auto it = terms_.begin(), end = terms_.end(); it != end; ++it)
        {
            if (*it == t)
            {
                terms_.erase(it);
            }
        }
    }

    void Instance::setPosition(const Point &p) // appelé plus tard
    {
        position_ = Point(p);  // Position de l'instance
        for (Term *t : terms_) // Parcours des terms de l'instance
        {
            Point p_symbol = ((t->getOwnerCell())->getSymbol())->getTermPosition(t); // Position du term dans le symbole
            p_symbol.translate(position_);                                           // Term du symbole translaté de la position de l'instance
            (t->getNode())->setPosition(p_symbol);                                   // réglage de la position du term d'instance
        }
    }

    void Instance::setPosition(int x, int y)    //utilisée pour la position de l'instance
    {
        setPosition(Point(x, y));
    }

    void Instance::toXml(std::ostream &o)
    {
        o << indent << "<instance name=\"" << name_ << "\" mastercell=\"" << masterCell_->getName() << "\" x=\"" << position_.getX() << "\" y=\"" << position_.getY() << "\"/>\n";
    }

    Instance *Instance::fromXml(Cell *c, xmlTextReaderPtr p)
    {
        const xmlChar *cur_tag = xmlTextReaderConstLocalName(p);
        const xmlChar *ist = xmlTextReaderConstString(p, (const xmlChar *)"instance");
        const xmlChar *name_tag = xmlTextReaderConstString(p, (const xmlChar *)"name");
        const xmlChar *mastercell_tag = xmlTextReaderConstString(p, (const xmlChar *)"mastercell");
        const xmlChar *x_tag = xmlTextReaderConstString(p, (const xmlChar *)"x");
        const xmlChar *y_tag = xmlTextReaderConstString(p, (const xmlChar *)"y");
        if (cur_tag != ist)
            return nullptr;

        std::string name = xmlCharToString(xmlTextReaderGetAttribute(p, name_tag));
        std::string mastercell = xmlCharToString(xmlTextReaderGetAttribute(p, mastercell_tag));
        std::string x = xmlCharToString(xmlTextReaderGetAttribute(p, x_tag));
        std::string y = xmlCharToString(xmlTextReaderGetAttribute(p, y_tag));

        if (name == "" || mastercell == "" || x == "" || y == "")
            return nullptr;

        Cell *model = Cell::find(mastercell);
        if(model == nullptr)    //si le modèle requis n'était pas déjà chargé, essayer de le lire depuis le xml
        {
            std::cerr << "Cellule modèle \"" << mastercell << "\" non chargée, tentative de lecture du xml..." << std::endl;
            model = Cell::load(mastercell);

            if(model == nullptr)    //si la lecture du fichier xml échoue pour une raison ou pour une autre...
            {
                std::cout << BOLDRED << "...fichier xml pour "<< mastercell << " introuvable ou endommagé." << RESET << std::endl;
                std:: cout << YELLOW << "Utilisation d'une cellule modèle vide à la place." << RESET << std::endl;
                model = Cell::find("Default");  //On utilise une cellule vide créée dans le main à la place. Évite certains segfault (gestion supplémentaire dans Node::setId)
            }
            else
            {
                std::cout << "...lecture de "<< mastercell <<" réussie" << std::endl;
            }
        }

        Instance *inst = new Instance(c, model, name);
        inst->setPosition(std::stoi(x), std::stoi(y));

        return inst;
    }
}