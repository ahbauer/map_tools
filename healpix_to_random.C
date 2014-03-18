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
#include <healpix_base.h>
#include <healpix_map.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>

#include <gsl/gsl_rng.h>

int main (int argc, char **argv){
    
    if( argc != 3 ){
        cerr << "Usage: healpix_to_random healpix_filename n_random_points" << endl;
        return 1;
    }
    
    string filename(argv[1]);
    unsigned int n_points = atoi(argv[2]);
    
    double cutoff = 0.5;
    
    cout << setiosflags(ios::fixed) << setprecision(9);
    
    Healpix_Map<double> mask;
    read_Healpix_map_from_fits( filename.c_str(), mask );
    
    
    const gsl_rng_type * T;
    gsl_rng * r;
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc (T);
    gsl_rng_set (r, rand()); // seed it with a pseudo-random number
    
    unsigned int count = 0;
    while( count < n_points ){
        
        // pick a random RA & dec (evenly spherically distributed!)
        double uu = gsl_rng_uniform (r);
        double vv = gsl_rng_uniform (r);
        double ra = uu*360.0;
        double dec = 90.0 - acos(2.0*vv-1.0)*180.0/3.1415926;
        
        double phi = uu*6.2831852;
        double theta =acos(2.0*vv-1.0);
        
        pointing mypointing(theta, phi);
        if( mask[mask.ang2pix(mypointing)] >= cutoff ){
            cout << ra << " " << dec << endl;
            ++count;
        }
        
    }
    
    return 0;
    
}