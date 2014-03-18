#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
#include <healpix_base.h>
#include <healpix_map.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>

int main (int argc, char **argv){

  if( argc != 2 ){
    cerr << "Usage: healpix_to_partpix filename" << endl;
    return 1;
  }

  string infilename(argv[1]);

  Healpix_Map<double> map;
  read_Healpix_map_from_fits( infilename, map );

  string scheme = "NEST";
  if( map.Scheme() == RING ){
      scheme = "RING";
  }
  cout << scheme << " " << map.Order() << endl;
  for( int i=0; i<map.Npix(); ++i ){
      if( map[i] != 0. ){
          cout << i << " " << map[i] << endl;
      }
  }

  return 0;

}
