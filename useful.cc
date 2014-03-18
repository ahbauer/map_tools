//
//  useful.cc
//
//  Defines useful things like sqr() and abs(), etc.
//
//  Peter Andrews
//

#include "useful.hh"

#include <sstream>
using std::ostringstream;

#include <string>
using std::string;


template<class T>
string ns_useful::string_ize( const T& val )
{
  ostringstream s;
  s << val;
  return s.str();
}

template<class Iter>
void ns_useful::delete_pointers( Iter begin, Iter end ) 
{
  while( begin != end )
    delete *begin++;
}

