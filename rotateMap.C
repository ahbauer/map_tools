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
#include <dirent.h>
#include <ctime>
#include <omp.h>
#include <healpix_base.h>
#include <healpix_map.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>

int main (int argc, char **argv){
    
    if( argc != 3 ){
    cerr << "Usage: rotate_map filename outfile" << endl;
    return 1;
    }

    string infilename(argv[1]);
    string outfilename(argv[2]);

    double rotation_angle = 3.1415926/18.0;

    Healpix_Map<double> map;
    read_Healpix_map_from_fits( infilename, map );
    Healpix_Map<double> outmap = Healpix_Map<double>(map.Order(), RING);

    for( int i=0; i<map.Npix(); ++i ){
        
        pointing oldpointing = map.pix2ang(i);
        
        double newphi = rotation_angle + oldpointing.phi;
        if( newphi > 6.2831852 ){
            newphi -= 6.2831852;
        }

        pointing newpointing( oldpointing.theta, newphi );
        int pixnum = outmap.ang2pix( newpointing );
        outmap[pixnum] = map[i];
    }

    fitshandle myfits = fitshandle();
    myfits.create(outfilename.c_str());
    write_Healpix_map_to_fits(myfits, outmap, PLANCK_FLOAT64);
    myfits.close();

    return 0;

}
    