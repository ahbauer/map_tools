#include <vector>
using std::vector;
#include <string>
using std::string;
#include <sstream>
using std::stringstream;
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <iomanip>
using namespace std;
//#include "../Healpix_2.15a/src/cxx/Healpix_cxx/healpix_base.h"
#include <healpix_base.h>
#include <healpix_map.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>

int main (int argc, char **argv){

  string listname = argv[1];
  string maskname = argv[2];
  cerr << "running " << listname << " past the mask " << maskname << endl;

  // assuming mask resolution!!

  Healpix_Map<double> mask = Healpix_Map<double>(8, RING);
  read_Healpix_map_from_fits( maskname.c_str(), mask );

  ifstream listfile( listname.c_str() );
  while( ! listfile.eof() ){
    char line[2048];
    listfile.getline( line, 2048);
    double ra, dec;
    stringstream linestream(line);
    linestream >> ra >> dec;
    double phi = ra*3.1415926/180.;
    double theta = (90.-dec)*3.1415926/180.;
    pointing mypointing(theta, phi);
    int pixnum = mask.ang2pix( mypointing );
    //    fprintf( stdout, "test %s\n", line );
    if( mask[pixnum] > 0.5 )
      fprintf( stdout, "%s\n", line );
  }

  return 0;

}
