#!/bin/ksh
# ==================================================================
# Error handling
set -e # stop the shell on first error
set -u # fail when using an undefined variable

# ensure correct permission for installation
umask 0022

# docker build?
[[ -d opt/boost_1_53_0/ ]] && export BOOST_ROOT=/opt/boost_1_53_0/ ARCH=linux PYTHONPATH=/usr/local:${PYTHONPATH:=} LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH:=}

# ====================================================================
show_error_and_exit() {
   echo "cmake.sh expects at least one argument"
   echo " cmake.sh [ options] "
   echo ""
   echo "   debug          - make a DEBUG build"
   echo "   make           - run make after cmake"
   echo "   ecbuild        - Use git cloned ecbuild over the module loaded ecbuild(default)"
   echo "   sys_install    - install to /usr/local/apps/ecflow/<version>  defaults is /var/tmp/$USER/install/cmake/ecflow/<version>"
   echo "   test           - run all the tests"
   echo "   test_safe      - only run deterministic tests"
   echo "   ctest          - all ctest -R <test> -V"
   echo "   clang          - build with clang compiler"
   echo "   clang_tidy     - create compilation database for clang_tdiy and then call run-clang-tidy.py"
   echo "   tsan           - is short for clang thread sanitiser"
   echo "   asan           - is short for address sanitiser"
   echo "   msan           - is short for memory sanitiser"
   echo "   no_gui         - Don't build the gui"
   echo "   ssl            - build using openssl"
   echo "   secure_user    - enable password for client server"
   echo "   log            - enable debug output"
   echo "   package_source - produces ecFlow-<version>-Source.tar.gz file, for users"
   echo "                    copies the tar file to $SCRATCH"
   echo "   copy_tarball   - copies ecFlow-<version>-Source.tar.gz to /tmp/$USER/tmp/. and untars file"
   echo ""
   echo "For a system install"
   echo "./cmake.sh sys_install make -j8 install"
   exit 1
}

sys_install=
ecbuild_arg=
copy_tarball_arg=
package_source_arg=
make_arg=
make_only_arg=
test_arg=
test_safe_arg=
clang_arg=
clang_tidy_arg=
intel_arg=
tsan_arg=
mode_arg=release
verbose_arg=
ctest_arg=
clean_arg=
no_gui_arg=
ssl_arg=
secure_user_arg=
log_arg=
asan_arg=
msan_arg=
while [[ "$#" != 0 ]] ; do   
   if [[ "$1" = debug || "$1" = release ]] ; then
      mode_arg=$1
   elif  [[ "$1" = make_only ]] ; then
      make_only_arg=make
      shift
      while [[ "$#" != 0 ]] ; do
         make_only_arg="$make_only_arg $1"
         shift
      done
      break
   elif  [[ "$1" = make ]] ; then
      make_arg=$1
      shift
      while [[ "$#" != 0 ]] ; do
         make_arg="$make_arg $1"
         shift
      done
      break
   elif [[ "$1" = no_gui ]] ;      then no_gui_arg=$1 ;
   elif [[ "$1" = ssl ]]   ;       then ssl_arg=$1 ;
   elif [[ "$1" = sys_install ]] ; then sys_install=$1 ;
   elif [[ "$1" = secure_user ]] ; then secure_user_arg=$1 ;
   elif [[ "$1" = ecbuild ]] ; then ecbuild_arg=$1 ;
   elif [[ "$1" = log ]]   ; then log_arg=$1 ;
   elif [[ "$1" = clang ]] ; then clang_arg=$1 ;
   elif [[ "$1" = clang_tidy ]] ; then clang_tidy_arg=$1 ;
   elif [[ "$1" = intel ]] ; then intel_arg=$1 ;
   elif [[ "$1" = clean ]] ; then clean_arg=$1 ;
   elif [[ "$1" = tsan ]]   ; then tsan_arg=$1 ;
   elif [[ "$1" = asan ]]  ; then asan_arg=$1 ;
   elif [[ "$1" = msan ]]  ; then msan_arg=$1 ;
   elif [[ "$1" = package_source ]] ; then package_source_arg=$1 ;
   elif [[ "$1" = copy_tarball ]] ; then copy_tarball_arg=$1 ;
   elif [[ "$1" = test ]] ;  then test_arg=$1 ;
   elif [[ "$1" = test_safe ]] ; then test_safe_arg=$1 ;
   elif [[ "$1" = ctest ]] ; then  
      ctest_arg=$1 ; 
      shift
      while [[ "$#" != 0 ]] ; do
         ctest_arg="$ctest_arg $1"
         shift
      done
      break
   else
   	 show_error_and_exit
   fi

   # shift remove last argument
   shift
done

echo "copy_tarball_arg=$copy_tarball_arg"
echo "package_source_arg=$package_source_arg"
echo "make_arg=$make_arg"
echo "test_arg=$test_arg"
echo "test_safe_arg=$test_safe_arg"
echo "clang_arg=$clang_arg"
echo "clang_tidy_arg=$clang_tidy_arg"
echo "tsan_arg=$tsan_arg"
echo "mode_arg=$mode_arg"
echo "verbose_arg=$verbose_arg"
echo "no_gui_arg=$no_gui_arg"
echo "ecbuild_arg=$ecbuild_arg"
set -x # echo script lines as they are executed
set -o pipefail # fail if last(rightmost) command exits with a non-zero status

# ==================== compiler flags ========================================
# 
# GNU 4.8+ -Wno-unused-local-typedefs   -> get round warning in boost headers
# GNU 5.3  -Wno-unused-variable         -> get round warning in boost headers
# GNU 6.1  -Wno-deprecated-declarations -> auto_ptr deprecated warning, mostly in boost headers  
# CLANG    -ftemplate-depth=512
#
CXX_FLAGS="-Wno-unused-local-typedefs -Wno-unused-variable -Wno-deprecated-declarations -Wno-address -Wno-maybe-uninitialized"
 

# ==================== modules ================================================
# To load module automatically requires Korn shell, system start scripts
#
module unload clang
module load gnu/7.3.0
cmake_extra_options="-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
if [[ "$clang_arg" = clang || "$clang_tidy_arg" = clang_tidy ]] ; then
    # ecflow fails to write boost ser' files with clang 6.0.1/7.0.1, but in debug all tests pass
    # Had to apply fix: http://clang-developers.42468.n3.nabble.com/boost-serialization-crash-with-clang-5-0-0-td4058283.html
    # - still have other crashes n serilisation see: ECFLOW-1328
    module unload gnu
    module load clang/7.0.1
    CXX_FLAGS=""
    CXX_FLAGS="$CXX_FLAGS -Wno-deprecated-declarations -Wno-deprecated-register -Wno-expansion-to-defined"
fi

module load cmake/new    # need cmake 3.12.0 to build python3. Allow boost python 2 and 3 libs to be found  
module load ecbuild/new
module load boost/1.53.0     # uncomment to use local BOOST_ROOT  may need -DBOOST_ROOT=$BOOST_ROOT 
module load python3/3.6.5-01
# To build python3 when cmake < 3.12.0 use
# -DPYTHON_EXECUTABLE=/usr/local/apps/python3/%PYTHON3_VERSION%/bin/python3 

# ==============================================================================================
# sanitisers
if [[ "$tsan_arg" = tsan && "$asan_arg" = asan ]] ; then
    echo "Cant use address and thread sanitiser at the same time"
fi
if [[ "$tsan_arg" = tsan ]] ; then
   CXX_FLAGS="$CXX_FLAGS -fsanitize=thread -fno-omit-frame-pointer"
   cmake_extra_options="$cmake_extra_options -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=thread'"  # LINK FLAGS
fi
if [[ "$asan_arg" = asan ]] ; then
   CXX_FLAGS="$CXX_FLAGS -fsanitize=address -fno-omit-frame-pointer"
   cmake_extra_options="$cmake_extra_options -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address'"  # LINK FLAGS
fi
if [[ "$msan_arg" = msan ]] ; then
   CXX_FLAGS="$CXX_FLAGS -fsanitize=memory -fPIE -fno-omit-frame-pointer -fsanitize-memory-track-origins"
   cmake_extra_options="$cmake_extra_options -DCMAKE_EXE_LINKER_FLAGS=-fsanitize=memory"  # LINK FLAGS
   #LINK_FLAGS='-DCMAKE_EXE_LINKER_FLAGS="-fsanitize=memory -fPIE -pie"'
fi

if [[ "$ARCH" = cray ]] ; then

    # disable new UI, no QT on cray
    # Use the cray wrappers, these will add the correct flags.
    # Assumes we have CRAY_ADD_RPATH=yes
    cmake_extra_options="$cmake_extra_options -DENABLE_UI=OFF -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=CC -DCMAKE_Fortran_COMPILER=ftn"
    
    if [[ $intel_arg = intel ]] ; then
        module swap PrgEnv-cray PrgEnv-intel
    else
    	module swap PrgEnv-cray PrgEnv-gnu
    fi
    module unload atp                     # must use for NON MPI code (ATP abnormal termination processing only works with cray MPI for ESM modes)
    module load craype-target-local_host  # must use for NON MPI code
    export CRAY_ADD_RPATH=yes
    export ECFLOW_CRAY_BATCH=1
fi

 
########################################################################################
# ====================================================================================
# default to Release  
cmake_build_type=
if [[ $mode_arg = debug ]] ; then
    cmake_build_type=Debug
else
    cmake_build_type=Release
fi


# ====================================================================================
# clean up source before packaging, do this after deleting ecbuild
#
if [[ $package_source_arg = package_source ]] ; then
	source build_scripts/clean.sh
fi

# ====================================================================================
# Use for local install
release=$(cat VERSION.cmake | grep 'set( ECFLOW_RELEASE' | awk '{print $3}'| sed 's/["]//g')
major=$(cat VERSION.cmake   | grep 'set( ECFLOW_MAJOR'   | awk '{print $3}'| sed 's/["]//g')
minor=$(cat VERSION.cmake   | grep 'set( ECFLOW_MINOR'   | awk '{print $3}'| sed 's/["]//g')

# =======================================================================================
# Change directory
#
source_dir=$(pwd)
workspace=$(pwd)/..

if [[ $clean_arg = clean ]] ; then
	rm -rf ../bdir4/$mode_arg/ecflow
fi

mkdir -p ../bdir4/$mode_arg/ecflow
cd ../bdir4/$mode_arg/ecflow

# =============================================================================================
# ctest
#
if [[ $test_safe_arg = test_safe ]] ; then
	ctest -R ^u_
	ctest -R c_
	ctest -R py_u
	ctest -R s_client
	if [[  $test_safe_arg = test_safe ]] ; then
	   exit 0
	fi
fi
if [[ "$ctest_arg" != "" ]] ; then
    if [[ "$asan_arg" = asan ]] ; then
        if [[ $clang_arg != "clang" ]] ; then
            # for python module we need to preload asan as it needs to be the very first library
            # ==2971==ASan runtime does not come first in initial library list; 
            #              you should either link runtime to your application or manually preload it with LD_PRELOAD.
	        export LD_PRELOAD=/usr/local/apps/gcc/6.3.0/lib64/gcc/x86_64-suse-linux/6.3.0/libasan.so.3 
	    fi
	    export ASAN_OPTIONS=suppressions=$WK/build_scripts/ecflow_asan.supp  
	    export LSAN_OPTIONS=suppressions=$WK/build_scripts/ecflow_lsan.supp
	    $ctest_arg  
    elif [[ "$tsan_arg" = tsan ]] ; then
        if [[ $clang_arg != "clang" ]] ; then
            # LD_PRELOAD needed otherwise we get: .... cannot allocate memory in static TLS block
            export LD_PRELOAD=/usr/local/apps/gcc/6.3.0/lib64/gcc/x86_64-suse-linux/6.3.0/libtsan.so
        fi
        export ASAN_OPTIONS=suppressions=$WK/build_scripts/ecflow_asan.supp  
        export LSAN_OPTIONS=suppressions=$WK/build_scripts/ecflow_lsan.supp
        $ctest_arg 
    else
        $ctest_arg 
	fi
	exit 0
fi

if [[ "$make_only_arg" != "" ]] ; then
	$make_only_arg
	exit 0
fi

# ====================================================================================
#
# -DCMAKE_PYTHON_INSTALL_TYPE = [ local | setup ]
# -DCMAKE_PYTHON_INSTALL_PREFIX should *only* used when using python setup.py (CMAKE_PYTHON_INSTALL_TYPE=setup)
#   *AND* for testing python install to local directory
#

ssl_options=
if [[ $ssl_arg = ssl ]] ; then
    ssl_options="-DENABLE_SSL=ON"
fi

secure_user_options=
if [[ $secure_user_arg = secure_user ]] ; then
    secure_user_options="-DENABLE_SECURE_USER=ON"
fi

log_options=
if [[ $log_arg = log ]] ; then
    log_options="-DECBUILD_LOG_LEVEL=DEBUG"
fi

gui_options=
if [[ $no_gui_arg = no_gui ]] ; then
    gui_options="-DENABLE_GUI=OFF -DENABLE_UI=OFF"
fi

test_options=
if [[ $test_arg = test ]] ; then
   test_options="-DENABLE_ALL_TESTS=ON"
fi

if [[ $package_source_arg = package_source ]] ; then
    # for packaging we build GUI by default, and do not run all tests
    gui_options=  
fi

install_prefix=/var/tmp/$USER/install/cmake/ecflow/$release.$major.$minor
if [[ $sys_install = sys_install ]] ; then
   install_prefix=/usr/local/apps/ecflow/$release.$major.$minor
fi

ecbuild=ecbuild
if [[ $ecbuild_arg = ecbuild ]] ; then
   ecbuild=$workspace/ecbuild/bin/ecbuild
fi

# An alternative is to run cmake directly. (i.e to use its options/flags)
# cmake -C $workspace/ecflow/bamboo/macosx1010-flags.cmake $source_dir \
#        -DCMAKE_MODULE_PATH=$workspace/ecbuild/cmake \
#  .....

$ecbuild $source_dir \
            -DCMAKE_BUILD_TYPE=$cmake_build_type \
            -DCMAKE_INSTALL_PREFIX=$install_prefix  \
            -DENABLE_WARNINGS=ON \
            -DCMAKE_CXX_FLAGS="$CXX_FLAGS" \
            -DSITE_SPECIFIC_SERVER_SCRIPT="/home/ma/emos/bin/ecflow_site.sh" \
            ${cmake_extra_options} \
            ${gui_options} \
            ${ssl_options} \
            ${secure_user_options} \
            ${log_options} \
            ${test_options} # -DCMAKE_PREFIX_PATH="/tmp/$USER/opt/qt5/"
            
            #-DPYTHON_EXECUTABLE=/usr/local/apps/python3/3.6.5-01/bin/python3 \
            #-DPYTHON_EXECUTABLE=/usr/local/apps/python/2.7.12-01/bin/python \
            #-DCMAKE_EXE_LINKER_FLAGS='-fsanitize=memory -fPIE -pie' 
            #-DENABLE_STATIC_BOOST_LIBS=ON \
            #-DCMAKE_PYTHON_INSTALL_TYPE=local \
            #-DENABLE_PYTHON=OFF   \
            #-DENABLE_PYTHON_PTR_REGISTER=ON  \
            #-DCMAKE_PYTHON_INSTALL_PREFIX=/var/tmp/$USER/install/cmake/ecflow/$release.$major.$minor   \
            #-DCMAKE_PREFIX_PATH="/usr/local/apps/qt/5.5.0/5.5/gcc_64/" \
            #-DENABLE_GUI=ON       \  # ecflowview
            #-DENABLE_UI=ON        \  # ecflow_ui      
            #-DENABLE_ALL_TESTS=ON \
            #-DENABLE_SERVER=OFF   \
            #-DENABLE_PROFILING=ON \
            #-DECBUILD_GPROF_FLAGS \
# export PATH=/tmp/$USER/opt/qt5/bin:$PATH

if [[ "$clang_tidy_arg" = clang_tidy ]] ; then
    python $WK/build_scripts/run-clang-tidy.py
fi
# =============================================================================================
if [[ "$make_arg" != "" ]] ; then
	$make_arg 
	# $make_arg VERBOSE=1
	
    # generate the server file locally, and install it. Otherwise list of server will not be complete set
    echo $make_arg | grep -q "install"
	if [[ $? -eq 0 ]] ; then
		if [[ -f /home/ma/emos/bin/ecflow_site_server_install.sh ]] ; then

   			/home/ma/emos/bin/ecflow_site_server_install.sh -g

    		if [[ -f servers ]] ; then
        		mv servers $install_prefix/share/ecflow/.
            fi
		fi
	fi
	exit 0
fi

# =============================================================================================
if [[ $package_source_arg = package_source ]] ; then
	make package_source
	
	if [[ $copy_tarball_arg = copy_tarball ]] ; then
		rm -rf /tmp/$USER/tmp
		mkdir -p /tmp/$USER/tmp
		cp ecFlow-$release.$major.$minor-Source.tar.gz  /tmp/$USER/tmp/.
		cd /tmp/$USER/tmp/
		tar -zxf ecFlow-$release.$major.$minor-Source.tar.gz
	fi
	
	cp ecFlow-$release.$major.$minor-Source.tar.gz $SCRATCH/.
fi

# =========================================================================================
# NOTES:
# Boost:  
#  By default it looks for environment variable BOOST_ROOT, if not it can specified on the command line. i.e
#  -DBOOST_ROOT=/var/tmp/$USER/boost/boost_1_53_0

# ============================================================================================
# Python:
# -DCMAKE_PYTHON_INSTALL_TYPE = [ local | setup ]
#    default: local
#    local  : this will install to $INSTALL_PREFIX/$release.$major.$minor/lib/python2.7/site-packages/ecflow/
#    setup  : experimental only, python way of installing
#
#    -DCMAKE_PYTHON_INSTALL_PREFIX should *only* used when using python setup.py (CMAKE_PYTHON_INSTALL_TYPE=setup)
#    *AND* for testing python install to local directory
