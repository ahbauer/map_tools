#include <unistd.h>
#include <vector>
using std::vector;
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

#include <partpix_map.h>


int main (int argc, char const *argv[]){

    Healpix_Map<double> oldmap(7, RING);
    oldmap.fill(0.);
    for( int i=0; i<19661; ++i ){
        oldmap[i]=1.0;
    }
    cout << "my Healpix map has order " << oldmap.Order() << "!" << endl;
    // cerr << "sleeping!" << endl;
    // sleep(5);

    system( "rm inmap.fits" );
    fitshandle myfits = fitshandle();
    myfits.create("inmap.fits");
    write_Healpix_map_to_fits(myfits, oldmap, PLANCK_FLOAT64);
    myfits.close();
    
    cout << "done writing old map" << endl;

    Partpix_Map<double> newmap(9, oldmap);
    cout << "my Partpix map has order " << newmap.Order() << "!!!" << endl;
    // now fill up the new map with data, using the old one as the mask (NECESSARY).
    double val = 1.0;
    for( int i=0; i<newmap.Npartpix(); ++i ){
        int j = newmap.highResPix(i);
        newmap[j] = val;
        val += 0.1;
    }
    
    cout << "done assigning to the new map" << endl;

    
    // for( int i=0; i<newmap.Npix(); ++i ){
    //     pointing mypointing = newmap.pix2ang(i);
    //     int pixnum = oldmap.ang2pix(mypointing);
    //     if( oldmap[pixnum] > 0.5 ){
    //         newmap[i] = val;
    //         val += 0.1;
    //     }
    // }
    // cerr << "sleeping again!" << endl;    
    // sleep(5);

    // now let's loop over the partpix pixels...
    // cout << "The map values:" << endl;
    // for( int i=0; i<newmap.Npartpix(); ++i ){
    //     int j=newmap.highResPix(i);
    //     double val = newmap[j];
    //     cout << val << " ";
    // }
    // cout << endl;

    Healpix_Map<double> outmap = newmap.to_Healpix( 0. );
    cout << "Done converting new map to healpix" << endl;

    system( "rm outmap.fits" );
    myfits = fitshandle();
    myfits.create("outmap.fits");
    write_Healpix_map_to_fits(myfits, outmap, PLANCK_FLOAT64);
    myfits.close();
    
    cout << "Done writing new map" << endl;
    
    Partpix_Map<double> importmap(7, oldmap);
    cout << "Done making low res map" << endl;

    importmap.Import_degrade(newmap, oldmap);

    cout << "done with downgrade" << endl;

    Healpix_Map<double> importhmap = importmap.to_Healpix( 0. );

    cout << "converted to healpix" << endl;
    
    
    system( "rm importmap.fits" );
    myfits = fitshandle();
    myfits.create("importmap.fits");
    write_Healpix_map_to_fits(myfits, importhmap, PLANCK_FLOAT64);
    myfits.close();
    
    return 0;
}