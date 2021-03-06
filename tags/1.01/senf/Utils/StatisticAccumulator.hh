// $Id$
//
// Copyright (C) 2010
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Thorsten Horstmann <thorsten.horstmann@fokus.fraunhofer.de>
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

/** \file
    \brief StatisticAccumulator public header */

#ifndef HH_SENF_Utils_StatisticAccumulator_
#define HH_SENF_Utils_StatisticAccumulator_ 1

// Custom includes
#include <math.h>
#include <boost/cstdint.hpp>
///////////////////////////////hh.p////////////////////////////////////////
namespace senf {

     struct StatisticsData
     {
	 StatisticsData( float min_, float avg_, float max_, float stddev_, boost::uint32_t count_)
	     : min(min_), avg(avg_), max(max_), stddev(stddev_), count(count_){
	 };
	 StatisticsData( StatisticsData const & other)
	     : min(other.min), avg(other.avg), max(other.max), stddev(other.stddev), count(other.count){
	 };
	 StatisticsData()
	     : min(0.0), avg(0.0), max(0.0), stddev(0.0), count(0){
	 };

         float min;
         float avg;
         float max;
         float stddev;
         boost::uint32_t count;
     };

     /** \brief Accumulate measurement values

         The accumulator mainly do the prelimenary work for the senf::Statistic class.
         It accumulates certain values with in an interval to be used by senf::Statistics

         \li the senf::Statistics class
         \li statistics sources

         This class provides the average, standard deviation, min, max values over one
         interval, means until clear() is called. It rather directly calculates the results
         then collection all single values provided by calling accumulate().
         \see senf::Statistics to process accumulated values
         \ingroup senf_statistics
      */
    template <class T>
    class StatisticAccumulator
    {
    public:
        StatisticAccumulator();
//        virtual ~StatisticAccumulator();

        void clear();
        ///< Reset accumulated values.
        /**< This member reset all values. */
        void accumulate(T value);
        ///< Gather value to be accumulated.
        /**< This method accumulates a value.
             \param[in] value to be accumulated value*/
        void setLastAvg(T value);
        ///< If no real data was collected, this method specifies the min/avg/max value to be returned.
        /**< This method specifies the the min/avg/max value to be returned if no real data was colelcted
             \param[in] value to be returned*/

    public:
        T     min() const;
        ///< Returns current minimal value.
        /**< This method returns the minimal value of the current accumulation.*/
        T     max() const;
        ///< Returns current maximal value.
        /**< This method returns the maximal value of the current accumulation.*/
        float avg() const;
        ///< Returns average value.
        /**< This method returns the average value of the current accumulation.*/
        float last_avg() const;
        ///< Returns former average value.
        /**< This method returns the average value of the former accumulation period.*/
        float stddev() const;
        ///< Returns standard deviation value.
        /**< This method returns the standard deviation value of the current accumulation.*/
        boost::uint32_t count() const;
        ///< Returns count of accumulated values.
        /**< This method returns count of accumulated values of the current accumulation.*/
        void data( StatisticsData & data_) const;
        ///< Returns the accumulated data as a tuple
        /**< This method returns the accumulated information as a tuple.*/  

    private:
        T sum_squared_;
        T sum_;
        T min_;
        T max_;
        float last_avg_;
        boost::uint32_t count_;


    };


    typedef StatisticAccumulator<int> StatisticAccumulatorInt;
    typedef StatisticAccumulator<float> StatisticAccumulatorFloat;

}
///////////////////////////////hh.e////////////////////////////////////////
//#include "StatisticAccumulator.cci"
#include "StatisticAccumulator.ct"
#include "StatisticAccumulator.cti"
#endif

