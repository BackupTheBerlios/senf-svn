// $Id$
//
// Copyright (C) 2008
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Christian Niephaus <cni@berlios.de>
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

// Definition of RadiotapPacket non-inline non-template functions

#include "RadiotapPacket.hh"
//#include "RadiotapPacket.ih"

// Custom includes
#include "WLANPacket.hh"
#include <boost/io/ios_state.hpp>
#include <memory.h>

extern "C" {
#   include "radiotap/radiotap_iter.h"
}

#define prefix_
//-/////////////////////////////////////////////////////////////////////////////////////////////////

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// Offset table management

prefix_ senf::RadiotapPacketParser::OffsetTable &
senf::RadiotapPacketParser::offsetTable(boost::uint32_t presentFlags)
{
    typedef std::map<boost::uint32_t, OffsetTable> OffsetMap;
    static OffsetMap offsetMap;

    OffsetMap::iterator i (offsetMap.find(presentFlags));
    if (i == offsetMap.end())
        i = offsetMap.insert(std::make_pair(presentFlags, OffsetTable())).first;
    return i->second;
}

prefix_ void senf::RadiotapPacketParser::parseOffsetTable(boost::uint8_t * data, int maxLength,
                                                          OffsetTable & table)
{
    struct ieee80211_radiotap_iterator iter;
    ieee80211_radiotap_iterator_init(&iter,
                                     (struct ieee80211_radiotap_header *)data,
                                     maxLength,
                                     0);
    unsigned size (8u);
    while (ieee80211_radiotap_iterator_next(&iter) == 0) {
        if (iter.is_radiotap_ns &&
            iter.this_arg_index <= int(RadiotapPacketParser::MAX_INDEX))
            table[iter.this_arg_index] = iter.this_arg - data;
        // We need to set size here in the loop since the iter fields are only valid
        // when at least one present bit is set ...
        size = iter.this_arg - data + iter.this_arg_size;
    }
    table[MAX_INDEX+1] = size;
}

prefix_ void senf::RadiotapPacketParser::buildOffsetTable(boost::uint32_t presentFlags,
                                                          OffsetTable & table)
{
    SENF_ASSERT(!(presentFlags & ( (1<<IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE) |
                                   (1<<IEEE80211_RADIOTAP_VENDOR_NAMESPACE) |
                                   (1<<IEEE80211_RADIOTAP_EXT) )),
                "Extended or vendor fields not supported");

    struct ieee80211_radiotap_header header;
    memset(&header, 0, sizeof(header));
    // header.it_version = 0;

    // Iterating this packet will generate invalid addresses but we don't care since neither
    // radiotap.c nor we will ever dereference those pointers, we just calculate the offsets.
    // This works, as long as we don't support extension headers ...
    header.it_len = 0xFFFF;
    header.it_present = presentFlags;

    parseOffsetTable((boost::uint8_t*)&header, header.it_len, table);
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::RadiotapPacketParser

unsigned const senf::RadiotapPacketParser_Header::FIELD_SIZE[] = {
    8, 1, 1, 4, 2, 1, 1, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1, 1 };

prefix_ senf::UInt32Parser senf::RadiotapPacketParser::init_fcs()
{
    if (!has_fcs()) {
        protect(), data().insert(data().end(), 4u, 0u);
        init_flags().fcsAtEnd_() = true;
    }
    return fcs();
}

prefix_ void senf::RadiotapPacketParser::disable_fcs()
{
    if (has_fcs()) {
        validate(RadiotapPacketParser_Header::fixed_bytes+4);
        data().erase(data().end()-4, data().end());
        flags().fcsAtEnd_() = false;
    }
}

prefix_ senf::RadiotapPacketParser::OffsetTable const &
senf::RadiotapPacketParser::getTable(boost::uint32_t presentFlags)
    const
{
    OffsetTable & table(offsetTable(presentFlags));
    if (! table[MAX_INDEX+1])
        buildOffsetTable(presentFlags, table);
    return table;
}

prefix_ void senf::RadiotapPacketParser::insertRemoveBytes(unsigned from , unsigned to, int bytes)
{
    data_iterator b (i() + from);
    data_iterator e (i() + to);
    if (bytes >= 0) {
        // Insert some bytes cleaning the old bytes to 0 first
        std::fill(b, e,  0u);
        if (bytes > 0)
            // need to protect the parser since data().insert() invalidates iterators
            protect(), data().insert(e, bytes, 0u);
    }
    else { // bytes < 0
        // Remove some bytes ...
        // remember: bytes is negative ...
        if (b < e + bytes)
            std::fill(b, e + bytes, 0u);
        data().erase(e + bytes, e);
    }
}

prefix_ void senf::RadiotapPacketParser::updatePresentFlags(boost::uint32_t flags)
{
    if (flags == presentFlags())
        return;
    validate(bytes());

    OffsetTable const & oldTable (currentTable());
    OffsetTable const & newTable (getTable(flags));
    unsigned b (RadiotapPacketParser_Header::fixed_bytes);
    int cumulativeNewBytes (0);

    for (unsigned index (0); index <= MAX_INDEX; ++index) {
        // Skip any unchanged fields
        for (; index <= MAX_INDEX+1
                 && ((oldTable[index] == 0 && newTable[index] == 0)
                     || (oldTable[index]+cumulativeNewBytes == newTable[index])); ++index)
            if (newTable[index] != 0)
                b = newTable[index] + FIELD_SIZE[index];
        if (index > MAX_INDEX+1)
            break;
        // Now skip over all changed fields
        // (The condition index <= MAX_INDEX is not needed here since the last
        // table entry MAX_INDEX+1 is always != 0 in both tables)
        for (; ! (oldTable[index]!=0 && newTable[index]!=0); ++index) ;
        // index now either points to
        // a) an entry set in both tables
        // b) at the end of the table which contains the total length
        // (remember: the table has a size of MAX_INDEX+2 entries !!)
        // in both cases, the difference between the new and old size
        // is found from the difference between the old and the new table
        // entry
        int newBytes (newTable[index] - oldTable[index] - cumulativeNewBytes);
        insertRemoveBytes(b, oldTable[index] + cumulativeNewBytes, newBytes);
        cumulativeNewBytes += newBytes;
        b = newTable[index] + FIELD_SIZE[index];
    }
    length() += cumulativeNewBytes;
    presentFlags() = flags;
    currentTable_ = &newTable;
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
// senf::RadiotapPacketType

prefix_ void senf::RadiotapPacketType::dump(packet p, std::ostream & os)
{
    boost::io::ios_all_saver ias(os);
    os << "Radiotap:\n"
       << senf::fieldName("version") << unsigned(p->version()) << '\n'
       << senf::fieldName("length")  << unsigned(p->length()) << '\n';

#   define FIELD(name, sign, desc)                                      \
        if (p->name ## Present())                                       \
            os << senf::fieldName(desc) << sign(p->name()) << '\n';

#   define ENTER(name)                                                  \
        if (p->name ## Present()) {                                     \
            packet::Parser::name ## _t subparser (p->name());

#   define SUBFIELD(name, sign, desc)                                   \
        os << senf::fieldName(desc) << sign(subparser.name()) << '\n';

#   define LEAVE()                                                      \
        }

#   define START_FLAGS(desc)                                            \
        os << senf::fieldName(desc);

#   define FLAG(name, desc)                                             \
        if (subparser.name()) os << desc " "

#   define END_FLAGS()                                                  \
        os << '\n';

    FIELD           ( tsft,              boost::uint64_t, "MAC timestamp"        );
    ENTER           ( flags                                                      );
      START_FLAGS   (                                     "flags"                );
        FLAG        (     shortGI,                            "ShortGI"          );
        FLAG        (     badFCS,                             "BadFCS"           );
        FLAG        (     fcsAtEnd,                           "FCSatEnd"         );
        FLAG        (     fragmentation,                      "Frag"             );
        FLAG        (     wep,                                "WEP"              );
        FLAG        (     shortPreamble,                      "ShortPreamble"    );
        FLAG        (     cfp,                                "CFP"              );
      END_FLAGS     (                                                            );
    LEAVE           (                                                            );
    FIELD           ( rate,              unsigned,        "rate"                 );
    ENTER           ( channelOptions                                             );
      SUBFIELD      (     freq,          unsigned,        "channel frequency"    );
      START_FLAGS   (                                     "channel flags"        );
        FLAG        (     flag2ghz,                           "2GHz"             );
        FLAG        (     ofdm,                               "OFDM"             );
        FLAG        (     cck,                                "CCK"              );
        FLAG        (     turbo,                              "Turbo"            );
        FLAG        (     quarterRateChannel,                 "Rate/4"           );
        FLAG        (     halfRateChannel,                    "Rate/2"           );
        FLAG        (     gsm,                                "GSM"              );
        FLAG        (     staticTurbo,                        "StaticTurbo"      );
        FLAG        (     gfsk,                               "GFSK"             );
        FLAG        (     cckOfdm,                            "CCK+OFDM"         );
        FLAG        (     passive,                            "Passive"          );
        FLAG        (     flag5ghz,                           "5GHz"             );
      END_FLAGS     (                                                            );
    LEAVE           (                                                            );
    FIELD           ( fhss,              unsigned,        "FHSS"                 );
    FIELD           ( dbmAntennaSignal,  signed,          "antenna signal (dBm)" );
    FIELD           ( dbmAntennaNoise,   signed,          "antenna noise (dBm)"  );
    FIELD           ( lockQuality,       unsigned,        "lock quality"         );
    FIELD           ( txAttenuation,     unsigned,        "tx attenuation"       );
    FIELD           ( dbTxAttenuation,   unsigned,        "tx attenuation (dB)"  );
    FIELD           ( dbmTxAttenuation,  signed,          "tx attenuation (dBm)" );
    FIELD           ( antenna,           unsigned,        "antenna"              );
    FIELD           ( dbAntennaSignal,   unsigned,        "antenna signal (dB)"  );
    FIELD           ( dbAntennaNoise,    unsigned,        "antenna noise (dB)"   );
    ENTER           ( rxFlags                                                    );
      START_FLAGS   (                                     "rx flags"             );
        FLAG        (     badPlcp,                            "BadPLCP"          );
      END_FLAGS     (                                                            );
    LEAVE           (                                                            );
    ENTER           ( txFlags                                                    );
      START_FLAGS   (                                     "tx flags"             );
        FLAG        (     fail,                               "Fail"             );
        FLAG        (     txRts,                              "RTS"              );
        FLAG        (     txCts,                              "CTS"              );
      END_FLAGS     (                                                            );
    LEAVE           (                                                            );
    FIELD           ( rtsRetries,        unsigned,        "rts retries"          );
    FIELD           ( dataRetries,       unsigned,        "data retries"         );

    if (p->flagsPresent() && p->flags().fcsAtEnd())
        os << senf::fieldName("fcs") << unsigned(p->fcs()) << '\n';

#   undef END_FLAGS
#   undef FLAG
#   undef START_FLAGS
#   undef LEAVE
#   undef SUBFIELD
#   undef ENTER
#   undef FIELD
}

prefix_ void senf::RadiotapPacketType::init(packet p)
{
    // ?? Why the heck do we need the +0? Otherwise we get an
    // 'undefined reference to 'RadiotapPacketParser_Header::fixed_bytes'
    p->length() << RadiotapPacketParser_Header::fixed_bytes+0;
}

prefix_ senf::PacketInterpreterBase::factory_t senf::RadiotapPacketType::nextPacketType(packet p)
{
    static factory_t frameTypeFactory[] = { WLANPacket_MgtFrame::factory(),
                                            WLANPacket_CtrlFrame::factory(),
                                            WLANPacket_DataFrame::factory(),
                                            no_factory() };
    return frameTypeFactory[p->frameType()];
}

prefix_ senf::RadiotapPacketType::optional_range
senf::RadiotapPacketType::nextPacketRange(packet p)
{
    parser rtParser (p.parser());
    size_type h (senf::bytes(rtParser));
    size_type t (rtParser.flagsPresent() && rtParser.flags().fcsAtEnd() ? 4 : 0);
    return p.size() <= h+t
        ? no_range()
        : optional_range( range(p.data().begin() + h, p.data().end() - t) );
}

//-/////////////////////////////////////////////////////////////////////////////////////////////////
#undef prefix_


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
