// -*- explicit-buffer-name: "Node.cpp<M1-MOBJ/4-5>" -*-

#include <limits>
#include "Cell.h"
#include "Node.h"
#include "Term.h"
#include "Net.h"
#include "Instance.h"
#include "Line.h"

namespace Netlist
{

	using namespace std;

	const size_t Node::noid = numeric_limits<size_t>::max();

	Node::Node(size_t id)
		: id_(id), position_(), lines_()
	{
	}

	Node::~Node()	//appelé dans le destructeur de NodePoint et NodeTerm
	{
		while (not lines_.empty())
		{
			delete lines_[0];
		}
	}

	void Node::attach(Line *line)
	{
		for (size_t i = 0; i < lines_.size(); ++i)
		{
			if (lines_[i] == line)
				return;
		}
		lines_.push_back(line);
	}

	void Node::detach(Line *line)
	{
		vector<Line *>::iterator iline = lines_.begin();
		for (; iline != lines_.end(); ++iline)
		{
			if ((*iline) == line)
			{
				lines_.erase(iline);
				break;
			}
		}
	}

	NodePoint::NodePoint(Net *net, size_t id, Point p)
		: Node(id), net_(net)
	{
		setPosition(p);
		net_->add(this);
	}

	NodePoint::~NodePoint()
	{
		net_->remove(this);
	}

	Net *NodePoint::getNet() const
	{
		return net_;
	}

	void NodePoint::toXml(ostream &stream) const
	{
		stream << indent << "<node x=\"" << position_.getX()
			   << "\" y=\"" << position_.getY()
			   << "\" id=\"" << id_ << "\"/>\n";
	}

	NodeTerm::NodeTerm(Term *term, size_t id):
	Node(id), term_(term)
	{
	}

	NodeTerm::~NodeTerm()
	{
		if (getNet())
			getNet()->remove(this);
	}

	inline Net *NodeTerm::getNet() const
	{
		return term_->getNet();
	}

	void NodeTerm::toXml(std::ostream &o) const
	{

		if ((term_)->isInternal()) // ici on va identifier le term est de type internal ou external afin de savoir si on est dans une instance ou pas
		{
			o << "<node term=\"" << (term_)->getName() << "\" instance=\"" << (term_->getInstance())->getName() << "\" id=\"" << id_ /*<< "\" x=\"" << position_.getX() << "\" y=\"" << position_.getY() */ << "\"/>\n";
		}
		else
		{
			o << "<node term=\"" << (term_)->getName() << "\" id=\"" << id_ /*<< "\" x=\"" << position_.getX() << "\" y=\"" << position_.getY() */ << "\"/>\n";
		}
	}

	bool Node::fromXml(Net *n, xmlTextReaderPtr p)
	{
		const xmlChar *cur_tag = xmlTextReaderConstLocalName(p);
		const xmlChar *nd = xmlTextReaderConstString(p, (const xmlChar *)"node");
		const xmlChar *term_tag = xmlTextReaderConstString(p, (const xmlChar *)"term");
		const xmlChar *inst_tag = xmlTextReaderConstString(p, (const xmlChar *)"instance");
		const xmlChar *id_tag = xmlTextReaderConstString(p, (const xmlChar *)"id");
		const xmlChar *x_tag = xmlTextReaderConstString(p, (const xmlChar *)"x");
		const xmlChar *y_tag = xmlTextReaderConstString(p, (const xmlChar *)"y");

		if (cur_tag != nd)
			return false;

		std::string term = xmlCharToString(xmlTextReaderGetAttribute(p, term_tag));
		std::string inst = xmlCharToString(xmlTextReaderGetAttribute(p, inst_tag));
		std::string id = xmlCharToString(xmlTextReaderGetAttribute(p, id_tag));
		std::string x = xmlCharToString(xmlTextReaderGetAttribute(p, x_tag));
		std::string y = xmlCharToString(xmlTextReaderGetAttribute(p, y_tag));

		if (id == "" /*|| x == "" || y == ""*/)
			return false;
		// curNode->setPosition(stoi(x), stoi(y));

		if (term == "")
		{
			if (x == "" || y == "")
				return false;
			new NodePoint(n, (size_t)stoi(id), Point(stoi(x), stoi(y)));
			return true;
		}
		else
		{
			if (inst == "")
			{
				Cell *curCell = n->getCell();
				Term *curTerm = curCell->getTerm(term); // term est le nom du term
				NodeTerm *curNode = static_cast<NodeTerm*>(curTerm->getNode());
				curNode->setId((size_t)stoi(id));
				(curTerm->getCell())->connect(term, n);
				return true;
			}
			else
			{
				Cell *curCell = n->getCell();
				Instance *curInst = curCell->getInstance(inst);
				Term *curTerm = curInst->getTerm(term);
				if(curTerm == nullptr) return false;	//Peut arriver si un fichier xml a été mal lu et qu'une cell Default a été utilisée
				NodeTerm *curNode = static_cast<NodeTerm*>(curTerm->getNode());
				curNode->setId((size_t)stoi(id));
				(curTerm->getInstance())->connect(term, n);
				return true;
			}
		}
	}
} // Netlist namespace.
