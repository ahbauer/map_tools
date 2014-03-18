#include <stdio.h>

#include <iostream>
using std::cout;
using std::flush;
using std::cerr;
using std::endl;

#include <string>
using std::string;
#include <fstream>
using std::ifstream;
using std::ofstream;
#include <sstream>
using std::stringstream;

#include <healpix_base.h>
#include <healpix_base2.h>
#include <healpix_map.h>
#include <partpix_map.h>
#include <partpix_map2.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>

#include "H5Cpp.h"
using namespace H5;

int main (int argc, char **argv){
    
    string infilename( argv[1] );
    string line;
    ifstream infile( infilename.c_str() );
    getline( infile, line );
    stringstream linestream(line);
    int ordering;
    int order;
    linestream >> ordering >> order;
    cout << "Ordering " << ordering << ", order " << order << endl;
    vector<long int> ps;
    vector<double> vs;
    getline( infile, line );
    while( ! infile.eof() ){
        stringstream linestream(line);
        long int p;
        double v = 1.;
        linestream >> p >> v;
        ps.push_back(p);
        vs.push_back(v);
        getline( infile, line );
    }
    
    // write out the maps (hdf5)
    string outfilename = "map.h5";
    H5::H5File *file = new H5::H5File( outfilename, H5F_ACC_TRUNC ); // clobber!
    H5::Group* group = new H5::Group( file->createGroup( "/data" ));
    H5::PredType datatype( H5::PredType::NATIVE_DOUBLE );
    hsize_t dimsf[2];
    dimsf[1] = 2;
    int rank = 2;
    
    // the map dataset
    double *data = (double*) malloc( sizeof(double) * 2*ps.size()+2 );
    data[0] = order;
    data[1] = ordering;
    unsigned int index = 2;
    for( unsigned int i=0; i<ps.size(); ++i ){
        data[index] = ps[i];
        ++index;
        data[index] = vs[i];
        ++index;
    }
    dimsf[0] = ps.size()+1;
    H5::DataSpace *dataspace = new H5::DataSpace( rank, dimsf );
    string datasetname = "/data/map";
    H5::DataSet *dataset = new H5::DataSet( file->createDataSet( datasetname, datatype, *dataspace ) );
    dataset->write( data, H5::PredType::NATIVE_DOUBLE );
    delete dataspace;
    delete dataset;
    free( data );
    
    return 0;
    
}
