//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef OUTPUTFILEPROVIDER_HPP_
#define OUTPUTFILEPROVIDER_HPP_

#include <QObject>

#include "VDir.hpp"
#include "VInfo.hpp"
#include "InfoProvider.hpp"
#include "VTask.hpp"
#include "VTaskObserver.hpp"

class OutputFileClient;
class OutputCache;

class OutputFileProvider : public QObject, public InfoProvider
{
Q_OBJECT

public:
	 explicit OutputFileProvider(InfoPresenter* owner);

	 void visit(VInfoNode*);
	 void clear();

	 //Get a particular jobout file
     void file(const std::string& fileName,bool useCache=true);
     void fetchFile(const std::string& fileName,VDir::FetchMode fetchMode,bool useCache=true);
     void setDirectories(const std::vector<VDir_ptr>&);

     std::string joboutFileName() const;
     bool isTryNoZero(const std::string& fileName) const;

private Q_SLOTS:
     void slotOutputClientError(QString);
     void slotOutputClientProgress(QString,int);
     void slotOutputClientFinished();

private:
     VDir_ptr dirToFile(const std::string& fileName) const;
     void fetchFile(ServerHandler *server,VNode *n,const std::string& fileName,bool isJobout,bool detachCache);
	 void fetchJoboutViaServer(ServerHandler *server,VNode *n,const std::string&);
     bool fetchFileViaOutputClient(VNode *n,const std::string& fileName,bool useCache);
	 bool fetchLocalFile(const std::string& fileName);

     OutputFileClient *outClient_;
     bool useOutputClientOnly_;
     std::vector<VDir_ptr> dirs_;
     OutputCache* outCache_;
};

#endif
