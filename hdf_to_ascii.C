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
#include <ctime>

#include "H5Cpp.h"
using namespace H5;

// Operator function
extern "C" herr_t print_data( hid_t loc_id, const char *dataset_name, /*const H5L_info_t *linfo,*/ void *optdata );


herr_t print_data( hid_t loc_id, const char *dataset_name, /*const H5L_info_t *linfo,*/ void *optdata ){
    
    // this should be run on a dataset...

    //H5File *file = (H5File*) optdata;

    hid_t dataset_id = H5Dopen2( loc_id, dataset_name, H5P_DEFAULT );
    hid_t dataspace_id = H5Dget_space( dataset_id );
    //DataSet dataset = file->openDataSet( dataset_name );
    //DataSpace dataspace = dataset.getSpace();

    hsize_t dims_out[2];
    int rank = H5Sget_simple_extent_ndims( dataspace_id );
    H5Sget_simple_extent_dims( dataspace_id, dims_out, NULL );
    //int ndims = dataspace.getSimpleExtentDims( dims_out, NULL);
    //int rank = dataspace.getSimpleExtentNdims();
    
    hsize_t datatype_id = H5Dget_type(dataset_id);
    H5T_class_t t_class = H5Tget_class(datatype_id);
    //DataType data_type = dataset.getDataType();

    cerr << "rank = " << rank << ": " << dims_out[0] << " " << dims_out[1] << " datatype " << t_class << endl;

    int *data_out_int;
    if(t_class == 0){
        data_out_int = (int*) malloc( sizeof(int*) * dims_out[0]*dims_out[1] );
        H5Dread( dataset_id, datatype_id, dataspace_id, H5S_ALL, H5P_DEFAULT, data_out_int );
        for( unsigned int i=0; i<dims_out[0]; ++i ){
            for( unsigned int j=0; j<dims_out[1]; ++j ){
                cout << data_out_int[dims_out[1]*i+j] << " ";
            }
            cout << endl;
        }
        free( data_out_int );
    }
    
    double *data_out_double;
    if(t_class == 1){
        data_out_double = (double*) malloc( sizeof(double*) * dims_out[0]*dims_out[1] );
        H5Dread( dataset_id, datatype_id, dataspace_id, H5S_ALL, H5P_DEFAULT, data_out_double );
        for( unsigned int i=0; i<dims_out[0]; ++i ){
            for( unsigned int j=0; j<dims_out[1]; ++j ){
                cout << data_out_double[dims_out[1]*i+j] << " ";
            }
            cout << endl;
        }
        free( data_out_double );
    }
    
    /*

    DataSpace memspace( ndims, dims_out );

    double *data_out_double;
    int *data_out_int;

    if( data_type.getClass() == 0 ){
        data_out_int = (int*) malloc( sizeof(int*) * dims_out[0]*dims_out[1] );
        dataset.read( data_out_int, data_type, memspace, dataspace );
    }
    else if( data_type.getClass() == 1 ){
        data_out_double = (double*) malloc( sizeof(double*) * dims_out[0]*dims_out[1] );
        dataset.read( data_out_double, data_type, memspace, dataspace );
    }
    else{
        cerr << "unknown data type class " << data_type.getClass() << endl;
        return 1;
    }

    if( data_type.getClass() == 0 ){
        for( unsigned int i=0; i<dims_out[0]; ++i ){
            for( unsigned int j=0; j<dims_out[1]; ++j ){
                cout << data_out_int[dims_out[0]*j+i] << " ";
            }
            cout << endl;
        }
        // for( unsigned int i=0; i<dims_out[0]*dims_out[1]; ++i ){
        //     outfile << data_out_int[i] << endl;
        // }
        free( data_out_int );
    }
    else if( data_type.getClass() == 1 ){
        for( unsigned int i=0; i<dims_out[0]; ++i ){
            for( unsigned int j=0; j<dims_out[1]; ++j ){
                cout << data_out_double[dims_out[0]*j+i] << " ";
            }
            cout << endl;
        }
        free( data_out_double );
    }
    */

    return 0;

}


int main (int argc, char **argv){
    
    if( argc != 2 ){
        cerr << "Usage: hdf_to_ascii.C hdfname" << endl;
        cerr << "prints to stdout" << endl;
        return 1;
    }

    string infilename( argv[1] );

    H5File file1( infilename, H5F_ACC_RDONLY );
    
    // let's just go get the stuff we want.
    Group group = file1.openGroup("meta");
    DataSet dataSet = group.openDataSet("meta");
    Attribute attr = dataSet.openAttribute("ang_mean");
    int attrsize = attr.getStorageSize();
    StrType dataType(PredType::C_S1, attrsize);

    char *u_mean = (char*) malloc(attrsize);
    attr.read( dataType, u_mean );
    cerr << u_mean << endl;

    attr = dataSet.openAttribute("ang_width");
    attrsize = attr.getStorageSize();
    dataType = StrType(PredType::C_S1, attrsize);
    char *u_width = (char*) malloc(attrsize);
    attr.read( dataType, u_width );
    cerr << u_width << endl;

    free(u_mean);
    free(u_width);
    

    int n_objs1 = file1.getNumObjs(); 
    cerr << "The file has " << n_objs1 << " objects" << endl;

    for( int g=0; g<n_objs1; ++g ){
        
        H5G_obj_t firstType = file1.getObjTypeByIdx(g);
        if( firstType != H5G_GROUP ){
            cerr << "Hmmm, the first thing is not a group." << endl;
            return 1;
        }
        string group_name = file1.getObjnameByIdx(g);
        cerr << "   Group name is " << group_name << endl;

        Group group = file1.openGroup(group_name);
        int n_objs2 = group.getNumObjs(); 
        cerr << "   The group has " << n_objs2 << " objects" << endl;
        if( group_name == "meta" ){
            int nattr = group.getNumAttrs();
            cerr << "Group meta has " << nattr << " attributes" << endl;
            for( int k=0; k<nattr; ++k ){
                Attribute attr = group.openAttribute(k);
                string attrname = attr.getName();
                int attrsize = attr.getStorageSize();
                cerr << "attribute " << attrname << " has storage size " << attrsize << endl;
            }
        }
        for( int i=0; i<n_objs2; ++i ){
            string dataset_name = group.getObjnameByIdx(i);
            cerr << "      Object name is " << dataset_name << endl;

            if( dataset_name == "meta" || dataset_name == "bright" ){
                int type = group.getObjTypeByIdx(i);
                cerr << "object is of type " << type << endl;
                if( type == 0 ){
                    Group subgroup = group.openGroup(dataset_name);
                    int n_objs3 = group.getNumObjs(); 
                    cerr << "   The group has " << n_objs3 << " objects" << endl;
                    for( int j=0; j<1; ++j ){
                        int subdataset_type = subgroup.getObjTypeByIdx(j);
                        string subdataset_name = subgroup.getObjnameByIdx(j);
                        cerr << "      subobject name is " << subdataset_name << ", type " << subdataset_type << endl;
                        if( subdataset_type == 1 ){
                            DataSet subdataset = subgroup.openDataSet(subdataset_name);
                            int size = subdataset.getStorageSize();
                            cerr << "the subdataset takes up size " << size << endl;
                            size = subdataset.getInMemDataSize();
                            cerr << "in memory, " << size << endl;
                            //ArrayType array = subgroup.openArrayType(subdataset_name);
                            int nattr = subdataset.getNumAttrs();
                            cerr << "the subdataset has " << nattr << " attributes" << endl;
                            for( int k=0; k<nattr; ++k ){
                                Attribute attr = subdataset.openAttribute(k);
                                string attrname = attr.getName();
                                int attrsize = attr.getStorageSize();
                                cerr << "attribute " << attrname << " has storage size " << attrsize << endl;
                                DataType dataType(H5T_STRING, attrsize);
                                char *attribute = (char*) malloc(attrsize);
                                attr.read( dataType, attribute );
                                cerr << attribute << endl;
                                free(attribute);
                            }
                        }
                    }
                }
                else if( type == 1 ){
                    DataSet dataSet = group.openDataSet(dataset_name);
                    int nattr = dataSet.getNumAttrs();
                    cerr << "the dataset has " << nattr << " attributes" << endl;
                    for( int k=0; k<nattr; ++k ){
                        Attribute attr = dataSet.openAttribute(k);
                        string attrname = attr.getName();
                        int attrsize = attr.getStorageSize();
                        cerr << "attribute " << attrname << " has storage size " << attrsize << endl;
                        DataType dataType(H5T_STRING, attrsize);
                        char *attribute = (char*) malloc(attrsize);
                        attr.read( dataType, attribute );
                        cerr << attribute << endl;
                        free(attribute);
                    }
                }
            }
            else{
                DataSet metaDataSet = group.openDataSet(dataset_name);
                int size = metaDataSet.getStorageSize();
                cerr << "Hi!  the dataset takes up " << size << " space" << endl;
                DSetCreatPropList proplist = metaDataSet.getCreatePlist();
                int nfilts = proplist.getNfilters();
                cerr << "there are " << nfilts << " filters" << endl;
                DataSpace metaSpace = metaDataSet.getSpace();
                int ndims = metaSpace.getSimpleExtentNdims();
                cerr << "there are " << ndims << " dimensions in the dataspace" << endl;
                hsize_t dims[2];
                dims[0] = 0;
                dims[1] = 0;
                metaSpace.getSimpleExtentDims(&(dims[0]), NULL);
                cerr << "dimensions are of size " << dims[0] << " " << dims[1] << endl;
                //metaSpace.selectAll();
                int nelems = metaSpace.getSimpleExtentNpoints(); 
                cerr << "there are " << nelems << " elements in the dataspace" << endl;
            }
        }
    
    }
    
    /*
    double ctest1 = clock();
    H5File file( infilename, H5F_ACC_RDONLY );
    H5G_obj_t firstType = file.getObjTypeByIdx(0);
    if( firstType == H5G_GROUP ){
        cerr << "the first thing is a group!" << endl;
        string group_name = file.getObjnameByIdx(0);
        cerr << "with name " << group_name << endl;
        int index=0;
        file.iterateElems( group_name, &index, print_data, &file );
    }
    else if( firstType == H5G_DATASET ){
        cerr << "the first thing is a dataset!" << endl;
        string dataset_name = file.getObjnameByIdx(0);
        cerr << "with name " << dataset_name << endl;
        print_data( 0, dataset_name.c_str(), &file );
    }
    double ctest2 = clock();
    cerr << "time to read hdf5: " << (ctest2-ctest1)/CLOCKS_PER_SEC << endl;
    */

    
    return 0;
    
}


