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

  if( argc != 3 ){
    cerr << "Usage: healpix_delta_to_n filename outfilename" << endl;
    return 1;
  }

  string infilename(argv[1]);
  string outfilename(argv[2]);

  Healpix_Map<double> map;
  read_Healpix_map_from_fits( infilename, map );

  Healpix_Map<double> outmap = Healpix_Map<double>(map.Order(), RING);

  double mean = 10.0; // arbitrary!
  for( int i=0; i<map.Npix(); ++i ){

      // this is specific for changing kappa to mu overdensities, 
      // and also only for a specific alpha value:
      double dmu = -0.292*2.0*map[i];
      outmap[i] = (dmu+1)*mean;
  }
  
  fitshandle myfits = fitshandle();
  myfits.create(outfilename.c_str());
  write_Healpix_map_to_fits(myfits, outmap, PLANCK_FLOAT64);
  myfits.close();

  return 0;

}
