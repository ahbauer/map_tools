//
//  exception_extension.hh
//
//  Code to deal with errors and debugging. 
//
//
//  Peter Andrews  Oct 7 2000
//

// Multiple inclusion guards
#ifndef HG_EXCEPTION_EXTENSION_HH
#define HG_EXCEPTION_EXTENSION_HH

#include <exception>
#include <iostream>
#include <string>
#include <typeinfo>

namespace ns_exception_extension {

  using std::cout;            // Declared in <iostream>
  using std::cerr;            // Declared in <iostream>
  using std::endl;            // Declared in <iostream>
  using std::string;          // Declared in <string>
  
  // Can be thrown and caught, and message read later.
  class message : public std::exception {

  private:

    string msg;

  public:

    message() : msg( "No message passed" ) {}
    message( const string& m ) : msg( m ) {}
    ~message() throw() {}

    void show() const
    {
      cerr << msg << endl;      
      cout.flush();
      cerr.flush();
    }

    // Okay.. I'm lying.  I suppose c_str() could throw...
    virtual const char * what() const throw() { return msg.c_str(); }

  };

  // instant_message instantly writes a message to cerr, and
  // stores the message for later use, if any.
  // It can be thrown and caught as an exception.
  //
  class instant_message : public message {

  public:

    instant_message( const string& m ) : message( m ) { show(); }

  };

  inline void report_exception( const std::exception& e, 
				const string& location = 
				"undisclosed location",
				bool what = true ) {
    cerr << "Caught exception " << typeid( e ).name() 
	 << " in " << location << "." << endl; 
    // Too complicated...
    if ( what && 
	 string( e.what() ) != string( message().what() ) &&
	 string( e.what() ) != string( std::exception().what() ) ) 
      cerr << "Message: " << e.what() << "." << endl;
  }
   


}

typedef ns_exception_extension::message my_exception;


#endif
