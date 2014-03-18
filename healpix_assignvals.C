#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <sstream>
using std::stringstream;
#include <fstream>
using std::ifstream;
#include <healpix_base.h>
#include <healpix_map.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>

int main (int argc, char **argv){

  if( argc != 4 ){
    cerr << "Usage: healpix_assignvals filname mapname resolution(e.g. 10)" << endl;
    cerr << "       where the first two columns of filename are RA Dec" << endl;
    return 1;
  }
  
  string filename(argv[1]);
  string mapname(argv[2]);
  int resolution = atoi(argv[3]);

  Healpix_Map<double> map = Healpix_Map<double>(resolution, RING);
  read_Healpix_map_from_fits( mapname, map );

  ifstream infile( filename.c_str() );
  while( !infile.eof() ){
    char line[2048];
    infile.getline( line, 2048);
    double ra, dec;
    stringstream linestream(line);
    linestream >> ra >> dec;
    double phi = ra*3.1415926/180.;
    double theta = (90.-dec)*3.1415926/180.;
    pointing mypointing(theta, phi);
    double val = map.interpolated_value( mypointing );
    cout << line << " " << val << endl;
  }

  return 0;

}

