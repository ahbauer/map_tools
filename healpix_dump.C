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
    cerr << "Usage: healpix_dump filename" << endl;
    return 1;
  }
  
  string filename(argv[1]);

  cerr << "Dumping " << filename << endl;

  Healpix_Map<double> map;
  read_Healpix_map_from_fits( filename, map );

  for( int i=0; i<map.Npix(); ++i )
    cout << i << " " << map[i] << endl;

  return 0;

}

