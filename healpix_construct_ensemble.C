#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <fstream>
using std::ifstream;
#include <sstream>
using std::stringstream;
#include <healpix_base.h>
#include <healpix_map.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>

int main (int argc, char **argv){

  if( argc != 2 ){
    cerr << "Usage: healpix_construct_ensemble filename" << endl;
    return 1;
  }
  
  string filename(argv[1]);


  ifstream mockfile;
  for( unsigned int mapnum = 0; mapnum < 125; ++mapnum ){

    Healpix_Map<double> mymap = Healpix_Map<double>(8, RING);
    mymap.fill(0.);

    mockfile.open( filename.c_str() );
    while( ! mockfile.eof() ){
      char line[4096];
      mockfile.getline( line, 4096 );
      double ra, dec;
      int row, num;
      stringstream linestream(line);
      linestream >> ra >> dec >> row;
      for( unsigned int i=0; i<=mapnum; ++i )
	linestream >> num;
      
      double phi = ra*3.1415926/180.;
      double theta = (90.-dec)*3.1415926/180.;
      pointing mypointing(theta, phi);
      int pixnum = mymap.ang2pix( mypointing );
      //if( mymap[pixnum] != 0. ){
      //cout << "Reassigning " << pixnum << " " << mymap[pixnum] << " to " << num << ", pos " << ra << " " << dec << endl;
      //}
      mymap[pixnum] = num;
    }
    mockfile.close();

    fitshandle myfits;
    stringstream fitsname;
    fitsname << "map" << mapnum << ".fits"; 
    myfits.create(fitsname.str());
    write_Healpix_map_to_fits(myfits, mymap, PLANCK_FLOAT64);
    myfits.close();

  }

  return 0;

}
