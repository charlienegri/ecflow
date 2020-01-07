//============================================================================
// Copyright 2009-2020 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "TriggerCollector.hpp"

#include "UiLog.hpp"
#include "VAttribute.hpp"
#include "VItem.hpp"
#include "VItemPathParser.hpp"
#include "VNode.hpp"

#include <algorithm>

#define _UI_TRIGGERCOLLECTOR_DEBUG

TriggerListCollector::~TriggerListCollector()
{
    clear();
}

bool TriggerListCollector::add(VItem* t, VItem* dep,Mode mode)
{
    TriggerListItem *item=new TriggerListItem(t,dep,mode) ;
    items_.push_back(item);
    return true;

#if 0
    if(dep)
    {
        UiLog().dbg() << " dep=" << dep->typeName() << " " +  dep->strName();
        UiLog().dbg() << "    =" << item->dep_->typeName() << " " <<  item->dep_->strName());
    }
#endif
}

void TriggerListCollector::setDependency(bool b)
{
    extended_=b;
    clear();
}

void TriggerListCollector::clear()
{
    /*for(size_t i=0; i < items_.size(); i++)
    {
        delete items_[i];
    }*/
    items_.clear();
}


bool TriggerChildCollector::add(VItem* t, VItem*,Mode)
{
    if(!t->isAncestor(node_))
    {
        // child_ is a kid of node_ whose trigger is outside its subtree
        return collector_->add(t,child_,TriggerCollector::Child);
    }
    return false;
}

bool TriggerParentCollector::add(VItem* t, VItem*,Mode)
{
    return collector_->add(t,parent_,TriggerCollector::Parent);
}

bool TriggeredCollector::add(VItem* trigger, VItem*,Mode)
{
    if(VNode *n=trigger->isNode())
    {
        n->addTriggeredData(node_);
    }

    else if(VAttribute* a=trigger->isAttribute())
    {
        trigger->parent()->addTriggeredData(node_,a);

//        if(a->type() && a->type()->name() == "event")
//        {
//            trigger->parent()->addTriggeredData(node_,a);
//        }
    }
    return false;
}

const std::set<TriggerCollector::Mode>& TriggerTableItem::modes() const
{
    if(modes_.empty())
    {
        for(std::size_t i=0; i < deps_.size(); i++)
        {
            modes_.insert(deps_[i].mode());
        }
    }
    return modes_;
}

//=====================================
// TriggerTableCollector
//=====================================

TriggerTableCollector::~TriggerTableCollector()
{
    clear();
}

bool TriggerTableCollector::add(VItem* trigger, VItem* dep,Mode mode)
{
    Q_ASSERT(trigger);

    TriggerTableItem *item=0;
    for(std::size_t i=0; i < items_.size(); i++)
    {
        if(items_[i]->item() == trigger)
        {          
            item=items_[i];
            break;
        }
    }

    if(!item)
    {        
        item=new TriggerTableItem(trigger);
        items_.push_back(item);
    }

    item->addDependency(dep,mode);
    return true;
}

void TriggerTableCollector::setDependency(bool b)
{
    extended_=b;
    clear();
}

void TriggerTableCollector::clear()
{
    for(size_t i=0; i < items_.size(); i++)
    {
        delete items_[i];
    }
    items_.clear();
}

bool TriggerTableCollector::contains(TriggerTableItem* item) const
{
    return (std::find(items_.begin(),items_.end(), item) != items_.end());
}

bool TriggerTableCollector::contains(const VNode* node,bool attrParents) const
{
    for(size_t i=0; i < items_.size(); i++)
    {
        if(VItem* it=items_[i]->item())
        {
            if(VNode *n=it->isNode())
            {
                if(n == node)
                    return true;
            }
            else if(attrParents)
            {
                if (VAttribute *a=it->isAttribute())
                    if(a->parent() == node)
                        return true;
            }
        }

    }

    return false;
}

TriggerTableItem* TriggerTableCollector::find(const VItem* item) const
{
    for(size_t i=0; i < items_.size(); i++)
    {
        if(items_[i]->item() == item)
            return items_[i];
    }
    return 0;
}

TriggerTableItem* TriggerTableCollector::findByContents(const VItem* item) const
{
    if(!item)
        return 0;

    for(size_t i=0; i < items_.size(); i++)
    {
        if(item->sameContents(items_[i]->item()))
        {
            return items_[i];
        }
    }
    return 0;
}
