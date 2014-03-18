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
#include <healpix_base.h>

bool good_pixel( double ra, double dec, double val ){
    
    double mag_cut = 23.6;
    
    // if( (ra<72. || dec>-60.) && val>mag_cut )
    //     return true;
    
    if( ra>0. && ra<20. && dec>0. && dec<20. )
        return true;
        
    return false;
}


int main (int argc, char **argv){
    
    if( argc != 2 ){
        cerr << "Usage: partpix_cut partpix_filename" << endl;
        cerr << "But must manually change the cuts in the code!" << endl;
        return 0;
    }

    

    string filename = argv[1];
    

    // read in the mask
    FILE * maskfile = fopen( filename.c_str(), "r" );
    if( maskfile == NULL ){
        cerr << "Problem opening " << filename << endl;
        return 1;
    }
    int mask_order;
    Healpix_Base mask;
    bool header = true;
    char line_char[256];
    string scheme;
    while( fgets( line_char, 256, maskfile ) ){
        if( header ){
            char sch[16];
            int num = sscanf(line_char, "%s %d", sch, &mask_order );
            if( num != 2 ){
                cerr << "Problem reading header from mask file, number of arguments read = " << num << ", not 2" << endl;
                return 1;
            }
            scheme = string(sch);
            if( (!scheme.compare("RING")) && (!scheme.compare("NEST")) ){
                cerr << "Scheme listed in mask header line is not RING or NEST, but " << scheme << endl;
                return 1;
            }
            Healpix_Ordering_Scheme scheme_enum = RING;
            if( !scheme.compare("NEXT") ){
                cerr << "NEST!" << endl;
                scheme_enum = NEST;
            }
            mask = Healpix_Base( mask_order, scheme_enum );
            header = false;
            cout << sch << " " << mask_order << endl;
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
        
        // calculate ra & dec from pixel
        pointing mypointing = mask.pix2ang(pix);
        double ra = mypointing.phi*180.0/3.1415926;
        double dec = 90. - mypointing.theta*180./3.1415926;
        
        if( good_pixel( ra, dec, val ) ){
            cout << pix << " " << val << endl;
        }
        
    } // while
    
    return 0;
    
} // main
