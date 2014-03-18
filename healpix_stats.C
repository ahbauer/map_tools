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
    cerr << "Usage: healpix_stats filename resolution(e.g. 10)" << endl;
    return 1;
  }
  
  string filename(argv[1]);
  int resolution = atoi(argv[2]);

  cerr << "Using " << filename << " which has resolution " << resolution << endl;

  Healpix_Map<double> map = Healpix_Map<double>(10, RING);
  read_Healpix_map_from_fits( filename, map );

  double avg = map.average();
  double rms = map.rms();

  cout << "Mean: " << avg << " RMS: " << rms << endl;

  return 0;

}
