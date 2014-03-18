#include <stdio.h>

#include <iostream>
using std::cout;
using std::flush;
using std::cerr;
using std::endl;

#include <string>
using std::string;
#include <sstream>
using std::stringstream;
#include <fstream>
using std::ifstream;
using std::ofstream;

#include <math.h>

#include "../yale_pipeline/cppsrc/cc_fitsio.hh"
using namespace ns_cc_fitsio;

#include <healpix_base.h>
#include <healpix_base2.h>
#include <healpix_map.h>
#include <partpix_map.h>
#include <partpix_map2.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>

int main (int argc, char **argv){

    if( argc != 4 ){
        cerr << "Usage: fits_to_partpix fits_filename healpix_order partpix_output_filename" << endl;
        return 1;
    }

  string filename( argv[1] );
  int order = atoi( argv[2] );
  string outfilename( argv[3] );
  cerr << "Converting " << filename << " to a partpix text file " << outfilename << " of order " << order << endl;

  fitsfile * fptr;
  int status = 0;
  if ( fits_open_file( &fptr, filename.c_str(), READONLY, &status ) ) {
    fits_report_error( stderr, status );
    cerr << "Could not open fits file " << filename << endl;
    return 1;
  }
  
  int n1 = 0;
  int n2 = 0;
  char comment[256];
  fits_read_key( fptr, TINT, "NAXIS1", &n1, comment, &status );
  fits_read_key( fptr, TINT, "NAXIS2", &n2, comment, &status );
  cerr << "Image dimensions (" << n1 << ", " << n2 << ")" << endl;
  
  float crval1;
  fits_read_key( fptr, TFLOAT, "CRVAL1", &crval1, comment, &status );
  float crval2;
  fits_read_key( fptr, TFLOAT, "CRVAL2", &crval2, comment, &status );
  float crpix1;
  fits_read_key( fptr, TFLOAT, "CRPIX1", &crpix1, comment, &status );
  float crpix2;
  fits_read_key( fptr, TFLOAT, "CRPIX2", &crpix2, comment, &status );
  float cd1_1;
  fits_read_key( fptr, TFLOAT, "CD1_1", &cd1_1, comment, &status );
  float cd1_2;
  fits_read_key( fptr, TFLOAT, "CD1_2", &cd1_2, comment, &status );
  float cd2_1;
  fits_read_key( fptr, TFLOAT, "CD2_1", &cd2_1, comment, &status );
  float cd2_2;
  fits_read_key( fptr, TFLOAT, "CD2_2", &cd2_2, comment, &status );
  cerr << "WCS Parameters: " << crval1 << " " << crval2 << " " << crpix1 << " " << crpix2 << " " << cd1_1 << " " << cd1_2 << " " << cd2_1 << " " << cd2_2 << endl;

  // read in fits file to an array
  float nulval = 0;
  int anynul;
  int npix = n1*n2;
  int* array_mask = new int[npix];
  if( fits_read_img( fptr, TINT, 1, npix, &nulval, array_mask, &anynul, &status ) ) { 
    fits_report_error( stderr, status );
    cerr << "Could not read fits file " << filename << endl;
    return 1;
  }
  
  vector<pointing> pointings;
  for( unsigned int x=0; x<n1; ++x ){
      for( unsigned int y=0; y<n2; ++y ){
          unsigned int index = y*n1+x; 
          if( array_mask[index] > 2 )
              continue;
              
          double x1 = cd1_1*(x-crpix1)+cd1_2*(y-crpix2);
          double y1 = cd2_1*(x-crpix1)+cd2_2*(y-crpix2);

          // WCS de-projection transformation to equatorial coordinates (in radians)
          double ra_0 = crval1*3.1415926/180.;
          double dec_0 = crval2*3.1415926/180.;

          double x2 = x1*3.1415926/180.; // ra
          double y2 = y1 *3.1415926/180.; // dec

          double ra = ra_0 + atan(x2/(cos(dec_0)-(y2*sin(dec_0))));
          double dec = asin( (sin(dec_0)+y2*cos(dec_0))/(sqrt(1+(x2*x2)+(y2*y2))) );

          double phi = ra;
          double theta = 3.1415926/2.0 - dec;
          pointing mypointing(theta, phi);
          pointings.push_back( mypointing );
          
      }
  }

    delete( array_mask );

    cerr << pointings.size() << " good pixels in the fits mask" << endl;
  
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

    // system( "rm fp.fits" );
    // fitshandle myfits;
    // myfits.create("fp.fits");
    // write_Healpix_map_to_fits(myfits, footprintMap, PLANCK_FLOAT64);
    // myfits.close();
  
    // now make the partpix mask
    Partpix_Map2<int> mask(order, footprintMap);
    cerr << "The mask has " << mask.Npartpix() << " partpixels" << endl;

    mask.fill(0);
    for( unsigned int i=0; i<pointings.size(); ++i ){
        mask[mask.ang2pix(pointings[i])] = mask[mask.ang2pix(pointings[i])]+1;
    }
    
    cerr << "Filled up the mask" << endl;

    // TBD: what should i use as my cutoff value rather than 0?
    // what should i use as my cutoff value rather than 0?
    int max_mask = 0;
    for( int i1=0; i1<mask.Npartpix(); ++i1 ){
        int i=mask.highResPix(i1);
        if( mask[i] > max_mask )
            max_mask = mask[i];
    }
    int cutoff = int(max_mask/2.0 + 0.5);
    cerr << "Using a cutoff of " << cutoff << " good mask pixels per healpix pixel" << endl;
    
    // what is the histogram of the mask values?
    ofstream outfile( outfilename.c_str() );
    outfile << "RING " << order << endl;
    for( unsigned int i1=0; i1<mask.Npartpix(); ++i1 ){
        unsigned int i = mask.highResPix(i1);
        pointing mypointing = mask.pix2ang(i);
        int val = mask[i];
        if( val > cutoff ){
            //double ra = mypointing.phi*180./3.1415926;
            //double dec = 90. - mypointing.theta*180./3.1415926;
            outfile << i << endl;
            //cout << ra << " " << dec << " " << val << endl;
        }
    }
  
  
  return 0;
}