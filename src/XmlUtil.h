#pragma once

#include <libxml/xmlreader.h>
#include <string>

namespace Netlist {


  std::string  xmlCharToString    ( xmlChar* );
  bool         xmlGetIntAttribute ( xmlTextReaderPtr, const std::string& attribute, int& value );


}  // Netlist namespace.
