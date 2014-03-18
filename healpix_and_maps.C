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
    cerr << "Usage: healpix_and_masks mask1 mask2 outmask" << endl;
    cerr << "       in here you can set a flag to NOT the second mask..." << endl;
    return 1;
  }
  
  double mask_limit = 0.5;
  double NOTsecond = false;
  
  string infilename1(argv[1]);
  string infilename2(argv[2]);
  string outfilename(argv[3]);
  
  cerr << infilename1 << " & " << infilename2 << " -> " << outfilename << endl;
  
  Healpix_Map<double> map1i;
  read_Healpix_map_from_fits( infilename1, map1i );
  Healpix_Map<double> map2i;
  read_Healpix_map_from_fits( infilename2, map2i );
  
  int order = 1;
  if( map1i.Order() > map2i.Order() ){
      order = map1i.Order();
  }
  else{
      order = map2i.Order();
  }
  
  // this is a waste of ram, but i'm lazy and have lots of ram.
  Healpix_Map<double> map1( order, RING );
  map1.Import(map1i);
  Healpix_Map<double> map2( order, RING );
  map2.Import(map2i);
  
  for( int i=0; i<map1.Npix(); ++i ){
      if( NOTsecond ){
          if( map2[i] != 0. ){
              map1[i] = 0.;
          }
      }
      else{
          if( map2[i] < mask_limit ){
              map1[i] = 0.;
          }
      }
  }
  
  fitshandle myfits = fitshandle();
  myfits.create(outfilename.c_str());
  write_Healpix_map_to_fits(myfits, map1, PLANCK_FLOAT64);
  myfits.close();
  
  return 0;
  
}
