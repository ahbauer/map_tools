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

    if( argc != 2 ){
    cerr << "Usage: h5_to_partpix hdf5file" << endl;
    return 1;
    }

    string mapname(argv[1]);

    //cout << setiosflags(ios::fixed) << setprecision(9);

    if( access( mapname.c_str(), F_OK ) == -1 ){
    cerr << "File " << mapname << " is not readable!" << endl;
    return 1;
    }

      H5File file1( mapname, H5F_ACC_RDONLY );
      H5G_obj_t firstType = file1.getObjTypeByIdx(0);
      if( firstType != H5G_GROUP ){
          cerr << "The map is not standard format;  the first element is not a group." << endl;
          throw exception();
      }
      string group_name = file1.getObjnameByIdx(0);
      if( group_name != "data" ){
          cerr << "Group name is " << group_name << ", not data" << endl;
          throw exception();
      }
      Group group = file1.openGroup("data");
      DataSet dataset1 = group.openDataSet("pixels");

      hsize_t ds1size = dataset1.getStorageSize();
      unsigned long npix = ds1size/sizeof(unsigned long);

      unsigned long *map_pix;
      map_pix = new unsigned long[ds1size];

      cerr << "reading map_pix... " << endl;
      dataset1.read(map_pix, PredType::NATIVE_ULONG);
      cerr << " done!" << endl;

      DataSet dataset1a = group.openDataSet("values");

      ds1size = dataset1a.getStorageSize();
      if( npix != ds1size/sizeof(float) ){
          cerr << "Problem, number of pixels " << npix << " != number of data points " << ds1size/sizeof(float) << endl;
          throw exception();
      }
      
      float *map_data;
      map_data = new float[ds1size];
      cerr << "reading map_data...";
      dataset1a.read(map_data, PredType::NATIVE_FLOAT);
      cerr << " done!" << endl;

      int map_order = map_pix[0];
      int map_scheme = map_data[0];
      cerr << "map has order " << map_order << " and scheme " << map_scheme << endl;

      /*
      DataSet dataset2 = group.openDataSet("mask_pixels");
      hsize_t ds2size = dataset2.getStorageSize();
      unsigned long npix2 = ds2size/sizeof(unsigned long);
      cerr << "mask 1 has " << npix2-1 << " pixels" << endl;

      unsigned long *mask_pix = new unsigned long[ds2size];
      dataset2.read(mask_pix, PredType::NATIVE_ULONG);

      DataSet dataset2a = group.openDataSet("mask_values");
      ds2size = dataset2.getStorageSize();
      if( npix2 != ds2size/sizeof(unsigned long) ){
          cerr << "Problem, mask has " << npix2 << " pixels but " << ds2size/sizeof(unsigned long) << " values" << endl;
          throw exception();
      }

      unsigned long *mask_data = new unsigned long[ds2size];
      dataset2a.read(mask_data, PredType::NATIVE_ULONG);

      if( mask_data[0] != RING ){
          cerr << "Problem, mask ordering is not RING but " << mask_data[0] << endl;
          throw exception();
      }
      int mask_order = mask_pix[0];
      cerr << "mask info " << mask_order << " " << mask_data[0] << endl;
      vector<unsigned long> mask_pixel_list;
      for( unsigned long i=1; i<npix2; ++i ){
          if( mask_data[i] > 0.5 ){
              mask_pixel_list.push_back(mask_pix[i]);
          }
      }

      delete[] mask_pix;
      delete[] mask_data;
      */
      
      cerr << "Finished reading in the map" << endl;
      
      
    if( map_scheme == 0 ){
        cout << "RING ";
    }
    else if( map_scheme == 1 ){
        cout << "NEST ";
    }
    else{
        cerr << "map ordering is not 0 or 1!" << endl;
        return 1;
    }
    cout << map_order << endl;;
    
    for( unsigned int i=1; i<ds1size; ++i ){
        cout << map_pix[i] << " " << map_data[i] << endl;
    }
    
  return 0;

}
