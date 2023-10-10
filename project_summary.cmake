message( STATUS "------------------------------------------------------" )

if(Boost_FOUND)
    message( STATUS " Boost_NO_BOOST_CMAKE              : [${Boost_NO_BOOST_CMAKE}]" )
    message( STATUS " Boost_USE_MULTITHREADED           : [${Boost_USE_MULTITHREADED}]" )
    message( STATUS " Boost_NO_SYSTEM_PATHS             : [${Boost_NO_SYSTEM_PATHS}]" )
    message( STATUS " Boost_DETAILED_FAILURE_MSG        : [${Boost_DETAILED_FAILURE_MSG}]" )
    message( STATUS " Boost_MAJOR_VERSION               : [${Boost_MAJOR_VERSION}]" )
    message( STATUS " Boost_MINOR_VERSION               : [${Boost_MINOR_VERSION}]" )
    message( STATUS " Boost_SUBMINOR_VERSION            : [${Boost_SUBMINOR_VERSION}]" )
    message( STATUS " Boost_INCLUDE_DIRS                : [${Boost_INCLUDE_DIRS}]" )
    message( STATUS " Boost_LIBRARY_DIR_RELEASE         : [${Boost_LIBRARY_DIR_RELEASE}]" )
    message( STATUS " Boost_TIMER_LIBRARY               : [${Boost_TIMER_LIBRARY}]" )
    message( STATUS " Boost_CHRONO_LIBRARY              : [${Boost_CHRONO_LIBRARY}]" )
    message( STATUS " Boost_SYSTEM_LIBRARY              : [${Boost_SYSTEM_LIBRARY}]" )
    message( STATUS " Boost_FILESYSTEM_LIBRARY          : [${Boost_FILESYSTEM_LIBRARY}]" )
    message( STATUS " Boost_PROGRAM_OPTIONS_LIBRARY     : [${Boost_PROGRAM_OPTIONS_LIBRARY}]" )
    message( STATUS " Boost_DATE_TIME_LIBRARY           : [${Boost_DATE_TIME_LIBRARY}]" )
    message( STATUS " Boost_UNIT_TEST_FRAMEWORK_LIBRARY : [${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}]" )
    message( STATUS " Boost_TEST_EXEC_MONITOR_LIBRARY   : [${Boost_TEST_EXEC_MONITOR_LIBRARY}]" )
    message( STATUS " Boost_REGEX_LIBRARY               : [${Boost_REGEX_LIBRARY}]" )
    message( STATUS " Boost_PYTHON_LIBRARY              : [${Boost_PYTHON_LIBRARY}]" )
    message( STATUS " Boost_PYTHON_LIBRARY_RELEASE      : [${Boost_PYTHON_LIBRARY_RELEASE}]" )
    message( STATUS " Boost_PYTHON3_LIBRARY_RELEASE     : [${Boost_PYTHON3_LIBRARY_RELEASE}]" )
    message( STATUS " Boost_PYTHON27_LIBRARY            : [${Boost_PYTHON27_LIBRARY}]" )
    message( STATUS " Boost_PYTHON27_LIBRARY_RELEASE    : [${Boost_PYTHON27_LIBRARY_RELEASE}]" )
    message( STATUS " Boost_PYTHON36_LIBRARY            : [${Boost_PYTHON36_LIBRARY}]" )
    message( STATUS " Boost_PYTHON36_LIBRARY_RELEASE    : [${Boost_PYTHON36_LIBRARY_RELEASE}]" )
    message( STATUS " Boost_PYTHON37_LIBRARY            : [${Boost_PYTHON37_LIBRARY}]" )
    message( STATUS " Boost_PYTHON37_LIBRARY_RELEASE    : [${Boost_PYTHON37_LIBRARY_RELEASE}]" )
    message( STATUS " Boost_PYTHON38_LIBRARY            : [${Boost_PYTHON38_LIBRARY}]" )
    message( STATUS " Boost_PYTHON38_LIBRARY_RELEASE    : [${Boost_PYTHON38_LIBRARY_RELEASE}]" )
    message( STATUS " Boost_PYTHON39_LIBRARY            : [${Boost_PYTHON39_LIBRARY}]" )
    message( STATUS " Boost_PYTHON39_LIBRARY_RELEASE    : [${Boost_PYTHON39_LIBRARY_RELEASE}]" )
    message( STATUS " Boost_PYTHON310_LIBRARY           : [${Boost_PYTHON310_LIBRARY}]" )
    message( STATUS " Boost_PYTHON310_LIBRARY_RELEASE   : [${Boost_PYTHON310_LIBRARY_RELEASE}]" )
    message( STATUS " Boost_PYTHON311_LIBRARY           : [${Boost_PYTHON311_LIBRARY}]" )
    message( STATUS " Boost_PYTHON311_LIBRARY_RELEASE   : [${Boost_PYTHON311_LIBRARY_RELEASE}]" )
    message( STATUS " Boost_PYTHON312_LIBRARY           : [${Boost_PYTHON312_LIBRARY}]" )
    message( STATUS " Boost_PYTHON312_LIBRARY_RELEASE   : [${Boost_PYTHON312_LIBRARY_RELEASE}]" )
endif()

if (ENABLE_SSL)
    if (OPENSSL_FOUND)
        ecbuild_info(" OpenSSL VERSION  ${OPENSSL_VERSION}")
        ecbuild_info("         LIBS     ${OPENSSL_LIBRARIES}")
        ecbuild_info("         INCLUDES ${OPENSSL_INCLUDE_DIR}")
    else()
        ecbuild_warn("Can *not* find openssl libraries. ecflow will build without ssl support")
    endif() 
endif()
