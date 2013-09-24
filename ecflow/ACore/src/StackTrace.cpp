////============================================================================
//// Name        :
//// Author      : Avi
//// Revision    : $Revision: #7 $ 
////
//// Copyright 2009-2012 ECMWF. 
//// This software is licensed under the terms of the Apache Licence version 2.0 
//// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
//// In applying this licence, ECMWF does not waive the privileges and immunities 
//// granted to it by virtue of its status as an intergovernmental organisation 
//// nor does it submit to any jurisdiction. 
////
//// Description :
////============================================================================
//
//#include "StackTrace.hpp"
//#include <sstream>
//#include <iostream>
//
//#if defined(__GNUC__)
//#include <execinfo.h>
//#include <cxxabi.h>
//#endif
//
////#define DEBUG_ME 1
//
//using namespace std;
//
//namespace ecf {
//
//std::string DeMangleCXXName( const std::string &stackString )
//{
//#ifdef DEBUG_ME
//	std::cout <<"DeMangleCXXName stackString " <<  stackString << "\n";
//#endif
//    size_t start = stackString.find( '(' );
//    size_t end = stackString.find( '+' );
//    if( std::string::npos == start || std::string::npos == end ) return stackString;
//
//    ++start; // puts us pass the '('
//    size_t n = end - start;
//    std::string mangled = stackString.substr( start, n );
//
//    const size_t kMaxNameLen = 4096;
//    char function[kMaxNameLen];
//    int status;
//    size_t len;
//    char *ret = abi::__cxa_demangle( mangled.c_str(), function, &len, &status );
//    if (ret &&  ( 0 == status ))  {// de-mangling successful
//    	// status is set to one of the following values:
//    	// 0: The de mangling operation succeeded.
//    	// -1: A memory allocation failure occurred.
//    	// -2: mangled_name is not a valid name under the C++ ABI mangling rules.
//    	// -3: One of the arguments is invalid.
//
//    	// Can return the variable string(ret) or std::string( function )
//    	// std::cout << ret << "\n";
//
//        return std::string( function );
//    }
//
//    // If de-mangling fails, returned mangled name with some parens
//    return mangled + "()";
//}
//
//std::string StackTrace::dump( const std::string &file, int line, int depth )
//{
//#if defined(__GNUC__)
////	   This is not well-optimised, requires compilation with "-rdynamic" on linux
////	    and doesn't do a great job of demangling the symbol names. It is sufficient
////	    though to work out call trace.
//
//#ifdef DEBUG_ME
//	std::cout <<"MyStackTrace \n";
//#endif
//
//    std::stringstream result;
//    result << "Call Stack from " << file << Str::COLON() << line << "\n";
//
//    // create a void* array to hold the function addresses. We will only go at most 50 deep
//	const size_t kMaxDepth = depth;
//    void *stackAddrs[kMaxDepth];
//    size_t stackDepth = backtrace( stackAddrs, kMaxDepth );
//
//    // now get the function names associated with these symbols. This should work for elf
//    // binaries, though additional linker options may need to have been called
//    // (e.g. -rdynamic for GNU ld. See the glibc documentation for 'backtrace')
//	char** stackStrings = backtrace_symbols( stackAddrs, stackDepth );
//
//#ifdef DEBUG_ME
//	std::cout <<"   MyStackTrace 6  \n";
//#endif
//
//    for( size_t i = 1; i < stackDepth; ++i ) {
//        result << "   " << DeMangleCXXName( stackStrings[i] ) << "\n";
//    }
//
//    // we now need to release the memory of the symbols array. Since it was allocated using
//    // malloc, we must release it using 'free'
//    std::free( stackStrings );
//    return result.str();
//#else
//    return "Back trace no supported on this platform";
//#endif
//}
//}
//
