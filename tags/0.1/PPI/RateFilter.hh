// Copyright (C) 2007
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#ifndef RATEFILTER_HH_
#define RATEFILTER_HH_

// Custom includes
#include "Connectors.hh"
#include "Module.hh"
#include "IntervalTimer.hh"


namespace senf {
namespace ppi {
namespace module {

class RateFilter
    : public Module
{
    SENF_PPI_MODULE(RateFilter);
public:

    connector::ActiveInput<> input;
    connector::ActiveOutput<> output;

    RateFilter(senf::ClockService::clock_type interval);
//    void changeInterval(senf::ClockService::clock_type interval);   not yet implemented!

private:
    void timeout();
    ppi::IntervalTimer timer;
};

}}} //namespaces

#endif /*RATEFILTER_HH_*/
