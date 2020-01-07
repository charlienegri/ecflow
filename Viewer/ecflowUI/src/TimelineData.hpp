//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//=============================================================================

#ifndef TIMELINEDATA_HPP
#define TIMELINEDATA_HPP

#include <string>
#include <vector>

#include <QDateTime>
#include <QObject>
#include <QHash>
#include <QtGlobal>

struct TimelineItemStats
{
    int total;
    int min;
    int max;
    int perc25;
    int median;
    int perc75;
};


class TimelineItem
{
public:
    enum Type {UndeterminedType,ServerType,SuiteType,FamilyType,TaskType};

    TimelineItem() : type_(UndeterminedType), sortIndex_(0), treeIndex_(0) {}
    TimelineItem(const std::string& path,unsigned char status,unsigned int time,Type type=UndeterminedType);
    size_t size() const {return status_.size();}
    const std::string& path() const {return path_;}
    Type type() const {return type_;}
    void setType(Type t) {type_ = t;}
    size_t sortIndex() const {return sortIndex_;}
    size_t treeIndex() const {return treeIndex_;}
    void setTreeIndex(size_t t) {treeIndex_=t;}
    bool isTask() const {return type_ ==  TaskType;}
    void add(unsigned char status,unsigned int time);
    int firstInPeriod(QDateTime startDt,QDateTime endDt) const;
    bool hasSubmittedOrActiveDuration(QDateTime startDt,QDateTime endDt) const;
    int firstSubmittedDuration(QDateTime startDt,QDateTime endDt) const;
    int firstActiveDuration(QDateTime startDt,QDateTime endDt,unsigned int tlEndTime) const;
    void meanSubmittedDuration(float&,int&,unsigned int tlEndTime) const;
    void meanActiveDuration(float&,int&,unsigned int tlEndTime) const;
    void durationStats(unsigned char statusId,int& num,float& mean, TimelineItemStats& stats,unsigned int tlEndTime) const;
    void days(std::vector<unsigned int>&) const;

    static unsigned int fromQDateTime(QDateTime dt)
          {return dt.toMSecsSinceEpoch()/1000;}

    static QDateTime toQDateTime(unsigned int t)  {
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
        return QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(t)*1000,Qt::UTC);
#else
        return QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(t)*1000).toUTC();
#endif
    }


//protected:
    std::string path_;
    Type type_;
    size_t sortIndex_;
    size_t treeIndex_;
    size_t parentIndex_;
    std::vector<unsigned int> start_;
    std::vector<unsigned char> status_;

};

class TimelineData : public QObject
{
    Q_OBJECT
public:
    enum LoadStatus {LoadNotTried,LoadFailed,LoadDone};

    TimelineData(QObject* parent=0) : QObject(parent),
        startTime_(0), endTime_(0), maxReadSize_(0), fullRead_(false), loadStatus_(LoadNotTried) {}

    void loadLogFile(const std::string& logFile,size_t maxReadSize,const std::vector<std::string>& suites);
    void loadMultiLogFile(const std::string& logFile,const std::vector<std::string>& suites,int logFileIndex, bool last);

    QDateTime loadedAt() const {return loadedAt_;}
    size_t size() const {return  items_.size();}
    const std::vector<TimelineItem>& items() const {return items_;}
    unsigned int startTime() const {return startTime_;}
    unsigned int endTime() const {return endTime_;}
    QDateTime qStartTime() const {return TimelineItem::toQDateTime(startTime_);}
    QDateTime qEndTime() const {return TimelineItem::toQDateTime(endTime_);}
    void clear();    
    void setItemType(int index,TimelineItem::Type type);
    void setItemTreeIndex(size_t index,size_t treeIndex);
    bool isFullRead() const {return fullRead_;}
    LoadStatus loadStatus() const {return loadStatus_;}
    void markAsLoadDone() {loadStatus_ = LoadDone;}
    bool indexOfItem(const std::string&,size_t&);
    const std::vector<size_t>& sortIndex() const {return sortIndex_;}

    static bool parseLine(const std::string& line,std::string& name,
                          unsigned char& statusId,unsigned int& statusTime);

Q_SIGNALS:
    void loadProgress(size_t current,size_t total);

protected:
    void loadLogFileCore(const std::string& logFile,size_t maxReadSize,const std::vector<std::string>& suites, bool multi);
    void guessNodeType();
    TimelineItem::Type guessNodeType(const std::string& line,const std::string& name) const;
    TimelineItem::Type guessNodeType(const std::string& line) const;
    void sortByPath();

    std::vector<TimelineItem> items_;
    int numOfRows_;
    unsigned int startTime_;
    unsigned int endTime_;
    QDateTime loadedAt_;
    size_t maxReadSize_;
    bool fullRead_;
    LoadStatus loadStatus_;
    QHash<QString,size_t> pathHash_;
    std::vector<size_t> sortIndex_;
};

#endif // TIMELINEDATA_HPP
