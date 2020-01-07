//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef VCONFIG_HPP_
#define VCONFIG_HPP_

#include <vector>

#include <boost/property_tree/ptree.hpp>

class VProperty;
class VServerSettings;
class VSettings;

//This singleton class stores the configuration of the viewer.

class VConfig
{
	friend class VServerSettings;

public:
    ~VConfig();
    
    static VConfig* instance();
    
    const std::string& appName() const {return appName_;}
    const std::string& appLongName() const {return appLongName_;}
    void init(const std::string& parDir);
    const std::vector<VProperty*>& groups() const {return groups_;}
    VProperty* find(const std::string& path);

    VProperty* cloneServerGui(VProperty *linkTarget);

    void saveSettings();
    void importSettings();

protected:
    VConfig();
    
    void loadInit(const std::string& parFile);
    void loadProperty(const boost::property_tree::ptree& pt,VProperty *prop);
    void loadSettings();
    void saveSettings(const std::string& parFile,VProperty* guiProp,VSettings* vs,bool);
    void loadSettings(const std::string& parFile,VProperty* guiProp,bool);
    void loadImportedSettings(const boost::property_tree::ptree& pt,VProperty* guiProp);
    bool readRcFile(const std::string& rcFile,boost::property_tree::ptree& pt);

    VProperty* group(const std::string& name);

    static VConfig* instance_;
    
    std::string appName_;
    std::string appLongName_;
    std::vector<VProperty*> groups_;
};

#endif
