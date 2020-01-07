#ifndef server_H
#define server_H
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


class server {
public:

	server(int port);
	virtual ~server();

	void run();
	virtual void serve(int) = 0;

private:

	server(const server&);
	server& operator=(const server&);

	int  soc_;
	bool fork_;

};

inline void destroy(server**) {}

#endif
