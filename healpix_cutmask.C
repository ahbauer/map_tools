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

    if( argc != 3 ){
        cerr << "Usage: healpix_cutmask inputmask output_maskname" << endl;
        cerr << "Writes another healpix map which makes any desired cuts on the first one" << endl;
        cerr << "(like cutting out the LMC)" << endl;
        cerr << "note: makes the bad values = 0, not HEALPIX_NDEF or anything." << endl;
        return 1;
    }

    string maskname(argv[1]);
    string outname(argv[2]);

    Healpix_Map<double> mask;
    read_Healpix_map_from_fits( maskname.c_str(), mask );
    for( int i=0; i<mask.Npix(); ++i ){
        pointing mypointing = mask.pix2ang(i);
        double ra = mypointing.phi*180./3.1415926;
        double dec = 90.0 - mypointing.theta*180./3.1415926;

        if( mask[i] == Healpix_undef || isnan(mask[i]) )
            mask[i] = 0.;

        // THE CUTS:
        if( ra<60. || ra>95. || dec<-65. || dec>-40 ){
            mask[i] = 0.;
        }
    }

      
    fitshandle myfits;
    
    // system( "rm offset.fits" );
    myfits.create(outname.c_str());
    write_Healpix_map_to_fits(myfits, mask, PLANCK_FLOAT64);
    myfits.close();


  return 0;

}