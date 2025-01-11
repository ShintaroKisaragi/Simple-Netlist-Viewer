#include <fstream>
#include <cstdlib>
#include "Cell.h"
#include "Term.h"
#include "Net.h"
#include "Instance.h"
#include "Symbol.h"

#define BOLDRED "\033[1m\033[31m"
#define RESET "\033[0m"

namespace Netlist
{

	using namespace std;

	vector<Cell *> Cell::cells_;

	std::vector<Cell *>& Cell::getAllCells()
	{
		return cells_;
	}

	Cell *Cell::find(const string &name)
	//Cherche une cell dans le tableau de toutes les cells chargées en comparant leurs noms
	{
		for (vector<Cell *>::iterator icell = cells_.begin(); icell != cells_.end(); ++icell)
		{
			if ((*icell)->getName() == name)
				return *icell;
		}
		return NULL;
	}

	Cell::Cell(const string &name) : symbol_(this), name_(name), terms_(), instances_(), nets_(), maxNetIds_(0)
	{
		if (find(name))
		{
			cerr << BOLDRED << "[ERROR] Attempt to create duplicate of Cell <" << name << RESET << ">.\n"
				 << "        Aborting..." << endl;
			exit(1);
		}
		cells_.push_back(this);
	}

	Cell::~Cell()
	{
		for (vector<Cell *>::iterator icell = cells_.begin(); icell != cells_.end(); ++icell)
		{
			if (*icell == this)
			{
				cells_.erase(icell);
				break;
			}
		}

		while (not nets_.empty())
			delete *nets_.begin();
		while (not instances_.empty())
			delete *instances_.begin();
		while (not terms_.empty())
			delete *terms_.begin();
	}

	Instance *Cell::getInstance(const std::string &name) const
	//Cherche une instance par son nom dans le tableau de la cell actuelle
	{
		for (vector<Instance *>::const_iterator iinst = instances_.begin(); iinst != instances_.end(); ++iinst)
		{
			if ((*iinst)->getName() == name)
				return *iinst;
		}
		return NULL;
	}

	Term *Cell::getTerm(const std::string &name) const
	//Cherche un term par son nom dans le tableau des terms de la cell (terms externes)
	{
		for (vector<Term *>::const_iterator iterm = terms_.begin(); iterm != terms_.end(); ++iterm)
		{
			if ((*iterm)->getName() == name)
				return *iterm;
		}
		return NULL;
	}

	Net *Cell::getNet(const std::string &name) const
	//Cherche un net (fil de connexion) par son étiquette (nom) dans le tableau des nets de la cell
	{
		for (vector<Net *>::const_iterator inet = nets_.begin(); inet != nets_.end(); ++inet)
		{
			if ((*inet)->getName() == name)
				return *inet;
		}
		return NULL;
	}

	void Cell::setName(const string &cellName)
	//Change le nom de la cell actuelle. Si le nom est déjà attribué, annule le changement
	{
		if (cellName == name_)
			return;
		if (find(cellName) != NULL)
		{
			cerr << BOLDRED << "[ERROR] Cell::setName() - New Cell name <" << cellName << "> already exists." << RESET << endl;
			return;
		}
		name_ = cellName;
	}

	void Cell::add(Instance *instance)
	//Ajoute une instance à la cell à partir d'un modèle déjà existant
	{
		if (getInstance(instance->getName()))
		{
			cerr << BOLDRED << "[ERROR] Attemp to add duplicated instance <" << instance->getName() << ">." << RESET << endl;
			exit(1);
		}
		instances_.push_back(instance);
	}

	void Cell::add(Term *term)
	//Ajoute un terminal externe préexistant à la cell
	{
		if (getTerm(term->getName()))
		{
			cerr << BOLDRED << "[ERROR] Attemp to add duplicated terminal <" << term->getName() << ">." << RESET << endl;
			exit(1);
		}
		terms_.push_back(term);
	}

	void Cell::add(Net *net)
	//Ajoute un net préexistant à la cell
	{
		if (getNet(net->getName()))
		{
			cerr << BOLDRED << "[ERROR] Attemp to add duplicated Net <" << net->getName() << ">." << RESET << endl;
			exit(1);
		}
		nets_.push_back(net);
	}

	bool Cell::connect(const string &name, Net *net)
	//Connecte un term de la cell à un net
	{
		Term *term = getTerm(name);
		if (term == NULL)
			return false;

		term->setNet(net);
		return true;
	}

	void Cell::remove(Instance *instance)
	//Retire une instance de la cell
	{
		for (vector<Instance *>::iterator iinst = instances_.begin(); iinst != instances_.end(); ++iinst)
		{
			if (*iinst == instance)
				instances_.erase(iinst);
		}
	}

	void Cell::remove(Term *term)
	//Retire un term de la cell
	{
		for (vector<Term *>::iterator iterm = terms_.begin(); iterm != terms_.end(); ++iterm)
		{
			if (*iterm == term)
				terms_.erase(iterm);
		}
	}

	void Cell::remove(Net *net)
	//retire un net de la cell
	{
		for (vector<Net *>::iterator inet = nets_.begin(); inet != nets_.end(); ++inet)
		{
			if (*inet == net)
				nets_.erase(inet);
		}
	}

	unsigned int Cell::newNetId()
	{
		return maxNetIds_++;
	}

	void Cell::toXml(std::ostream &o) const
	//Appelée depuis save, fait l'écriture effective du xml
	{
		Indentation indent = Indentation(); // permet de traquer les niveaux d'indentation

		o << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" // déclarartion du fichier
		  << "<cell name=\"" << name_ << "\">\n"			// début de la Cell
		  << (++indent) << "<terms>\n";

		++indent;

		// toXml du Term
		for (auto it = terms_.begin(), end = terms_.end(); it != end; ++it)
		{
			o << indent;
			(*it)->toXml(o);
		}

		--indent;

		o << indent << "</terms>\n"
		  << indent << "<instances>\n";

		++indent;

		// toXml de l'instance
		for (auto it = instances_.begin(), end = instances_.end(); it != end; ++it)
		{
			o << indent;
			(*it)->toXml(o);
		}

		--indent;

		o << indent << "</instances>\n"
		  << indent << "<nets>\n";

		++indent;

		// toXml du Net -> Nodes
		for (auto n = nets_.begin(), n_end = nets_.end(); n != n_end; n++)
		{
			o << indent << "<net name=\"" << (*n)->getName() << "\" type=\"" << Term::toString((*n)->getType()) << "\">\n";

			(*n)->toXml(o);
			o << indent << "</net>\n";
		}

		--indent;

		o << indent << "</nets>\n"
		  << indent << "<symbol>\n";

		symbol_.toXml(o);

		o << indent << "</symbol>\n";

		o << --indent << "</cell>\n";
	}

	Cell *Cell::fromXml(xmlTextReaderPtr reader)
	//appelée depuis load, lit effectivement le fichier xml
	{
		enum State
		{
			Init = 0,
			BeginCell,
			BeginTerms,
			EndTerms,
			BeginInstances,
			EndInstances,
			BeginNets,
			EndNets,
			BeginSymbol,
			EndCell,
			ParseError
		};

		const xmlChar *cellTag = xmlTextReaderConstString(reader, (const xmlChar *)"cell");
		const xmlChar *netsTag = xmlTextReaderConstString(reader, (const xmlChar *)"nets");
		const xmlChar *termsTag = xmlTextReaderConstString(reader, (const xmlChar *)"terms");
		const xmlChar *instancesTag = xmlTextReaderConstString(reader, (const xmlChar *)"instances");
		const xmlChar *symbolTag = xmlTextReaderConstString(reader, (const xmlChar *)"symbol"); // TME7

		Cell *cell = NULL;
		State state = Init;

		while (true)
		{
			int status = xmlTextReaderRead(reader);
			if (status != 1)
			{
				if (status != 0)
				{
					cerr << BOLDRED << "[ERROR] Cell::fromXml(): Unexpected termination of the XML parser." << RESET << endl;
				}
				break;
			}

			switch (xmlTextReaderNodeType(reader))
			{
			case XML_READER_TYPE_COMMENT:
			case XML_READER_TYPE_WHITESPACE:
			case XML_READER_TYPE_SIGNIFICANT_WHITESPACE:
				continue;
			}

			const xmlChar *nodeName = xmlTextReaderConstLocalName(reader);

			switch (state)
			{
			case Init:
				if (cellTag == nodeName)
				{
					state = BeginCell;
					string cellName = xmlCharToString(xmlTextReaderGetAttribute(reader, (const xmlChar *)"name"));
					if (not cellName.empty())
					{
						cell = new Cell(cellName);
						state = BeginTerms;
						continue;
					}
					else
						state = ParseError;
				}
				break;
			case BeginTerms:
				if ((nodeName == termsTag) and (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT))
				{
					state = EndTerms;
					continue;
				}
				break;
			case EndTerms:
				if ((nodeName == termsTag) and (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT))
				{
					state = BeginInstances;
					continue;
				}
				else
				{
					if (Term::fromXml(cell, reader))
						continue;
				}
				break;
			case BeginInstances:
				if ((nodeName == instancesTag) and (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT))
				{
					state = EndInstances;
					continue;
				}
				break;
			case EndInstances:
				if ((nodeName == instancesTag) and (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT))
				{
					state = BeginNets;
					continue;
				}
				else
				{
					if (Instance::fromXml(cell, reader))
						continue;
				}
				break;
			case BeginNets:
				if ((nodeName == netsTag) and (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT))
				{
					state = EndNets;
					continue;
				}
				break;
			case EndNets:
				if ((nodeName == netsTag) and (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT))
				{
					state = BeginSymbol; // TME7
					continue;
				}
				else
				{
					if (Net::fromXml(cell, reader))
						continue;
				}
				break;
			case BeginSymbol: // TME7
				if ((nodeName == symbolTag) and (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT))
				{
					if (Symbol::fromXml(cell, reader))
					{
						state = EndCell;
						continue;
					}
				}
				break;
			case EndCell:
				if ((nodeName == cellTag) and (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT))
				{
					continue;
				}
				break;
			default:
				break;
			}

			cerr << BOLDRED << "[ERROR] Cell::fromXml(): Unknown or misplaced tag <" << nodeName
				 << "> (line:" << xmlTextReaderGetParserLineNumber(reader) << ")." << RESET << endl;
			break;
		}

		return cell;
	}

	Cell *Cell::load(const string &cellName)
	//Charge une cellule stockée dans le dossier cells. Si la cellule n'y est pas, renvoie nullptr
	{
		string cellFile = "../cells/" + cellName + ".xml";
		xmlTextReaderPtr reader;

		reader = xmlNewTextReaderFilename(cellFile.c_str());
		if (reader == NULL)
		{
			cerr << BOLDRED << "[ERROR] Cell::load() unable to open file <" << cellFile << ">." << RESET << endl;
			return NULL;
		}

		Cell *cell = Cell::fromXml(reader);
		xmlFreeTextReader(reader);

		for(Instance* i:cell->getInstances())
		{
			i->setPosition(i->getPosition());
		}

		return cell;
	}

	void Cell::save(const std::string& name, const std::string& path) const
	//prend le nom du fichier et son chemin pour enregistrer une cell au format xml via Cell::toXml
	{
		string fileName = path + "/" + name + ".xml";
		fstream fileStream(fileName.c_str(), ios_base::out | ios_base::trunc);
		if (not fileStream.good())
		{
			cerr << BOLDRED << "[ERROR] Cell::save() unable to open file <" << fileName << ">." << RESET << endl;
			return;
		}

		cerr << "Saving <Cell " << getName() << "> in <" << fileName << ">" << endl;
		this->toXml(fileStream);

		fileStream.close();
	}
} // Netlist namespace.
