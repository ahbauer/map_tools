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

  if( argc != 4 ){
    cerr << "Usage: healpix_convert filename outfile output_resolution" << endl;
    return 1;
  }

  string infilename(argv[1]);
  string outfilename(argv[2]);
  int output_resolution = atoi(argv[3]);

  Healpix_Map<double> map;
  read_Healpix_map_from_fits( infilename, map );
  cerr << "Read in map with order " << map.Order() << ", scheme " << map.Scheme() << endl;

  Healpix_Map<double> outmap = Healpix_Map<double>(output_resolution, map.Scheme());
  outmap.Import(map);

  fitshandle myfits = fitshandle();
  myfits.create(outfilename.c_str());
  write_Healpix_map_to_fits(myfits, outmap, PLANCK_FLOAT64);
  myfits.close();

  return 0;

}
