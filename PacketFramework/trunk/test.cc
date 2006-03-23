// $Id$
//
// Copyright (C) 2005 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
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

// Definition of non-inline non-template functions

//#include "test.h"
//#include "test.ih"

// Custom includes
#include "managed_pointer.hh"
#include "deque_list.hh"

#define prefix__
///////////////////////////////cc.p////////////////////////////////////////

typedef satcom::containers::deque_list<unsigned char> bytearea;

int main (int argc, char** argv)
{
    bytearea area;
    bytearea::iterator i = area.begin();
    bytearea::iterator j = i;
    area.end();
    ++i;
    --i;
    *i = 0;
    bytearea::inserter ins = area.inserter_at (i);
    bytearea::inserter iins = ins;
    ins.size();
    ins.max_size();
    ins.block_begin();
    ins.block_end();
    *ins = 0;
    ++ins;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix__


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
