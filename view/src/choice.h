#ifndef choice_H
#define choice_H
//=============================================================================================
// Name        : 
// Author      : 
// Revision    : $Revision: #4 $ 
//
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description : 
//=============================================================================================


class choice {
public:

	choice(int value = 0) : value_(value) {}
	choice(const choice& other) : value_(other.value_) {}

	operator int() const { return value_; }
	choice& operator=(int n) { value_ = n; return *this; }
	choice& operator=(const choice& other) { value_ = other.value_; return *this; }

private:
	
	int value_;

};

inline void destroy(choice**) {}

#endif
