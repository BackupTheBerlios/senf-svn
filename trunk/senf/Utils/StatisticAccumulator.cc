// $Id: StatisticAccumulator.cc 1795 2011-07-14 08:42:24Z pug $
//
// Copyright (C) 2009
// Fraunhofer Institute for Open Communication Systems (FOKUS)
//
// The contents of this file are subject to the Fraunhofer FOKUS Public License
// Version 1.0 (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// http://senf.berlios.de/license.html
//
// The Fraunhofer FOKUS Public License Version 1.0 is based on,
// but modifies the Mozilla Public License Version 1.1.
// See the full license text for the amendments.
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the License.
//
// The Original Code is Fraunhofer FOKUS code.
//
// The Initial Developer of the Original Code is Fraunhofer-Gesellschaft e.V.
// (registered association), Hansastraße 27 c, 80686 Munich, Germany.
// All Rights Reserved.
//
// Contributor(s):
//   Stefan Bund <g0dil@berlios.de>

#include "StatisticAccumulator.hh"

std::ostream & senf::operator<<(std::ostream & os, senf::StatisticsData const & _data)
 {
         format::IndentHelper indent;
         os << indent << "StatisticData:" << std::endl;
         os << indent << "min: "    << _data.min << std::endl;
         os << indent << "avg: "    << _data.avg << std::endl;
         os << indent << "max: "    << _data.max << std::endl;
         os << indent << "stddev: " << _data.stddev << std::endl;
         os << indent << "count: "  << _data.count << std::endl;
         return os;
 };