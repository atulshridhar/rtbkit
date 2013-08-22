/** utils.h                                 -*- C++ -*-
    Rémi Attab, 22 Aug 2013
    Copyright (c) 2013 Datacratic.  All rights reserved.

    Random utilities to write filter tests.

*/

#pragma once

#include "rtbkit/core/agent_configuration/include_exclude.h"
#include "rtbkit/common/exchange_connector.h"
#include "rtbkit/common/segments.h"
#include "rtbkit/common/filter.h"
#include "jml/utils/smart_ptr_utils.h"

#include <boost/test/unit_test.hpp>
#include <string>
#include <iostream>

namespace RTBKIT {


/******************************************************************************/
/* TITLE                                                                      */
/******************************************************************************/

void title(const std::string& title)
{
    std::string padding(80 - 4 - title.size(), '-');
    std::cerr << "[ " << title << " ]" << padding << std::endl;
}


/******************************************************************************/
/* CHECK                                                                      */
/******************************************************************************/

void check(
        const ConfigSet& configs,
        const std::initializer_list<size_t>& expected)
{
    ConfigSet ex;
    for (size_t cfg : expected) ex.set(cfg);

    ConfigSet diff = configs;
    diff ^= ex;

    if (diff.empty()) return;

    std::cerr << "val=" << configs.print() << std::endl
        << "exp=" << ex.print() << std::endl
        << "dif=" << diff.print() << std::endl;
    BOOST_CHECK(false);
}


/******************************************************************************/
/* IE                                                                         */
/******************************************************************************/

template<typename T, typename List = std::vector<T> >
IncludeExclude<T, List>
ie(     const std::initializer_list<T>& includes,
        const std::initializer_list<T>& excludes)
{
    IncludeExclude<T, List> ie;
    for (const auto& v : includes) ie.include.push_back(v);
    for (const auto& v : excludes) ie.exclude.push_back(v);
    return ie;
}

template<typename T, typename List = std::vector<T> >
IncludeExclude<T, List> ie()
{
    return IncludeExclude<T, List>();
}

/******************************************************************************/
/* SEGMENT                                                                    */
/******************************************************************************/

void segmentImpl(SegmentList& seg) {}

template<typename Arg>
void segmentImpl(SegmentList& seg, Arg&& arg)
{
    seg.add(std::forward<Arg>(arg));
}

template<typename Arg, typename... Args>
void segmentImpl(SegmentList& seg, Arg&& arg, Args&&... rest)
{
    segmentImpl(seg, std::forward<Arg>(arg));
    segmentImpl(seg, std::forward<Args>(rest)...);
}

template<typename... Args>
SegmentList segment(Args&&... args)
{
    SegmentList seg;
    segmentImpl(seg, std::forward<Args>(args)...);
    seg.sort();
    return seg;
}


/******************************************************************************/
/* ADD/REMOVE CONFIG                                                          */
/******************************************************************************/

void addConfig(FilterBase& filter, unsigned cfgIndex, AgentConfig& cfg)
{
    filter.addConfig(cfgIndex, ML::make_unowned_sp(cfg));
}

void removeConfig(FilterBase& filter, unsigned cfgIndex, AgentConfig& cfg)
{
    filter.removeConfig(cfgIndex, ML::make_unowned_sp(cfg));
}


/******************************************************************************/
/* FILTER EXCHANGE CONNECTOR                                                  */
/******************************************************************************/

struct FilterExchangeConnector : public ExchangeConnector
{
    FilterExchangeConnector(const std::string& name) :
        ExchangeConnector(name), name(name)
    {}

    std::string exchangeName() const { return name; }

    void configure(const Json::Value& parameters) {}
    void enableUntil(Date date) {}

private:
    std::string name;
};

} // namespace RTBKIT