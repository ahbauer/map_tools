BINDIR = /Users/bauer/software/bin
OBJDIR = /Users/bauer/software/obj
vpath %.o $(OBJDIR)
VPATH = $(BINDIR)

GSL         =  /opt/local/lib/libgsl.a
HEALPIXPATH =  /Users/bauer/software/pxcorr/Healpix_Partpix/src/cxx/osx
HDFDIR     =  /usr/local/hdf5-1.8.9/hdf5
HDF        =  /usr/local/hdf5-1.8.9/hdf5/lib/libhdf5.a /usr/local/hdf5-1.8.9/hdf5/lib/libhdf5_hl.a /usr/local/hdf5-1.8.9/hdf5/lib/libhdf5_cpp.a /usr/local/hdf5-1.8.9/hdf5/lib/libhdf5_hl_cpp.a
FITSIO      =  /opt/local/lib/libcfitsio.a
OPTLIBPATH  =  /opt/local/lib/
GFORTRAN    = /sw/lib/gcc4.6/lib/libquadmath.a /sw/lib/gcc4.6/lib/libgfortran.a
GCC         = /sw/lib/gcc4.6/lib/gcc/x86_64-apple-darwin11.2.0/4.6.2/libgcc.a

CC         = g++
CFLAGS     = -Wall -MD -g -O0 -I$(HEALPIXPATH)/include/ -I/opt/local/include/ -I/usr/local -I$(HDFDIR)/include 
LFLAGS     = -L$(HEALPIXPATH)/lib -L$(OPTLIBPATH) $(GCC) $(GFORTRAN) -lcfitsio -lcxxsupport -lhealpix_cxx -lpsht -lc_utils -lfftpack -L$(HDFDIR)/lib $(MPFLAG) -lz -lSystem  
MPFLAG    = -fopenmp 

DEP = ./Dependency_files

BINARIES = fits_to_healpix fits_to_partpix \
           hdf5_to_healpix hdf_to_ascii healpix_and_maps healpix_assignvals healpix_construct_ensemble healpix_convert healpix_cutmask \
           healpix_delta_to_n \
           healpix_contigregions healpix_dump healpix_eval_mask healpix_masktofile healpix_mask_list healpix_to_partpix healpix_to_random \
           healpix_stats \
           partpix_cut partpix_mask_from_cat partpix_to_healpix partpix_test \
           partpix_to_hdf5 

all:    $(DEP) \
        $(BINARIES) \
        TIDY

$(BINARIES):
	$(CC) $(CFLAGS) $^  -o $(BINDIR)/$@ $(LFLAGS) -lm

# Disable making of c files
%.o : %.c

# Disable .cc implicit rule
%.o : %.cc

# Produce object files from .C files only.
%.o : %.C
	$(CC) $(MPFLAG) $(CFLAGS) -c $< -o $@ 
	@  mv *.d $(DEP)


fits_to_healpix: fits_to_healpix.o $(FITSIO)
fits_to_partpix: fits_to_partpix.o $(FITSIO)
hdf5_to_healpix: hdf5_to_healpix.o $(HDF)
hdf_to_ascii: hdf_to_ascii.o $(HDF)
hdf_to_partpix: hdf_to_partpix.o $(HDF)
healpix_and_maps: healpix_and_maps.o
healpix_assignvals: healpix_assignvals.o
healpix_construct_ensemble: healpix_construct_ensemble.o
healpix_contigregions: healpix_contigregions.o
healpix_convert: healpix_convert.o
healpix_cutmask: healpix_cutmask.o
healpix_delta_to_n: healpix_delta_to_n.o
healpix_dump: healpix_dump.o
healpix_eval_mask: healpix_eval_mask.o
healpix_mask_list: healpix_mask_list.o
healpix_masktofile: healpix_masktofile.o
healpix_stats: healpix_stats.o
healpix_to_partpix: healpix_to_partpix.o
healpix_to_random: healpix_to_random.o $(GSL)
partpix_cut: partpix_cut.o
partpix_mask_from_cat: partpix_mask_from_cat.o
partpix_test: partpix_test.o
partpix_to_healpix: partpix_to_healpix.o
partpix_to_hdf5: partpix_to_hdf5.o $(HDF)

$(DEP):
	mkdir $@
	touch $@/default_dependency.d

.PHONY: TIDY
TIDY:
	-@      mv *.o $(OBJDIR)
#       -@      mv *.d $(DEP)

-include $(DEP)/*.d *.d

