// -*- explicit-buffer-name: "XmlUtil.cpp<M1-MOBJ/6>" -*-

#include "XmlUtil.h"
#include <cstdlib>
#include <iostream>
#define BOLDRED "\033[1m\033[31m"
#define RESET "\033[0m"


namespace Netlist {

    using namespace std;

    std::string  xmlCharToString( xmlChar* xmls )
    {
        string s;

        if (xmls) {
            s = (char*)xmls;
            free( xmls );
        }

        return s;
    }

    bool  xmlGetIntAttribute ( xmlTextReaderPtr reader, const string& attribute, int& value )
    {
        xmlChar* textValue = xmlTextReaderGetAttribute( reader, (const xmlChar*)attribute.c_str() );

        if (textValue == NULL) {
            const xmlChar* nodeName = xmlTextReaderConstLocalName     ( reader );
            cerr << BOLDRED << "[ERROR] \"" << attribute << "\" attribute missing in <"
            << (const char*)nodeName << "> tag (line:" << xmlTextReaderGetParserLineNumber(reader) << ")." << RESET << endl;
            return false;
        }
        value = atoi( (char*)textValue );
        return true;
    }


}   // Netlist namespace.
