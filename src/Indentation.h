#pragma once

#include <string>
#include <iostream>

namespace Netlist {
  
  class Indentation {
    public:
                    Indentation ();
      Indentation&  operator++  ();
      Indentation   operator++  (int);
      Indentation&  operator--  ();
      Indentation   operator--  (int);
    private:
      size_t        _tabulationSize;
      std::string   _indentation;

      friend std::ostream& operator<< ( std::ostream&, const Indentation& );
  };
  extern Indentation  indent;
}  // Netlist namespace.
