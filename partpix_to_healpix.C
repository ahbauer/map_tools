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

    // read in the mask
    FILE * spec_maskfile = fopen( filename.c_str(), "r" );
    if( spec_maskfile == NULL ){
        cerr << "Problem opening " << filename << endl;
        return 1;
    }
    int spec_mask_order;
    vector<int> spec_mask_pixels;
    vector<double> vals;
    bool header = true;
    char line_char[256];
    string scheme;
    while( fgets( line_char, 256, spec_maskfile ) ){
        if( header ){
            char sch[16];
            int num = sscanf(line_char, "%s %d", sch, &spec_mask_order );
            if( num != 2 ){
                cerr << "Problem reading header from mask file, number of arguments read = " << num << ", not 2" << endl;
                return 1;
            }
            scheme = string(sch);
            if( (!scheme.compare("RING")) && (!scheme.compare("NEST")) ){
                cerr << "Scheme listed in mask header line is not RING or NEST, but " << scheme << endl;
                return 1;
            }
            header = false;
            continue;
        }
        int pix;
        double val;
        int num = sscanf(line_char, "%d %lf", &pix, &val );
        if( num != 2 ){
            // allow the implicit assumption of a pixel's existence in the list meaning that the mask is good there.
            val = 1.0;
            num = sscanf(line_char, "%d", &pix );
            if( num != 1 ){
                cerr << "Problem reading from mask file, number of arguments read = " << num << ", not 1" << endl;
                return 1;
            }
        }
        spec_mask_pixels.push_back(pix);
        vals.push_back(val);
    }
    cerr << "Read in mask information, scheme " << scheme << ", order " << spec_mask_order << ", " << spec_mask_pixels.size() << " pixels" << endl;

    
    Healpix_Map<double> outMap;
    bool nest = false;
    if( scheme.compare("NEST") == 0 )
      nest = true;

    if( spec_mask_order < 14 ){

      if( scheme.compare("NEST") == 0 )
        outMap = Healpix_Map<double>( spec_mask_order, NEST );
      else
        outMap = Healpix_Map<double>( spec_mask_order, RING );
        
    
      for( unsigned int i=0; i<spec_mask_pixels.size(); ++i ){
        outMap[spec_mask_pixels[i]] = vals[i];
      }
    
      system( "rm outmap.fits" );
      fitshandle myfits;
      myfits.create("outmap.fits");
      write_Healpix_map_to_fits(myfits, outMap, PLANCK_FLOAT64);
      myfits.close();
      
    }
    else{

      cerr << "Dealing with the resolution > 13" << endl;
      Healpix_Base2 bigbase;
      if( nest )
	bigbase = Healpix_Base2( spec_mask_order, NEST );
      else
	bigbase = Healpix_Base2( spec_mask_order, RING );

      Healpix_Map<int> footprintMap;
      if( nest )
	footprintMap = Healpix_Map<int>( 5, NEST );
      else
	footprintMap = Healpix_Map<int>( 5, RING );
      footprintMap.fill(0);
      for( unsigned int i=0; i<spec_mask_pixels.size(); ++i ){
	pointing mypointing = bigbase.pix2ang(spec_mask_pixels[i]);
	int fppix = footprintMap.ang2pix(mypointing);
	footprintMap[fppix] = 1;
      }
      cerr << "Done with footprint map" << endl;

      Partpix_Map2<double> midMap( spec_mask_order, footprintMap );

      for( unsigned int i=0; i<spec_mask_pixels.size(); ++i ){
        midMap[spec_mask_pixels[i]] = vals[i];
      }

      cerr << "Downgrading from " << spec_mask_order << " to 13" << endl;
      Partpix_Map2<double> midMap2(13, footprintMap);
      midMap2.Import_degrade(midMap, footprintMap);
      outMap = midMap2.to_Healpix();

      system( "rm outmap.fits" );
      fitshandle myfits;
      myfits.create("outmap.fits");
      write_Healpix_map_to_fits(myfits, outMap, PLANCK_FLOAT64);
      myfits.close();
    }

    return 0;
        
}
    
