//
//  useful.hh
//
//  Defines useful things like sqr() and abs(), etc.
//
//  Peter Andrews
//

// Multiple inclusion guards
#ifndef HG_USEFUL_HH
#define HG_USEFUL_HH

#include <string>

#include <cmath>
using std::log10;
using std::sqrt;


// Assorted useful functions and classes.
namespace ns_useful {

  using std::string;                          // Declared in <string>

  // Returns the absolute value of passed argument.
  template<class T>
  inline T abs( T val ) { return val > 0 ? val : -val; }

  // Returns the square of passed argument.
  template<class T>
  inline T sqr( T val ) { return val * val; }

  // Returns the fractional part of passed argument.
  template <class T>
  inline T fractional_part( T val ) { return val - static_cast<int>( val ); }

  // Use when doing a max or min of two different types.  
  // The result is of the left argument's type.
  // Returns the maximum of two arguments, of the left argument type.
  template<class L, class R>
  inline L lmax( const L& l, const R& r ) { return l>(L)r?l:r; }
  // Returns the minimum of two arguments, of the left argument type.
  template<class L, class R>
  inline L lmin( const L& l, const R& r ) { return l<(L)r?l:r; }

  // Takes a value, makes it look like a function.
  //
  // This class encapsulates a variable of (almost) any type.
  // For example, if you have an int variable: <BR>
  // int my_var;<BR>
  // you use the variable as normal:<BR>
  // my_var = 5;<BR>
  // A function_maker encapsulating this int is defined and used like this:<BR>
  // function_maker<int> my_var2( my_var );<BR>
  // my_var2() = 5;<BR>
  // The only difference is that you put the () after the variable name.
  //
  template<class V>
  class function_maker {

    // The stored value.  Must be accessed through member functions.
    V val;

  public:

    // Constructors.

    // Default value, only works for types that can be constructed with 0.
    function_maker() : val( 0 ) {}
    // Encapsulate a value in a function_maker.
    template<class S>
    function_maker( const S& v ) : val( static_cast<V>( v ) ) {}
    // Create a function maker from another function maker.
    template<class S>
    function_maker( const function_maker<S>& v ) : 
      val( static_cast<V>( v() ) ) {}

    // Value return functions.

    // Return the stored value as a modifiable reference.
    V& operator() () { return val; }
    // Return the stored value.
    V operator() () const { return val; }

    // Return the stored value as a modifiable reference.  Verbose form.
    V& value() { return val; }
    // Return the stored value.  Verbose form.
    V value() const { return val; }

  };

  // Takes a value of any printable type, returns a string of that value.
  template<class T>
  string string_ize( const T& val );

  // If begin and end are a sequence of pointers, this function
  // deletes what the pointers point to.
  template<class Iter>
  void delete_pointers( Iter begin, Iter end );

  // Pi to a couple of significant figures.
  const double pi = 3.1415926535897932385;

  // A class designed to return true
  // Used to simulate an image mask where needed.
  class return_true {
  public:
    template<class T> 
    bool operator() ( const T, const T ) const { return true; }
  };

  template <class Obj> 
  class compare_ra {
  public:
    bool operator() ( const Obj& left, const Obj& right ) const {
      if ( left.ra == right.ra ) {
	return left.dec < right.dec;
      }
      else {
	return left.ra < right.ra;
      }
    }
  };

  template <class Obj> class compare_dec {
  public:
    bool operator() ( const Obj& left, const Obj& right ) const {
      if ( left.dec == right.dec ) {
	return left.ra < right.ra;
      }
      else {
	return left.dec < right.dec;
      }
    }
  };

}

#include "useful.cc"

#endif
