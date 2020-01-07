//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef OUTPUTITEMWIDGET_HPP_
#define OUTPUTITEMWIDGET_HPP_

#include "InfoPanelItem.hpp"

#include "VFile.hpp"
#include "VDir.hpp"

#include "ui_OutputItemWidget.h"

class OutputDirProvider;
class OutputFetchInfo;
class OutputModel;
class OutputSortModel;

class OutputItemWidget : public QWidget, public InfoPanelItem, protected Ui::OutputItemWidget
{
Q_OBJECT

public:
	explicit OutputItemWidget(QWidget *parent=0);
	~OutputItemWidget();

	void reload(VInfo_ptr);
	QWidget* realWidget();
	void clearContents();

	//From VInfoPresenter
	void infoReady(VReply*);
	void infoFailed(VReply*);
	void infoProgress(VReply*);
    void infoProgressStart(const std::string& text,int max);
    void infoProgress(const std::string& text,int value);

    void nodeChanged(const VNode*, const std::vector<ecf::Aspect::Type>&);
    void defsChanged(const std::vector<ecf::Aspect::Type>&) {}

protected Q_SLOTS:
	void slotOutputSelected(QModelIndex,QModelIndex);
	void slotUpdateDir();
	void on_searchTb__clicked();
	void on_gotoLineTb__clicked();
	void on_reloadTb__clicked();
	void on_fontSizeUpTb__clicked();
	void on_fontSizeDownTb__clicked();
    void on_saveFileAsTb__clicked();
    void on_copyPathTb__clicked();
    void on_dirReloadTb__clicked();

protected:
    void setCurrentInDir(const std::string&,VFile::FetchMode fetchMode);
    void updateDir(bool);
    void updateDir(const std::vector<VDir_ptr>&,bool);
	void enableDir(bool);
    void updateState(const FlagSet<ChangeFlag>&);
	void searchOnReload();
    void getCurrentFile(bool doReload);
	void getLatestFile();
    void currentDesc(std::string& fullName,VDir::FetchMode& fetchMode) const;
    void updateHistoryLabel(const std::vector<std::string>&);
    void displayDirErrors(const std::vector<std::string>& errorVec);

	OutputDirProvider* dirProvider_;
	OutputModel* dirModel_;
	OutputSortModel* dirSortModel_;

	bool userClickedReload_;
	bool ignoreOutputSelection_;
	QTimer* updateDirTimer_;
	static int updateDirTimeout_;
    OutputFetchInfo* fetchInfo_;
    bool dirColumnsAdjusted_;
    bool submittedWarning_;
};

#endif

