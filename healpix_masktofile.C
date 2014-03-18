#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <fstream>
using std::ofstream;
#include <healpix_base.h>
#include <healpix_map.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>

int main (int argc, char **argv){

  if( argc != 3 ){
    cerr << "Usage: healpix_masktofile mapname filename" << endl;
    cerr << "       Prints to the file the pixel IDs with value > 0.5" << endl;
    return 1;
  }
  
  string mapname(argv[1]);
  string filename(argv[2]);

  Healpix_Map<double> map;
  read_Healpix_map_from_fits( mapname, map );

  ofstream outfile( filename.c_str() );
  string scheme = "RING";
  if( map.Scheme() == 1 ){
      scheme = "NEST";
  }
  outfile << scheme << " " << map.Order() << endl;
  for( int i=0; i<map.Npix(); ++i ){
      if( map[i] > 0.5 )
          outfile << i << endl;
  }

  return 0;

}

