#pragma once

#include "Point.h"
#include "Cell.h"
#include "Term.h"

namespace Netlist
{

    class Net
    {
    private:
        Cell *owner_;
        std::string name_;
        unsigned int id_;
        Term::Type type_;
        std::vector<Node *> nodes_;
        std::vector<Line *> lines_;

    public:
        Net(Cell *, const std::string &, Term::Type);
        ~Net();

    public:
        Cell *getCell() const;
        const std::string &getName() const;
        unsigned int getId() const;
        Term::Type getType() const;
        const std::vector<Node *> &getNodes() const;
        size_t getFreeNodeId() const;
        const std::vector<Line *> &getLines() const;
        Node* getNode(size_t id) const;

    public:
        void add(Node *);
        bool remove(Node *);
        void toXml(std::ostream &o);
        static Net *fromXml(Cell *, xmlTextReaderPtr);
        void add(Line* line);
        bool remove(Line* line);
    };
}