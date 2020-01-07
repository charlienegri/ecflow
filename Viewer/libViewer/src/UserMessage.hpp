//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef USER_MESSAGE_HPP_
#define USER_MESSAGE_HPP_

#include <QDebug>
#include <QString>

#include <string>

class UserMessage
{
//Q_OBJECT
public:
    UserMessage();

    enum MessageType {INFO, WARN, ERROR, DBG};  // note: cannot have DEBUG because of possible -DDEBUG in cpp!

    static void setEchoToCout(bool toggle) {echoToCout_ = toggle;}
    static void message(MessageType type, bool popup, const std::string& message);
    static bool confirm(const std::string& message);

    static void debug(const std::string& message);
    static std::string toString(int);

private:
    static bool echoToCout_;


};


#endif
