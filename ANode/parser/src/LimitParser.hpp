#ifndef LIMITPARSER_HPP_
#define LIMITPARSER_HPP_

//============================================================================
// Name        :
// Author      : Avi
// Revision    : $Revision: #6 $ 
//
// Copyright 2009-2019 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//
// Description :
//============================================================================

#include "Parser.hpp"

class LimitParser : public Parser {
public:
   explicit LimitParser(DefsStructureParser* p) : Parser(p) {}
	const char* keyword() const override { return "limit"; }
	bool doParse(const std::string& line, std::vector<std::string>& lineTokens) override;
};

#endif /* LIMITPARSER_HPP_ */
