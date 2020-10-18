# Unidata NetCDF

[![Build Status](https://travis-ci.org/Unidata/netcdf-c.svg?branch=master)](https://travis-ci.org/Unidata/netcdf-c)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/157/badge.svg)](https://scan.coverity.com/projects/157)

### About
The Unidata network Common Data Form (**netCDF**) is an interface for
scientific data access and a freely-distributed software library that
provides an implementation of the interface.  The netCDF library also
defines a machine-independent format for representing scientific data.
Together, the interface, library, and format support the creation,
access, and sharing of scientific data.  The current netCDF software
provides C interfaces for applications and data.  Separate software
distributions available from Unidata provide Java, Fortran, Python,
and C++ interfaces.  They have been tested on various common
platforms.

#### Properties
NetCDF files are self-describing, network-transparent, directly
accessible, and extendible.  `Self-describing` means that a netCDF file
includes information about the data it contains.  `Network-transparent`
means that a netCDF file is represented in a form that can be accessed
by computers with different ways of storing integers, characters, and
floating-point numbers.  `Direct-access` means that a small subset of a
large dataset may be accessed efficiently, without first reading through
all the preceding data.  `Extendible` means that data can be appended to
a netCDF dataset without copying it or redefining its structure.

#### Use
NetCDF is useful for supporting access to diverse kinds of scientific
data in heterogeneous networking environments and for writing
application software that does not depend on application-specific
formats.  For information about a variety of analysis and display
packages that have been developed to analyze and display data in
netCDF form, see

* [Software for Manipulating or Displaying NetCDF Data](http://www.unidata.ucar.edu/netcdf/software.html)

##### More information
For more information about netCDF, see

* [Unidata Network Common Data Form (NetCDF)](http://www.unidata.ucar.edu/netcdf/)

### Latest releases
You can obtain a copy of the latest released version of netCDF
software for various languages:

* [C library and utilities](http://github.com/Unidata/netcdf-c)
* [Fortran](http://github.com/Unidata/netcdf-fortran)
* [Java](http://www.unidata.ucar.edu/downloads/netcdf/netcdf-java-4/)
* [Python](http://github.com/Unidata/netcdf4-python)
* [C++](http://github.com/Unidata/netcdf-cxx4)

### Copyright
Copyright and licensing information can be found [here](http://www.unidata.ucar.edu/software/netcdf/copyright.html), as well as in the COPYRIGHT file accompanying the software

### Installation

#### netCDF-C

To install the netCDF-C software, please see the file INSTALL in the
netCDF-C distribution, or the (usually more up-to-date) document:

* [Building NetCDF](https://www.unidata.ucar.edu/software/netcdf/workshops/most-recent/building/Getting%20and%20Building%20netCDF-C.pdf)

#### ESDM-NetCDF

##### Compiling

To compile and install ESDM-NetCDF, first install a working build system:

    $ autoreconf --install --force

After this, the project can be configured with these options:

    $ ./configure --prefix=<esdm-netcdf-path> --with-esdm=<esdm-path> --with-hdf5=<hdf5-path> LDFLAGS=-L<hdf5-path>/lib CFLAGS=-I<hdf5-path>/include CC=mpicc --enable-parallel-tests --disable-dap

<esdm-netcdf-path> is an installation location of your choice,
<esdm-path> is the installation location of a previously built ESDM version, and
<hdf5_path> is the installation location of an HDF5 version.
Building, testing, and installing is then done with the usual

    $ make -j8
    $ make check
    $ make install

Currently, not all tests run successfully.

##### Using ESDM-NetCDF

The steps above install a NetCDF library at <esdm-netcdf-path>/lib/libnetcdf.so.
As such, any program that was linked against NetCDF can be executed with ESDM-NetCDF.
There are two main ways to do this:

 1. Use an environment where <esdm-netcdf-path>/lib/libnetcdf.so is the only NetCDF library available.

 2. Override the environment's NetCDF library with

        $ LD_PRELOAD=<esdm-netcdf-path>/lib/libnetcdf.so <command>

As a quick test, `nccopy <somefile>.nc <copy>.nc` can be used as <command>.
This should result in the following output by default:

    $ nccopy <somefile>.nc <copy>.nc
    [ESDM] [POSIX] /mnt/lustre01/pf/k/k203053/private/repositories/esiwace/esdm/src/utils/auxiliary.c:148 ea_read_file(): WARN cannot open esdm.conf No such file or directory

    ESDM has not been shutdown correctly. Stacktrace:
    3: /lib64/libc.so.6(__libc_start_main+0x100) [0x7f30f72d9d20]
    4: nccopy() [0x402a29]

The error message about shutdown can safely be ignored for now (this should be fixed in a future version of ESDM/ESDM-NetCDF).
The other message means that no ESDM configuration could be loaded, and as such, that the ESDM features are not usable.

To actually use ESDM, an esdm.conf file needs to be provided and a file system needs to be created.
ESDM comes with a few sample esdm.conf file which can be used to create an ESDM file system for first tests:

#    $ cp <esdm-source-path>/src/test/esdm-posix.conf esdm.conf
# The file esdm.conf is already linked to esdm-posix.conf in the directory
    $ cp <esdm-source-path>/src/test/esdm.conf <esdm-path>/install/bin/
#    $ <esdm-path>/bin/mkfs.esdm -g -v --create --config=esdm.conf
    $ <esdm-path>/install/bin/mkfs.esdm -g -v --create --config=esdm.conf
    [mkfs] Creating ./_metadummy
    [mkfs] Creating ./_posix1
    [mkfs] OK


After this, the warning should go away, and it should be possbile to perform a copy into ESDM:

    $ nccopy <somefile>.nc esdm://test

    ESDM has not been shutdown correctly. Stacktrace:
    3: /lib64/libc.so.6(__libc_start_main+0x100) [0x7f3aefb35d20]
    4: nccopy() [0x402a29]

The error can still be safely ignored.
This creates a container named "test" and copies all NetCDF variables into corresponding ESDM datasets within this container.


### Documentation
A language-independent User's Guide for netCDF, and some other
language-specific user-level documents are available from:

* [Language-independent User's Guide](https://www.unidata.ucar.edu/software/netcdf/guide_toc.html)
* [NetCDF-C Tutorial](http://www.unidata.ucar.edu/software/netcdf/docs/tutorial_8dox.html)
* [Fortran-90 User's Guide](http://www.unidata.ucar.edu/software/netcdf/documentation/historic/netcdf-f90/index.html#Top)
* [Fortran-77 User's Guide](http://www.unidata.ucar.edu/software/netcdf/documentation/historic/netcdf-f77/index.html#Top)
* [netCDF-Java/Common Data Model library](https://www.unidata.ucar.edu/software/netcdf-java/)
* [netCDF4-python](http://unidata.github.io/netcdf4-python/)

A mailing list, netcdfgroup@unidata.ucar.edu, exists for discussion of
the netCDF interface and announcements about netCDF bugs, fixes, and
enhancements.  For information about how to subscribe, see the URL

* [Unidata netCDF Mailing-Lists](http://www.unidata.ucar.edu/netcdf/mailing-lists.html)

### Feedback
We appreciate feedback from users of this package.  Please send comments, suggestions, and bug reports to <support-netcdf@unidata.ucar.edu>.  
