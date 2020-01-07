//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "TimelineWidget.hpp"

#include <QtGlobal>
#include <QFileInfo>
#include <QTime>
#include <QButtonGroup>
#include <QFileDialog>

#include "IconProvider.hpp"
#include "FileInfoLabel.hpp"
#include "MainWindow.hpp"
#include "ServerHandler.hpp"
#include "SuiteFilter.hpp"
#include "TimelineData.hpp"
#include "TimelineModel.hpp"
#include "TimelinePreLoadDialog.hpp"
#include "TimelineView.hpp"
#include "TextFormat.hpp"
#include "UiLog.hpp"
#include "UIDebug.hpp"
#include "ViewerUtil.hpp"
#include "VFileInfo.hpp"
#include "VFileTransfer.hpp"
#include "VNode.hpp"
#include "VSettings.hpp"

#include "ui_TimelineWidget.h"


//=======================================================
//
// TimelineWidget
//
//=======================================================

TimelineWidget::TimelineWidget(QWidget *parent) :
    ui_(new Ui::TimelineWidget),
    maxReadSize_(100*1024*1024),
    logMode_(LatestMode),
    data_(0),
    filterTriggeredByEnter_(false),
    filterTriggerLimit_(200000),
    ignoreTimeEdited_(false),
    beingCleared_(false),
    typesDetermined_(false),
    treeOrderDetermined_(false),
    localLog_(true),
    logLoaded_(false),
    logTransferred_(false),
    fileTransfer_(0),
    detached_(false)
{
    ui_->setupUi(this);

    //message label
    ui_->messageLabel->hide();

    connect(ui_->messageLabel,SIGNAL(cancelLoad()),
            this,SLOT(slotCancelFileTransfer()));

    data_=new TimelineData(this);

    connect(data_,SIGNAL(loadProgress(size_t,size_t)),
            this,SLOT(slotLogLoadProgress(size_t,size_t)));

    //the models
    model_=new TimelineModel(this);
    sortModel_=new TimelineSortModel(model_,this);

    model_->setData(data_);

    view_=new TimelineView(sortModel_,this);

    ui_->viewHolderLayout->addWidget(view_);

    //ui_->view->setModel(model_);

    //make the horizontal scrollbar work
    //ui_->logView->header()->setStretchLastSection(false);
    //ui_->logView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    //Define context menu
    //ui_->logView->addAction(actionCopyEntry_);
    //ui_->logView->addAction(actionCopyRow_);

    //logInfo label
    ui_->logInfoLabel->setProperty("fileInfo","1");
    ui_->logInfoLabel->setWordWrap(true);
    ui_->logInfoLabel->setMargin(2);
    ui_->logInfoLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    ui_->logInfoLabel->setAutoFillBackground(true);
    ui_->logInfoLabel->setFrameShape(QFrame::StyledPanel);
    ui_->logInfoLabel->setTextInteractionFlags(Qt::LinksAccessibleByMouse|Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);

    // log mode
    ui_->logModeCombo->addItem("Current log","latest");
    ui_->logModeCombo->addItem("Archived log","archive");
    ui_->logModeCombo->setCurrentIndex(0);
    ui_->loadFileTb->hide();

    // view mode
    QButtonGroup *viewModeGr = new QButtonGroup(this);
    viewModeGr->addButton(ui_->timelineViewTb,0);
    viewModeGr->addButton(ui_->durationViewTb,1);
    viewModeGr->setExclusive(true);
    ui_->timelineViewTb->setChecked(true);

    ui_->pathFilterMatchModeCb->setMatchMode(StringMatchMode(StringMatchMode::WildcardMatch));  // set the default match mode

#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    ui_->pathFilterLe->setPlaceholderText(tr("Filter"));
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    ui_->pathFilterLe->setClearButtonEnabled(true);
#endif

    // sorting
    ui_->sortCombo->addItem("Sort by path","path");
    ui_->sortCombo->addItem("Sort by time","time");
    ui_->sortCombo->addItem("Sort by tree","tree");
    ui_->sortCombo->setCurrentIndex(0);

    QButtonGroup *sortGr = new QButtonGroup(this);
    sortGr->addButton(ui_->sortUpTb,0);
    sortGr->addButton(ui_->sortDownTb,1);
    sortGr->setExclusive(true);
    ui_->sortUpTb->setChecked(true);

    ui_->taskOnlyTb->setChecked(false);
    ui_->showChangedTb->setChecked(true);

    ui_->fromTimeEdit->setDisplayFormat("hh:mm:ss dd-MMM-yyyy");
    ui_->toTimeEdit->setDisplayFormat("hh:mm:ss dd-MMM-yyyy");

    ui_->zoomInTb->setDefaultAction(ui_->actionZoomIn);
    ui_->zoomOutTb->setDefaultAction(ui_->actionZoomOut);

    view_->setZoomActions(ui_->actionZoomIn,ui_->actionZoomOut);

    // signal-slot
    connect(ui_->logModeCombo,SIGNAL(currentIndexChanged(int)),
            this,SLOT(slotLogMode(int)));

    connect(viewModeGr,SIGNAL(buttonClicked(int)),
            this,SLOT(slotViewMode(int)));

    connect(ui_->pathFilterMatchModeCb,SIGNAL(currentIndexChanged(int)),
        this, SLOT(pathFilterMatchModeChanged(int)));

    connect(ui_->pathFilterLe,SIGNAL(textChanged(QString)),
            this,SLOT(slotPathFilterChanged(QString)));

    connect(ui_->pathFilterLe,SIGNAL(editingFinished()),
            this,SLOT(slotPathFilterEditFinished()));

    connect(ui_->subTreeTb,SIGNAL(toggled(bool)),
            this,SLOT(slotSubTree(bool)));

    connect(ui_->taskOnlyTb,SIGNAL(toggled(bool)),
            this,SLOT(slotTaskOnly(bool)));

    connect(ui_->sortCombo,SIGNAL(currentIndexChanged(int)),
            this,SLOT(slotSortMode(int)));

    connect(sortGr,SIGNAL(buttonClicked(int)),
            this,SLOT(slotSortOrderChanged(int)));

    connect(ui_->showChangedTb,SIGNAL(toggled(bool)),
            this,SLOT(slotShowChanged(bool)));

    connect(ui_->fromTimeEdit,SIGNAL(dateTimeChanged(QDateTime)),
            this,SLOT(slotStartChanged(QDateTime)));

    connect(ui_->toTimeEdit,SIGNAL(dateTimeChanged(QDateTime)),
            this,SLOT(slotEndChanged(QDateTime)));

    connect(ui_->startTb,SIGNAL(clicked()),
            this,SLOT(slotResetStart()));

    connect(ui_->endTb,SIGNAL(clicked()),
            this,SLOT(slotResetEnd()));

    connect(ui_->reloadTb,SIGNAL(clicked()),
            this,SLOT(slotReload()));

    connect(ui_->wholePeriodTb,SIGNAL(clicked()),
            this,SLOT(slotWholePeriod()));

    connect(view_,SIGNAL(periodSelected(QDateTime,QDateTime)),
            this,SLOT(slotPeriodSelectedInView(QDateTime,QDateTime)));

    connect(view_,SIGNAL(periodBeingZoomed(QDateTime,QDateTime)),
            this,SLOT(slotPeriodBeingZoomedInView(QDateTime,QDateTime)));

    connect(view_,SIGNAL(lookupRequested(QString)),
            this,SLOT(slotLookup(QString)));

    connect(view_,SIGNAL(copyPathRequested(QString)),
            this,SLOT(slotCopyPath(QString)));

    connect(ui_->loadFileTb,SIGNAL(clicked()),
            this,SLOT(slotLoadCustomFile()));

    //forced init
    slotSortMode(0);
    slotSortOrderChanged(0);
    slotTaskOnly(false);
    slotShowChanged(true);
}

TimelineWidget::~TimelineWidget()
{
    if(data_)
        delete data_;
}

// a complete clear
void TimelineWidget::clear()
{   
    beingCleared_=true;
    if(fileTransfer_)
    {
        fileTransfer_->stopTransfer();
        ui_->messageLabel->stopLoadLabel();
    }

    ui_->messageLabel->clear();
    ui_->messageLabel->hide();

    ui_->logInfoLabel->setText(QString());
    //viewHandler_->clear();

    model_->clearData();
    data_->clear();
    logFile_.clear();
    serverName_.clear();
    host_.clear();
    port_.clear();
    suites_.clear();
    remoteUid_.clear();
    currentNodePath_.clear();
    typesDetermined_=false;
    treeOrderDetermined_=false;
    localLog_=true;
    logLoaded_=false;
    logTransferred_=false;
    transferredAt_=QDateTime();

    ui_->pathFilterLe->clear();
    prevState_.valid=false;

    //reset the view mode to timeline
    ui_->timelineViewTb->setChecked(true);

    tmpLogFile_.reset();
    //ui_->startTe->clear();
    //ui_->endTe->clear();

    //reset the log mode to LatestMode
    ui_->logModeCombo->setCurrentIndex(0);
    logMode_ = LatestMode;
    ui_->reloadTb->show();
    ui_->loadFileTb->hide();
    archiveLogList_.clear();
    ui_->logModeCombo->hide();

    setAllVisible(false);
    updateFilterTriggerMode();

    beingCleared_=false;
}

void TimelineWidget::clearData(bool usePrevState)
{
    beingCleared_=true;
    if(fileTransfer_)
    {
        fileTransfer_->stopTransfer();
        ui_->messageLabel->stopLoadLabel();
    }

    ui_->messageLabel->clear();
    ui_->messageLabel->hide();
    ui_->logInfoLabel->setText(QString());

    model_->clearData();
    data_->clear();
    typesDetermined_=false;
    treeOrderDetermined_=false;
    localLog_=true;
    logLoaded_=false;
    logTransferred_=false;
    transferredAt_=QDateTime();

    if (!usePrevState)
    {
        ui_->pathFilterLe->clear();
        prevState_.valid=false;
        //reset the view mode to timeline
        ui_->timelineViewTb->setChecked(true);
        //ui_->startTe->clear();
        //ui_->endTe->clear();
    }

    tmpLogFile_.reset();

    setAllVisible(false);
    updateFilterTriggerMode();

    beingCleared_=false;
}

void TimelineWidget::updateInfoLabel(bool showDetails)
{
    QColor col(39,49,101);
    QString txt;

    if(logMode_ == LatestMode)
    {
        txt= Viewer::formatBoldText("Log file: ",col) + logFile_;

        txt+=Viewer::formatBoldText(" Server: ",col) + serverName_ +
             Viewer::formatBoldText(" Host: ",col) + host_ +
             Viewer::formatBoldText(" Port: ",col) + port_;

        if(showDetails)
        {
            if(data_->loadStatus() == TimelineData::LoadDone)
            {
                if(localLog_)
                {
                    VFileInfo fi(logFile_);
                    txt+=Viewer::formatBoldText(" Size: ",col) + fi.formatSize();
                }
                else
                {
                    VFileInfo fi(QString::fromStdString(tmpLogFile_->path()));
                    txt+=Viewer::formatBoldText(" Size: ",col) + fi.formatSize();
                }
            }

            txt+=Viewer::formatBoldText(" Source: ",col);

            //fetch method and time
            if(localLog_)
            {
                txt+=" read from disk ";
                if(data_->loadedAt().isValid())
                    txt+=Viewer::formatBoldText(" at ",col) + FileInfoLabel::formatDate(data_->loadedAt());
            }
            else
            {
                if(logTransferred_)
                {
                    txt+=" fetched from remote host ";
                }
                else
                {
                    txt+=" fetch failed from remote host ";
                }
                if(transferredAt_.isValid())
                    txt+=Viewer::formatBoldText(" at ",col) + FileInfoLabel::formatDate(transferredAt_);
            }

            if(data_->loadStatus() == TimelineData::LoadDone)
            {
                QColor warnCol(218,142,18);

                QString warnVerb;
                if(localLog_ && !data_->isFullRead())
                {
                    warnVerb="parsed";
                }
                else if(tmpLogFile_)
                {
                    QFileInfo fi(QString::fromStdString(tmpLogFile_->path()));
                    if(maxReadSize_ > 0 && static_cast<size_t>(fi.size()) == maxReadSize_)
                    {
                        warnVerb="fetched";
                    }
                }

                if(!warnVerb.isEmpty())
                {
                    txt+=Viewer::formatItalicText(" (" + warnVerb + " last " + QString::number(maxReadSize_/(1024*1024)) +
                                        " MB of file - maximum size reached)",warnCol);
                }
            }
        }
    }

    //archived
    else
    {
        if(archiveLogList_.loadableCount() == 1)
        {
           txt= Viewer::formatBoldText("Log file: ",col) + archiveLogList_.firstLoadablePath();
        }
        else if(archiveLogList_.loadableCount() > 1)
        {
           txt= Viewer::formatBoldText("Log files: ",col) + "multiple ["+
                   QString::number(archiveLogList_.loadableCount())  + "]";
        }

        txt+=Viewer::formatBoldText(" Server: ",col) + serverName_ +
            Viewer::formatBoldText(" Host: ",col) + host_ +
            Viewer::formatBoldText(" Port: ",col) + port_;

        if(showDetails)
        {
            if(data_->loadStatus() == TimelineData::LoadDone)
            {
                if(archiveLogList_.loadableCount() == 1)
                    txt+=Viewer::formatBoldText(" Size: ",col) + VFileInfo::formatSize(archiveLogList_.totalSize());
                else if (archiveLogList_.loadableCount() > 1)
                    txt+=Viewer::formatBoldText(" Total size: ",col) + VFileInfo::formatSize(archiveLogList_.totalSize());
            }

            txt+=Viewer::formatBoldText(" Source: ",col);
            txt+=" read from disk ";         
        }

    }

    ui_->logInfoLabel->setText(txt);

    checkButtonState();
}

void TimelineWidget::updateFilterTriggerMode()
{
    filterTriggeredByEnter_=(data_ && data_->size() > filterTriggerLimit_);
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    QString s=tr("Filter");
    if(filterTriggeredByEnter_)
    {
        s+=tr(" (hit Enter to run)");
    }
    ui_->pathFilterLe->setPlaceholderText(s);
#endif
}

void TimelineWidget::setAllVisible(bool b)
{
    ui_->viewControl->setVisible(b);
    view_->setVisible(b);
}

void TimelineWidget::slotPeriodSelectedInView(QDateTime start,QDateTime end)
{
    Q_ASSERT(data_);
    if(start >= data_->qStartTime() && end <= data_->qEndTime())
    {
        ignoreTimeEdited_=true;
        ui_->fromTimeEdit->setDateTime(start);
        ui_->toTimeEdit->setDateTime(end);
        ignoreTimeEdited_=false;
    }
    checkButtonState();
}

void TimelineWidget::slotPeriodBeingZoomedInView(QDateTime start,QDateTime end)
{
    Q_ASSERT(data_);
    if(start >= data_->qStartTime() && end <= data_->qEndTime())
    {
        ignoreTimeEdited_=true;
        ui_->fromTimeEdit->setDateTime(start);
        ui_->toTimeEdit->setDateTime(end);
        ignoreTimeEdited_=false;
    }
    //checkButtonState();
}


void TimelineWidget::slotLogMode(int)
{
    if(beingCleared_)
        return;

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    QString id=ui_->logModeCombo->currentData().toString();
#else
    QString id;
    if(ui_->logModeCombo->currentIndex() >=0)
        id=ui_->logModeCombo->itemData(ui_->logModeCombo->currentIndex()).toString();
#endif
    if(id == "latest")
        setLogMode(LatestMode);
    else if(id == "archive")
        setLogMode(ArchiveMode);
}

void TimelineWidget::slotViewMode(int)
{
    bool timelineMode=ui_->timelineViewTb->isChecked();

    //timeline
    if(timelineMode)
    {
        view_->setViewMode(TimelineView::TimelineMode);
        ui_->sortCombo->setEnabled(true);
        ui_->sortUpTb->setEnabled(true);
        ui_->sortDownTb->setEnabled(true);

        //reload filter
        slotShowChanged(ui_->showChangedTb->isChecked());

        //reset and reload the sort
        slotSortMode(0);
        slotSortOrderChanged(0);

    }
    //duration
    else
    {
        view_->setViewMode(TimelineView::DurationMode);
        ui_->sortCombo->setEnabled(false);
        ui_->sortUpTb->setEnabled(false);
        ui_->sortDownTb->setEnabled(false);

        //reload filter
        slotShowChanged(ui_->showChangedTb->isChecked());

        //reload sort
        sortModel_->setSortMode(TimelineSortModel::QtSortMode);
        slotShowChanged(ui_->showChangedTb->isChecked());
    }
}

void TimelineWidget::slotSubTree(bool b)
{
    if(b)
    {
        sortModel_->setRootNodeFilter(currentNodePath_);
    }
    else
    {
        sortModel_->setRootNodeFilter(QString());
    }
}

void TimelineWidget::slotTaskOnly(bool taskFilter)
{
    if(taskFilter)
    {
        determineNodeTypes();
    }

    sortModel_->setTaskFilter(taskFilter);
}

void TimelineWidget::pathFilterMatchModeChanged(int)
{
     sortModel_->setPathMatchMode(ui_->pathFilterMatchModeCb->currentMatchMode());
}

void TimelineWidget::slotPathFilterChanged(QString pattern)
{
    if(!filterTriggeredByEnter_)
        sortModel_->setPathFilter(pattern);
}

void TimelineWidget::slotPathFilterEditFinished()
{
    if(filterTriggeredByEnter_)
        sortModel_->setPathFilter(ui_->pathFilterLe->text());
}

void TimelineWidget::slotSortMode(int)
{
    int idx=ui_->sortCombo->currentIndex();
    if(idx == 0)
        sortModel_->setSortMode(TimelineSortModel::PathSortMode);
    else if (idx == 1)
        sortModel_->setSortMode(TimelineSortModel::TimeSortMode);
    else if (idx == 2)
    {
        determineTreeOrder();
        sortModel_->setSortMode(TimelineSortModel::TreeSortMode);
    }
}

void TimelineWidget::slotSortOrderChanged(int)
{
    bool ascending=ui_->sortUpTb->isChecked();
    sortModel_->setSortDirection(ascending);
}

void TimelineWidget::slotShowChanged(bool st)
{
    if(view_->viewMode() == TimelineView::TimelineMode)
    {
        sortModel_->setChangeFilterMode(st?TimelineSortModel::TimelineChangeFilterMode:
                                           TimelineSortModel::NoChangeFilterMode);
    }
    else if(view_->viewMode() == TimelineView::DurationMode)
    {
        sortModel_->setChangeFilterMode(st?TimelineSortModel::DurationChangeFilterMode:
                                           TimelineSortModel::NoChangeFilterMode);
    }
}

void TimelineWidget::slotStartChanged(const QDateTime& dt)
{
    if(!ignoreTimeEdited_)
    {        
        view_->setStartDate(dt);
    }
    checkButtonState();
}

void TimelineWidget::slotEndChanged(const QDateTime& dt)
{
    if(!ignoreTimeEdited_)
    {
        view_->setEndDate(dt);
    }
    checkButtonState();
}

void TimelineWidget::slotResetStart()
{
    ui_->fromTimeEdit->setDateTime(data_->qStartTime());
    checkButtonState();
}

void TimelineWidget::slotResetEnd()
{
    ui_->toTimeEdit->setDateTime(data_->qEndTime());
    checkButtonState();
}

void TimelineWidget::slotWholePeriod()
{
    Q_ASSERT(data_);
    ignoreTimeEdited_=true;
    ui_->fromTimeEdit->setDateTime(data_->qStartTime());
    ui_->toTimeEdit->setDateTime(data_->qEndTime());
    ignoreTimeEdited_=false;
    view_->setPeriod(data_->qStartTime(),data_->qEndTime());
    checkButtonState();
}

void TimelineWidget::slotLookup(QString nodePath)
{
    if(ServerHandler *sh=ServerHandler::find(serverName_.toStdString()))
    {
        VInfo_ptr ni=VInfo::createFromPath(sh,nodePath.toStdString());
        if(ni)
        {
            MainWindow::lookUpInTree(ni);
        }
    }
}

void TimelineWidget::slotCopyPath(QString nodePath)
{
    ViewerUtil::toClipboard(serverName_ + ":/" + nodePath);
}

void TimelineWidget::selectPathInView(const std::string& p)
{
    currentNodePath_=QString::fromStdString(p);

    if(ui_->subTreeTb->isChecked())
    {
        sortModel_->setRootNodeFilter(currentNodePath_);
    }

    QModelIndex idx=sortModel_->mapToSource(view_->currentIndex());
    if(idx.isValid() && idx.row() < static_cast<int>(data_->size()))
    {
        if(data_->items()[idx.row()].path() == p)
            return;
    }

    size_t pos=0;
    if(data_->indexOfItem(p,pos))
    {
        QModelIndex idx=sortModel_->mapFromSource(model_->index(pos,0));
        view_->setCurrentIndex(idx);
    }
}

void TimelineWidget::checkButtonState()
{
     bool fromStart=(ui_->fromTimeEdit->dateTime() == data_->qStartTime());
     bool toEnd=(ui_->toTimeEdit->dateTime() == data_->qEndTime());

     ui_->startTb->setEnabled(!fromStart);
     ui_->endTb->setEnabled(!toEnd);
     ui_->wholePeriodTb->setEnabled(!fromStart || !toEnd);
}

void TimelineWidget::setLogMode(LogMode logMode)
{
    if(logMode_ == logMode)
        return;

    logMode_ = logMode;
    if (logMode_ == LatestMode)
    {
        ui_->reloadTb->show();
        ui_->loadFileTb->hide();
        archiveLogList_.clear();
        clearData(false);
        reloadLatest(false);
    }
    else if(logMode_ == ArchiveMode)
    {
        ui_->reloadTb->hide();
        ui_->loadFileTb->show();
        archiveLogList_.clear();
        clearData(false);
        updateInfoLabel();
        checkButtonState();
    }
    else
    {
        UI_ASSERT(0, "Invalid logMode=" << logMode);
    }
}

// The reload button was pressed in LatestMode
void TimelineWidget::slotReload()
{
    Q_ASSERT(logMode_ == LatestMode);
    reloadLatest(true);
}

// Reload the logfile in current mode.
// canUsePrevState: false when we come form ArchiveMode or initial load,
//                  true otherwise
void TimelineWidget::reloadLatest(bool usePrevState)
{
    Q_ASSERT(logMode_ == LatestMode);
    if(!serverName_.isEmpty())
    {
        std::vector<std::string> suites;
        //we get the latest state of the suites and the remote uid
        if(ServerHandler *sh=ServerHandler::find(serverName_.toStdString()))
        {
            remoteUid_ = sh->uidForServerLogTransfer();
            setMaxReadSize(sh->maxSizeForTimelineData());

            if(SuiteFilter* sf=sh->suiteFilter())
            {
                if(sf->isEnabled())
                    suites=sh->suiteFilter()->filter();
            }

            suites_ = suites;
        }

        loadLatest(usePrevState);
        checkButtonState();
    }
}

void TimelineWidget::slotLoadCustomFile()
{
    Q_ASSERT(logMode_ == ArchiveMode);
    QStringList fileNames = QFileDialog::getOpenFileNames(this);
    if(fileNames.isEmpty())
        return;

    TimelineFileList archiveLogList = TimelineFileList(fileNames);
    TimelinePreLoadDialog dialog;
    dialog.setModal(true);
    dialog.init(archiveLogList);
    if(dialog.exec() == QDialog::Accepted)
    {
        archiveLogList_ = archiveLogList;
        loadArchive();
    }
}

// Initial load
void TimelineWidget::initLoad(QString serverName, QString host, QString port, QString logFile,
                          const std::vector<std::string>& suites, QString remoteUid, int maxReadSize,
                          const std::string& currentNodePath, bool detached)
{
    if(logMode_ != LatestMode)
        return;

    // we need to be in a clean state
    clear();
    Q_ASSERT(logMode_ == LatestMode);

    setDetached(detached);

    // clear() hides it so we need to show it again
    ui_->logModeCombo->show();

    // these must be kept until we call clear()!!!!
    serverName_=serverName;
    host_=host;
    port_=port;
    logFile_=logFile;
    suites_=suites;
    remoteUid_=remoteUid;
    setMaxReadSize(maxReadSize);
    currentNodePath_=QString::fromStdString(currentNodePath);

    loadLatest(false);
}

void TimelineWidget::loadLatest(bool usePrevState)
{
    Q_ASSERT(logMode_ == LatestMode);

    //if it is a reload we remember the current period
    if(usePrevState)
    {
        prevState_.valid=true;
        prevState_.startDt=ui_->fromTimeEdit->dateTime();
        prevState_.endDt=ui_->toTimeEdit->dateTime();
        prevState_.fullStart=(prevState_.startDt == data_->qStartTime());
        prevState_.fullEnd=(prevState_.endDt == data_->qEndTime());
        clearData(true);
    }
    else
    {
        prevState_.valid=false;
        clearData(false);
    }

    if(logFile_.isEmpty())
       return;

    logLoaded_=false;
    logTransferred_=false;
    localLog_=true;

    // at this point the file info label can only show some basic information
    // we do not have the details
    updateInfoLabel(false);

    QFileInfo fInfo(logFile_);

    //try to get it over the network, ascynchronous - will call loadCore() in the end
    if(!fInfo.exists())
    {
        localLog_=false;
        tmpLogFile_=VFile::createTmpFile(true); //will be deleted automatically
        ui_->messageLabel->showInfo("Fetching file from remote host ...");
        ui_->messageLabel->startLoadLabel(true);

        if(!fileTransfer_)
        {
            fileTransfer_=new VFileTransfer(this);

            connect(fileTransfer_,SIGNAL(transferFinished()),
                    this,SLOT(slotFileTransferFinished()));

            connect(fileTransfer_,SIGNAL(transferFailed(QString)),
                    this,SLOT(slotFileTransferFailed(QString)));

            connect(fileTransfer_,SIGNAL(stdOutputAvailable(QString)),
                    this,SLOT(slotFileTransferStdOutput(QString)));
        }

        fileTransfer_->transfer(logFile_,host_,QString::fromStdString(tmpLogFile_->path()),
                                maxReadSize_,remoteUid_);
    }
    // load local file
    else
    {
        loadCore(logFile_);
    }
}


// Load a single/multiple logfiles in ArchiveMode
void TimelineWidget::loadArchive()
{
    Q_ASSERT(logMode_ == ArchiveMode);

    clearData(false);

    logLoaded_=false;
    logTransferred_=false;
    localLog_=true;

    ViewerUtil::setOverrideCursor(QCursor(Qt::WaitCursor));

    ui_->messageLabel->showInfo("Loading timeline data from log file ...");

    bool loadDone=false;

    QTime timer;
    timer.start();

    //std::vector<std::string> suites;
    for(int i=0; i < archiveLogList_.items().count(); i++)
    {
        if(!archiveLogList_.items()[i].loadable_)
            continue;

        ui_->messageLabel->showInfo("Loading timeline data from log file [" +
                                    QString::number(i+1) + "/" + QString::number(archiveLogList_.items().count()) +
                                    "] ...");

        ui_->messageLabel->startProgress(100);

        try
        {
            data_->loadMultiLogFile(archiveLogList_.items()[i].dataPath().toStdString(),suites_,
                                    i, (i == archiveLogList_.items().count()-1));

            loadDone=true;
        }

        catch(std::runtime_error e)
        {
            //logLoaded_=false;
            ui_->messageLabel->stopProgress();

            std::string errTxt(e.what());
#if 0

            QFileInfo fInfo(logFile);
            if(!fInfo.exists())
            {
                errTxt+=" The specified log file <b>does not exist</b> on disk!";
            }
            else if(!fInfo.isReadable())
            {
                errTxt+=" The specified log file is <b>not readable</b>!";
            }
            else if(!fInfo.isFile())
            {
                errTxt+=" The specified log file is <b>not a file</b>!";
            }

            ui_->messageLabel->showError(QString::fromStdString(errTxt));
            data_->clear();
            setAllVisible(false);
            updateInfoLabel();
            ViewerUtil::restoreOverrideCursor();
            return;
#endif
        }

        UiLog().dbg() << "Logfile parsed: " << timer.elapsed()/1000 << "s";

        ui_->messageLabel->stopProgress();
        //ui_->messageLabel->hide();

        //logLoaded_=true;
        //setAllVisible(true);
        //updateInfoLabel();
        //updateFilterTriggerMode();

        //determine node types if task filter is on
        //if(ui_->taskOnlyTb->isChecked())
        //    determineNodeTypes();
    }

    ui_->messageLabel->hide();

    ViewerUtil::restoreOverrideCursor();

    if(!loadDone)
    {
        ui_->messageLabel->showError(QString::fromStdString("Could not parse any of the specified log files!"));
        data_->clear();
        setAllVisible(false);
        updateInfoLabel();
        return;
    }

    logLoaded_=true;
    data_->markAsLoadDone();
    setAllVisible(true);
    updateInfoLabel();
    updateFilterTriggerMode();

    initFromData();
}

void TimelineWidget::slotFileTransferFinished()
{ 
    //we are not in a cleared state
    if(!beingCleared_)
    {
        logTransferred_=true;
        ui_->messageLabel->stopLoadLabel();
        ui_->messageLabel->hide();
        ui_->messageLabel->update();
        loadCore(QString::fromStdString(tmpLogFile_->path()));
    }
}

void TimelineWidget::slotFileTransferFailed(QString err)
{
    if(!beingCleared_)
    {
        logTransferred_=false;
        ui_->messageLabel->stopLoadLabel();
        logLoaded_=false;
        ui_->messageLabel->showError("Could not fetch log file from remote host! <br>" + err);
        data_->clear();
        setAllVisible(false);
        updateInfoLabel();
    }
}

void TimelineWidget::slotFileTransferStdOutput(QString msg)
{
    if(!msg.simplified().isEmpty())
    {
        ui_->messageLabel->appendInfo(msg);
    }
}


void TimelineWidget::slotLogLoadProgress(size_t current,size_t total)
{
    int percent=100*current/total;
    if(percent >=0 && percent <=100)
        ui_->messageLabel->progress("",percent);

}

void TimelineWidget::slotCancelFileTransfer()
{
    if(fileTransfer_ && fileTransfer_->isActive())
    {
        //ui_->messageLabel->stopLoad();
        fileTransfer_->stopTransfer();
    }
}

void TimelineWidget::loadCore(QString logFile)
{
    ViewerUtil::setOverrideCursor(QCursor(Qt::WaitCursor));

    ui_->messageLabel->showInfo("Loading timeline data from log file ...");

    QTime timer;
    timer.start();

    ui_->messageLabel->startProgress(100);

    try
    {
        data_->loadLogFile(logFile.toStdString(),maxReadSize_,suites_);
    }
    catch(std::runtime_error e)
    {
        logLoaded_=false;
        ui_->messageLabel->stopProgress();

        std::string errTxt(e.what());

        QFileInfo fInfo(logFile);
        if(!fInfo.exists())
        {
            errTxt+=" The specified log file <b>does not exist</b> on disk!";
        }
        else if(!fInfo.isReadable())
        {
            errTxt+=" The specified log file is <b>not readable</b>!";
        }
        else if(!fInfo.isFile())
        {
            errTxt+=" The specified log file is <b>not a file</b>!";
        }

        ui_->messageLabel->showError(QString::fromStdString(errTxt));
        data_->clear();
        setAllVisible(false);
        updateInfoLabel();
        ViewerUtil::restoreOverrideCursor();
        return;
    }

    UiLog().dbg() << "Logfile parsed: " << timer.elapsed()/1000 << "s";

    ui_->messageLabel->stopProgress();
    ui_->messageLabel->hide();
    logLoaded_=true;
    setAllVisible(true);
    updateInfoLabel();
    updateFilterTriggerMode();

    //determine node types if task filter is on
    if(ui_->taskOnlyTb->isChecked())
        determineNodeTypes();

    //determine tree order when in tree sort mode
    if(sortModel_->sortMode() == TimelineSortModel::TreeSortMode)
        determineTreeOrder();

    ViewerUtil::restoreOverrideCursor();

    initFromData();
}

void TimelineWidget::initFromData()
{
    //set the period
    ignoreTimeEdited_=true;

    ui_->fromTimeEdit->setMinimumDateTime(data_->qStartTime());
    ui_->fromTimeEdit->setMaximumDateTime(data_->qEndTime());

    //try the set the previously used interval - for reload only
    if(prevState_.valid)
    {
        if(prevState_.startDt <= data_->qStartTime() || prevState_.fullStart)
            ui_->fromTimeEdit->setDateTime(data_->qStartTime());
        else if(prevState_.startDt < data_->qEndTime())
            ui_->fromTimeEdit->setDateTime(prevState_.startDt);
    }
    else
    {
        ui_->fromTimeEdit->setDateTime(data_->qStartTime());
    }

    ui_->toTimeEdit->setMinimumDateTime(data_->qStartTime());
    ui_->toTimeEdit->setMaximumDateTime(data_->qEndTime());

    if(prevState_.valid)
    {
        if(prevState_.endDt >= data_->qEndTime() || prevState_.fullEnd)
            ui_->toTimeEdit->setDateTime(data_->qEndTime());
        else if(prevState_.endDt > data_->qStartTime())
            ui_->toTimeEdit->setDateTime(prevState_.endDt);
    }
    else
    {
        ui_->toTimeEdit->setDateTime(data_->qEndTime());
    }

    ignoreTimeEdited_=false;

    view_->setPeriod(ui_->fromTimeEdit->dateTime(),ui_->toTimeEdit->dateTime());

    model_->setData(data_);

    view_->setViewMode(view_->viewMode(),true);

    checkButtonState();
}

void TimelineWidget::setDetached(bool d)
{
    detached_=d;
}

//Determine missing types
void TimelineWidget::determineNodeTypes()
{
    if(typesDetermined_)
        return;

    if(ServerHandler *sh=ServerHandler::find(serverName_.toStdString()))
    {
        ViewerUtil::setOverrideCursor(QCursor(Qt::WaitCursor));

        for(size_t i=0; i < data_->items().size() ;i++)
        {
            if(data_->items()[i].type() == TimelineItem::UndeterminedType)
            {
                if(VNode *vn=sh->vRoot()->find(data_->items()[i].path()))
                {
                    if(vn->isTask())
                        data_->setItemType(i,TimelineItem::TaskType);
                    else if(vn->isFamily())
                        data_->setItemType(i,TimelineItem::FamilyType);
                }
            }
        }

        ViewerUtil::restoreOverrideCursor();
    }

    typesDetermined_=true;
}

//Determine missing types
void TimelineWidget::determineTreeOrder()
{
    if(treeOrderDetermined_)
        return;

    if(ServerHandler *sh=ServerHandler::find(serverName_.toStdString()))
    {
        ViewerUtil::setOverrideCursor(QCursor(Qt::WaitCursor));

        // treeIndex = 0 is the server!!!!
        const std::vector<VNode*>& nv = sh->vRoot()->nodes();
        for(size_t i=0; i < nv.size(); i++)
        {
            size_t dataIdx=0;
            if(data_->indexOfItem(nv[i]->absNodePath(), dataIdx))
                data_->setItemTreeIndex(dataIdx, i+1);
        }

        // Now for all the nodes in the tree we have the treeIndex_ set.
        // We deal with the rest of it (i.e. with nodes in the timeline data but
        // not appearing in the tree!
        int idx = nv.size()+1;

        // iterate through the timeline items alphabetically
        const std::vector<size_t>& sortIndex=data_->sortIndex();
        for(size_t i=0; i < sortIndex.size() ;i++)
        {
            int dataIdx=sortIndex[i];
            if(data_->items()[dataIdx].type() != TimelineItem::ServerType &&
               data_->items()[dataIdx].treeIndex() == 0)
            {
               data_->setItemTreeIndex(dataIdx, idx);
               idx++;
            }
        }

        ViewerUtil::restoreOverrideCursor();
    }

    treeOrderDetermined_=true;
}

void TimelineWidget::setMaxReadSize(int maxReadSizeInMb)
{
    if (maxReadSizeInMb <= 0)
        maxReadSize_ = 0;
    else
        maxReadSize_ = static_cast<size_t>(maxReadSizeInMb)*1024*1024;
}

void TimelineWidget::writeSettings(VComboSettings* vs)
{
    int cbIdx=ui_->sortCombo->currentIndex();
    if(cbIdx != -1)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
        vs->put("sortMode", ui_->sortCombo->currentData().toString().toStdString());
#else
        if(ui_->sortCombo->currentIndex() >=0)
        {
            vs->put("sortMode", ui_->sortCombo->itemData(ui_->sortCombo->currentIndex()).toString().toStdString());
        }
#endif
    }

    vs->put("pathFilterMatchMode",ui_->pathFilterMatchModeCb->currentIndex());

    vs->put("subTree",ui_->subTreeTb->isChecked());
    vs->put("sortOrder",ui_->sortUpTb->isChecked()?"asc":"desc");
    vs->put("taskOnly",ui_->taskOnlyTb->isChecked());
    vs->put("showChanged",ui_->showChangedTb->isChecked());

    view_->writeSettings(vs);
}

void TimelineWidget::readSettings(VComboSettings* vs)
{
    //at this point the model is empty so it is cheap to call sort

    int matchModeIdx=vs->get<int>("pathFilterMatchMode",1);
    StringMatchMode matchMode=ui_->pathFilterMatchModeCb->matchMode(matchModeIdx);
    if(matchMode.mode() != StringMatchMode::InvalidMatch)
    {
        ui_->pathFilterMatchModeCb->setMatchMode(matchMode);
    }

    //sort mode
    QString sortMode=QString::fromStdString(vs->get<std::string>("sortMode",std::string()));
    ViewerUtil::initComboBoxByData(sortMode,ui_->sortCombo);

    //sort order
    QString sortOrder=QString::fromStdString(vs->get<std::string>("sortOrder",std::string()));
    if(sortOrder == "asc")
    {
        ui_->sortUpTb->setChecked(true);
    }
    else if(sortOrder == "desc")
    {
        ui_->sortDownTb->setChecked(true);
    }

    ui_->subTreeTb->setChecked(vs->get<bool>("subTree",false));
    ui_->showChangedTb->setChecked(vs->get<bool>("showChanged",true));
    ui_->taskOnlyTb->setChecked(vs->get<bool>("taskOnly",false));

    view_->readSettings(vs);
}
