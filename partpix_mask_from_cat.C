#include <unistd.h>
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
#include <healpix_base2.h>
#include <healpix_map.h>
#include <partpix_map.h>
#include <partpix_map2.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>
#include "useful.hh"
using namespace ns_useful;
#include "exception_extension.hh"
using namespace ns_exception_extension;

int main (int argc, char **argv){
    
    string filename = argv[1];
    
    vector<pointing> pointings;
    FILE * file = fopen( filename.c_str(), "r" );
    if( file == NULL )
        throw my_exception( string() + "Problem opening " + filename );
    char line[256];
    while( fgets( line, 256, file ) ){
        double ra, dec;
        int num = sscanf(line, "%lf %lf", &ra, &dec );
        if( num != 2 ){
            throw my_exception( string() + "Problem reading from file, number of arguments read = " + ns_useful::string_ize(num) + ", not 2" );
        }
        double phi = ra*3.1415926/180.;
        double theta = (90.-dec)*3.1415926/180.;
        pointing mypointing(theta, phi);
        pointings.push_back( mypointing );
    }
    cerr << "Read in " << pointings.size() << " objects" << endl;
    
    // from the masks, determine the best possible healpix footprint
    // requirements: must cover all of the mask area
    // must cover <80% of the area covered by the next lowest resolution
    // must be order <= 6 ?
    int footprint_order = 6;
    double footprint_area = 50000.;
    Healpix_Map<int> footprintMap;
    while(footprint_order <= 6){
        footprintMap = Healpix_Map<int>(footprint_order, RING);
        footprintMap.fill(0);
        for( unsigned int i=0; i<pointings.size(); ++i ){
            footprintMap[footprintMap.ang2pix(pointings[i])] = 1;
        }
        double footprint_area_new = 0.;
        for( int i=0; i<footprintMap.Npix(); ++i ){
          if( footprintMap[i] == 1 )
              footprint_area_new += 1.0;
        }
        footprint_area_new *= (41253./footprintMap.Npix());
        if( footprint_area_new <= 0.8*footprint_area ){
            footprint_area = footprint_area_new;
            cerr << "Footprint order " << footprint_order << " uses area " << footprint_area << " sq degrees... trying the next order." << endl;
            ++footprint_order;
        }
        else{
            footprint_area = footprint_area_new;
            break;
        }
    }
    cerr << "Using footprint order " << footprintMap.Order() << " with " << footprint_area << " sq degrees." << endl;

    system( "rm fp.fits" );
    fitshandle myfits;
    myfits.create("fp.fits");
    write_Healpix_map_to_fits(myfits, footprintMap, PLANCK_FLOAT64);
    myfits.close();

    // now make the partpix mask
    int order = 7;
    Partpix_Map2<int> mask(order, footprintMap);
    cerr << "The mask has " << mask.Npartpix() << " partpixels" << endl;

    mask.fill(0);
    for( unsigned int i=0; i<pointings.size(); ++i ){
        mask[mask.ang2pix(pointings[i])] = mask[mask.ang2pix(pointings[i])]+1;
    }
    
    cerr << "Filled up the mask" << endl;

    // what is the histogram of the mask values?
    cout << "RING " << order << endl;
    for( unsigned int i1=0; i1<mask.Npartpix(); ++i1 ){
        unsigned int i = mask.highResPix(i1);
        pointing mypointing = mask.pix2ang(i);
        int val = mask[i];
        if( val > 0 ){
            //double ra = mypointing.phi*180./3.1415926;
            //double dec = 90. - mypointing.theta*180./3.1415926;
            cout << i << endl;
            //cout << ra << " " << dec << " " << val << endl;
        }
    }

    return 0;
    
}
