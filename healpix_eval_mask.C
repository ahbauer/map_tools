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

    if( argc == 1 ){
        cerr << "Usage: healpix_eval_mask mask catalog" << endl;
        cerr << "Prints out the mag limit (where error = 0.1) vs the mask value" << endl;
        cerr << "Or, prints out the number of objects versus mag cut" << endl;
        cerr << "The catalog format should be ra dec mag mag_error z" << endl;
        return 1;
    }

    bool limit_comparison = true;
    bool area_limit_comparison = false;
    bool cut_efficiency = false;

    //double offset = 0.82; // auto
    double offset = 0.69; // model
    // double offset = 0.23; // detmodel

    double z_min = 0.95;
    double z_max = 1.15;

    string maskname = argv[1];
    string listname = argv[2];
    cerr << "running " << listname << " past the mask " << maskname << endl;


    Healpix_Map<double> mask;
    read_Healpix_map_from_fits( maskname.c_str(), mask );

    // if( mask.Scheme() == RING ){
    //     cout << "ring!" << endl;
    // }
    // else{
    //     cout << "nest!" << endl;
    // }

    int offset_map_order = 1;
    if( area_limit_comparison ){
        offset_map_order = 7;
    }
    Healpix_Map<double> offset_map(offset_map_order, mask.Scheme());
    Healpix_Map<double> offset_rms_map(offset_map_order, mask.Scheme());
    offset_map.fill(0.);
    offset_rms_map.fill(0.);

    vector< vector<int> > data( 50, vector<int>(50, 0) );
    vector< vector<double> > sums( offset_map.Npix()+1, vector<double>( 50, 0.) );
    vector< vector<int> > ns( offset_map.Npix()+1, vector<int>( 50, 0 ) );
    vector< vector< vector<double> > > vals( offset_map.Npix()+1, vector< vector<double> >(50, vector<double>() ) );

    vector<int> counts(50, 0);

    ifstream listfile( listname.c_str() );

    while( ! listfile.eof() ){
        char line[2048];
        listfile.getline(line, 2048);
        double ra, dec, mag, emag, z;
        stringstream linestream(line);
        linestream >> ra >> dec >> mag >> emag >> z;

        if( z < z_min || z > z_max ){
            continue;
        }
        
        int magbin = int(10.*(mag-20.));
        // cout << magbin << endl;
        if( magbin < 0 || magbin > 49 )
            continue;
        double phi = ra*3.1415926/180.;
        double theta = (90.-dec)*3.1415926/180.;
        pointing mypointing(theta, phi);
        int pixnum = mask.ang2pix( mypointing );
        double maskval = mask[pixnum];
        // cout << "maskval " << maskval << endl;
        if( maskval < 0. )
            continue;
        int maskbin = int(10.*(maskval-21.));
        if( maskbin < 0 || maskbin > 49 )
            continue;
            
        // cout << "bin " << maskbin << endl;
        
        for( double maskmagcut=21.; maskmagcut<26.; maskmagcut+=0.1 ){
            int maskmagcutbin = int(10.*(maskmagcut-21.0));
            double catmagcut = maskmagcut - offset;
            // cout << magcut << " " << magcutbin << " " << maskval << " " << counts[magcutbin] << endl;
            if( maskval > maskmagcut && mag < catmagcut ){
                counts[maskmagcutbin]++;
            }
        }
        
        int offsetmappix = offset_map.ang2pix( mypointing );
        if( emag < 0.11 && emag > 0.09 ){
            sums[offsetmappix][maskbin] += mag;
            ns[offsetmappix][maskbin]++;
            vals[offsetmappix][maskbin].push_back(mag);
            sums[offset_map.Npix()][maskbin] += mag;
            ns[offset_map.Npix()][maskbin]++;
            vals[offset_map.Npix()][maskbin].push_back(mag);
        }
        if( emag > 0.10 ){
            continue;
        }
        //cout << maskval << " " << maskbin << " " << pixnum << endl;
        data[maskbin][magbin]++;
        //cout << "added one to " << maskbin << " " << magbin << endl;
  }
  
  if( cut_efficiency ){
      for( double maskmagcut=21; maskmagcut<26; maskmagcut+=0.1 ){
          int magcutbin = int(10.*(maskmagcut-21.0));
          double catmagcut = maskmagcut - offset;
          cout << maskmagcut << " " << catmagcut << " " << counts[magcutbin] << endl;
      }
  }
  
  if( limit_comparison ){
      for( unsigned int mask=0; mask<50; ++mask ){

          // what's the mode of this histogram?
          // call that the mag limit.
          int max = 0;
          int modebin = 0;
          for( unsigned int mag=0; mag<50; ++mag ){
              if( data[mask][mag] > max ){
                  max = data[mask][mag];
                  modebin = mag;
              }
          }
  
          double catmag_median = 0.;
          if( vals[offset_map.Npix()][mask].size() > 3 ){
              vector<double> valscopy(vals[offset_map.Npix()][mask]);
              // cerr << "sorting valscopy with length " << valscopy.size() << endl;
              sort( valscopy.begin(), valscopy.end() );
              int index = int(valscopy.size()/2);
              // cerr << "trying " << index << " from length " << valscopy.size() << endl;
              catmag_median = valscopy[index];
              // cerr << catmag_median << endl;
          }
  
          double catmag_mean = 0.;
          if( ns[offset_map.Npix()][mask] > 0 ){
              catmag_mean = sums[offset_map.Npix()][mask]/ns[offset_map.Npix()][mask];
          }
          double maskmag = mask/10.0 + 21.05;
          double catmag = modebin/10.0 + 20.05;
          cout << catmag << " " << catmag_mean << " " << catmag_median << " " << maskmag << endl;
      }
  }
  
  if( area_limit_comparison ){
      
      bool jimmy_fwhm = true;
      double A = 43.095; // area of 2" diam aperture, in pixels.
      
      Healpix_Map<double> fwhm_map;
      read_Healpix_map_from_fits( "/Users/bauer/surveys/DES/sva1/systematics/maps/SVA1_COADD_GRIZY_i_FWHM_median.fits", fwhm_map );
      int n=0;
      double fwhm_mean = 0.;
      for( int i=0; i<fwhm_map.Npix(); ++i ){
          if( fwhm_map[i]>0. && fwhm_map[i]<10. ){
              fwhm_mean += fwhm_map[i];
              ++n;
          }
      }
      fwhm_mean /= n;
      
      
      // do the above, but for each pixel and save just the offsets between the two mag limits
      for( int opix=0; opix<offset_map.Npix(); ++opix ){
          
          double offset = 0.;
          double offset_rms = 0.;
          unsigned int n = 0;
          for( unsigned int mask=0; mask<50; ++mask ){
          
              // // what's the mode of this histogram?
              // // call that the mag limit.
              // int max = 0;
              // int modebin = 0;
              // for( unsigned int mag=0; mag<50; ++mag ){
              //     if( data[mask][mag] > max ){
              //         max = data[mask][mag];
              //         modebin = mag;
              //     }
              // }

              double catmag_median = 0.;
              // if( vals[opix][mask].size() > 0 )
              //       cout << opix << " " << vals[opix][mask].size() << endl;
              if( vals[opix][mask].size() > 5 ){
                  vector<double> valscopy(vals[opix][mask]);
                  // cerr << "sorting valscopy with length " << valscopy.size() << endl;
                  sort( valscopy.begin(), valscopy.end() );
                  int index = int(valscopy.size()/2);
                  // cerr << "trying " << index << " from length " << valscopy.size() << endl;
                  catmag_median = valscopy[index];
                  // cerr << catmag_median << endl;
              }

              double catmag_mean = 0.;
              if( ns[opix][mask] > 0 ){
                  catmag_mean = sums[opix][mask]/ns[opix][mask];
              }
              double maskmag = mask/10.0 + 21.05;
              // double catmag = modebin/10.0 + 20.05;
              if( catmag_median > 0. ){
                  // cout << maskmag << " " << catmag_median << " " << catmag_mean << endl;
                  if( jimmy_fwhm ){
                      // cout << "mask mag " << maskmag << " -> ";
                      double denom = pow(10., (30.-maskmag)/2.5);
                      double weight = 100.*A/(denom*denom);
                      int fwhm_pix = fwhm_map.ang2pix(offset_map.pix2ang(opix));
                      if( fwhm_map[fwhm_pix] > 0. || fwhm_map[fwhm_pix] < 10. ){
                          double delta_fwhm = fwhm_map[fwhm_pix]/fwhm_mean;
                          weight /= (delta_fwhm*delta_fwhm);
                          denom = sqrt(100.*A/weight);
                          maskmag = 30.0 - 2.5*log10(denom);
                      }
                      // cout << maskmag << endl;
                  }
                  // convert the mag to a weight
                  offset += (maskmag-catmag_median);
                  offset_rms += (maskmag-catmag_median)*(maskmag-catmag_median);
                  n++;
              }
          } // for mask pixel bin
      
          if( n>0 ){
              offset /= n;
              offset_rms /= n;
              offset_rms = sqrt(offset_rms - offset*offset);
          }
          offset_map[opix] = offset;
          offset_rms_map[opix] = offset_rms;
          
      } // for offset map pixel
      
      fitshandle myfits = fitshandle();
      system( "rm offset.fits" );
      myfits.create("offset.fits");
      write_Healpix_map_to_fits(myfits, offset_map, PLANCK_FLOAT64);
      myfits.close();
      myfits = fitshandle();
      system( "rm offset_rms.fits" );
      myfits.create("offset_rms.fits");
      write_Healpix_map_to_fits(myfits, offset_rms_map, PLANCK_FLOAT64);
      myfits.close();
  }

  return 0;

}