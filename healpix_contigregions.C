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
#include <healpix_map.h>
#include <fitsio.h>
#include <fitshandle.h>
#include <healpix_map_fitsio.h>
#include <healpix_data_io.h>
#include "useful.hh"
using ns_useful::string_ize;

int main (int argc, char **argv){

  int pix_per_region = 2;

  Healpix_Map<double> Map = Healpix_Map<double>(3, RING);
  vector<bool> used( Map.Npix(), false );

  vector<int> neighbor_indices;
  if( pix_per_region == 2 ){
    neighbor_indices.push_back(0);
    neighbor_indices.push_back(2);
    neighbor_indices.push_back(4);
    neighbor_indices.push_back(6);
  }
  else if( pix_per_region == 4 ){
    neighbor_indices.push_back(0);
    neighbor_indices.push_back(1);
    neighbor_indices.push_back(2);
    //neighbor_indices.push_back(3);
    //neighbor_indices.push_back(4);
    //neighbor_indices.push_back(5);
    //neighbor_indices.push_back(6);
    //neighbor_indices.push_back(7);
  }

  int regnum = 1;
  for( int i=0; i<Map.Npix(); ++i ){
    if( ! used[i] ){
      fix_arr<int,8> neighbors;
      Map.neighbors(i, neighbors );

      if( pix_per_region == 2 ){
	bool found = false;
	Map[i] = regnum;
	used[i] = true;
	for( unsigned int j=0; j<neighbor_indices.size(); ++j ){
	  if( neighbors[neighbor_indices[j]] < 0 ){
	    cerr << "neighbor index = " << neighbors[neighbor_indices[j]] << " for neighbor num " << neighbor_indices[j] << "!" << endl;
	    continue;
	  }
	  else if( ! used[neighbors[neighbor_indices[j]]] ){
	    Map[neighbors[neighbor_indices[j]]] = regnum;
	    used[neighbors[neighbor_indices[j]]] = true;
	    found = true;
	    break;
	  }
	}
	if( ! found )
	  cerr << "problem finding a good neighbor" << endl;
	regnum++;
      }
      else if( pix_per_region == 4 ){
	bool good = true;
	for( unsigned int j=0; j<neighbor_indices.size(); ++j ){
	  if( neighbors[neighbor_indices[j]] < 0 )
	    good = false;
	  else if( used[neighbors[neighbor_indices[j]]] )
	    good = false;
	}
	if( good ){
	  Map[i] = regnum;
	  used[i] = true;
	  for( unsigned int j=0; j<neighbor_indices.size(); ++j ){
	    Map[neighbors[neighbor_indices[j]]] = regnum;
	    used[neighbors[neighbor_indices[j]]] = true;
	  }
	  regnum++;
	}
      }
    } // if the pixel is not used
  } // for the pixels in the map

  cerr << "used " << regnum-1 << " regions" << endl;

  Healpix_Map<double> mask = Healpix_Map<double>(3, RING);
  for( int i=1; i<regnum; ++i ){
    mask.fill(0.);
    for( int j=0; j<Map.Npix(); ++j ){
      if( Map[j] == i )
	mask[j] = 1.0;
    }
    string filename = "mask" + string_ize(i) + ".fits";
    string command = "rm " + filename;
    system( command.c_str() );
    fitshandle myfits = fitshandle();
    myfits.create( filename );
    write_Healpix_map_to_fits(myfits, mask, PLANCK_FLOAT64);
    myfits.close();
  }


  system( "rm mask_scheme.fits" );
  fitshandle myfits = fitshandle();
  myfits.create("mask_scheme.fits");
  write_Healpix_map_to_fits(myfits, Map, PLANCK_FLOAT64);
  myfits.close();

  return 0;

}
