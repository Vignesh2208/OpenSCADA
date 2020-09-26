#ifndef __PC_EMULATOR_INCLUDE_ELEMENTARY_DATATYPES_H__
#define __PC_EMULATOR_INCLUDE_ELEMENTARY_DATATYPESE_H__

#include <iostream>
#include <vector>
#include <climits>
#include <cstdlib>
#include <assert.h>
#include <unordered_map>

#include "src/pc_emulator/proto/configuration.pb.h"

using namespace std;
using namespace pc_specification;

using MemType  = pc_specification::MemType;
using DataTypeCategory = pc_specification::DataTypeCategory;
using FieldInterfaceType = pc_specification::FieldInterfaceType;
typedef long long s64;

namespace pc_emulator {

    //! The elementary Date data type containing year, month, day
    struct DateDataType {
        int Year; 
        int Month;
        int Day;  

        //! Add operator for two date data types. Does nothing.
        DateDataType& operator+(const DateDataType& a) {
            return *this;
        }

        //! Sub operator for two date data types. Does nothing.
        DateDataType& operator-(const DateDataType& a) {
            return *this;
        }

        //! Divide operator for two date data types. Does nothing.
        DateDataType& operator/(const DateDataType& a) {
            return *this;
        }

        //! Mod operator for two date data types. Does nothing.
        DateDataType& operator%(const DateDataType& a) {
            return *this;
        }

        //! Multiply operator for two date data types. Does nothing.
        DateDataType& operator*(const DateDataType& a) {
            return *this;
        }

        //! AND operator for two date data types. Does nothing.
        DateDataType& operator&(const DateDataType& a) {
            return *this;
        }

        //! OR operator for two date data types. Does nothing.
        DateDataType& operator|(const DateDataType& a) {
            return *this;
        }

        //! XOR operator for two date data types. Does nothing.
        DateDataType& operator^(const DateDataType& a) {
            return *this;
        }

        //! Greater than operator for two date data types
        friend bool operator>(const DateDataType& b, const DateDataType& a) {
            if (b.Year > a.Year)
                return true;
            if (b.Year == a.Year && b.Month > a.Month)
                return true;
            if (b.Year == a.Year && b.Month == a.Month && b.Day > a.Day)
                return true;
            return false;
        }

        //! Greater than or equal operator for two date data types
        friend bool operator>=(const DateDataType& b, const DateDataType& a) {
            if (b.Year > a.Year)
                return true;
            if (b.Year == a.Year && b.Month > a.Month)
                return true;
            if (b.Year == a.Year && b.Month == a.Month && b.Day >= a.Day)
                return true;
            return false;
        }

        //! Less than operator for two date data types
        friend bool operator<(const DateDataType& b, const DateDataType& a) {
            if (b.Year < a.Year)
                return true;
            if (b.Year == a.Year && b.Month < a.Month)
                return true;
            if (b.Year == a.Year && b.Month == a.Month && b.Day < a.Day)
                return true;
            return false;
        }

        //! Less than or equal operator for two date data types
        friend bool operator<=(const DateDataType& b, const DateDataType& a) {
            if (b.Year < a.Year)
                return true;
            if (b.Year == a.Year && b.Month < a.Month)
                return true;
            if (b.Year == a.Year && b.Month == a.Month && b.Day <= a.Day)
                return true;
            return false;
        }

        //! Test for equality of two date data types
        friend bool operator==(const DateDataType& b, const DateDataType& a) {
            if (b.Year == a.Year && b.Month == a.Month && b.Day == a.Day)
                return true;
            return false;
        }

    };

    typedef struct DateDataType DateType;

    //! The elementary Date data type containing hour, minutes, seconds
    struct TODDataType {
        int Hr;
        int Min;
        int Sec;

        //! Add operator for two TOD data types
        TODDataType& operator+(const TODDataType& a) {
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            tot1_secs += tot2_secs;
            Hr = tot1_secs / 3600;
            Min = (tot1_secs - Hr*3600)/60;
            Sec = (tot1_secs - Hr*3600 - Min*60);
            return *this;
        }

        //! Sub operator for two TOD data types.
        TODDataType& operator-(const TODDataType& a) {
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            tot1_secs -= tot2_secs;
            Hr = tot1_secs / 3600;
            Min = (tot1_secs - Hr*3600)/60;
            Sec = (tot1_secs - Hr*3600 - Min*60);
            return *this;
        }

        //! Divide operator for two TOD data types. Does nothing.
        TODDataType& operator/(const TODDataType& a) {
            return *this;
        }

        //! Mod operator for two TOD data types. Does nothing.
        TODDataType& operator%(const TODDataType& a) {
            return *this;
        }

        //! Multiply operator for two TOD data types. Does nothing.
        TODDataType& operator*(const TODDataType& a) {
            return *this;
        }

        //! AND operator for two TOD data types. Does nothing.
        TODDataType& operator&(const TODDataType& a) {
            return *this;
        }

        //! OR operator for two TOD data types. Does nothing.
        TODDataType& operator|(const TODDataType& a) {
            return *this;
        }

        //! XOR operator for two TOD data types. Does nothing.
        TODDataType& operator^(const TODDataType& a) {
            return *this;
        }

        //! Greater than l operator for two TOD data types
        friend bool operator>(const TODDataType& b, const TODDataType& a) {
            auto tot1_secs = b.Hr*3600 + b.Min*60 + b.Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs > tot2_secs;
        }

        //! Greater than or equal operator for two TOD data types
        friend bool operator>=(const TODDataType& b, const TODDataType& a) {
            auto tot1_secs = b.Hr*3600 + b.Min*60 + b.Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs >= tot2_secs;
        }

        //! Less than operator for two TOD data types
        friend bool operator<(const TODDataType& b, const TODDataType& a) {
            auto tot1_secs = b.Hr*3600 + b.Min*60 + b.Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs < tot2_secs;
        }

        //! Less than or equal operator for two TOD data types
        friend bool operator<=(const TODDataType& b, const TODDataType& a) {
            auto tot1_secs = b.Hr*3600 + b.Min*60 + b.Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs <= tot2_secs;
        }

        //! Test for equality of two TOD data types
        friend bool operator==(const TODDataType& b, const TODDataType& a) {
            auto tot1_secs = b.Hr*3600 + b.Min*60 + b.Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs == tot2_secs;
        }

    };

    typedef struct TODDataType TODType;

    //!Date and Time (dt) data type
    struct DateTODDataType {
        DateType Date;
        TODType Tod;

    
        //! ADD operator for two dt data types. Does nothing.
        DateTODDataType& operator+(const DateTODDataType& a) {
            return *this;
        }

        //! Sub operator for two dt data types. Does nothing.
        DateTODDataType& operator-(const DateTODDataType& a) {
            return *this;
        }

        //! Divide operator for two dt data types. Does nothing.
        DateTODDataType& operator/(const DateTODDataType& a) {
            return *this;
        }

        //! Mod operator for two dt data types. Does nothing.
        DateTODDataType& operator%(const DateTODDataType& a) {
            return *this;
        }

        //! Multiply operator for two dt data types. Does nothing.
        DateTODDataType& operator*(const DateTODDataType& a) {
            return *this;
        }

        //! AND operator for two dt data types. Does nothing.
        DateTODDataType& operator&(const DateTODDataType& a) {
            return *this;
        }

        //! OR operator for two dt data types. Does nothing.
        DateTODDataType& operator|(const DateTODDataType& a) {
            return *this;
        }

        //! XOR operator for two dt data types. Does nothing.
        DateTODDataType& operator^(const DateTODDataType& a) {
            return *this;
        }

        //! Less than operator for two dt data types
        friend bool operator<(const DateTODDataType& b, const DateTODDataType& a) {
            if (b.Date < a.Date)
                return true;
            if (b.Date == a.Date)
                return b.Tod < a.Tod;
            return false;
        }

        //! Less than or equal operator for two dt data types
        friend bool operator<=(const DateTODDataType& b, const DateTODDataType& a) {
            if (b.Date < a.Date)
                return true;
            if (b.Date == a.Date)
                return b.Tod <= a.Tod;
            return false;
        }

        //! Greater than operator for two dt data types
        friend bool operator>(const DateTODDataType& b, const DateTODDataType& a) {
            if (b.Date > a.Date)
                return true;
            if (b.Date == a.Date)
                return b.Tod > a.Tod;
            return false;
        }

        //! Greater than or equal operator for two dt data types
        friend bool operator>=(const DateTODDataType& b, const DateTODDataType& a) {
            if (b.Date > a.Date)
                return true;
            if (b.Date == a.Date)
                return b.Tod >= a.Tod;
            return false;
        }

        // Test for equality of two dt data types
        friend bool operator==(const DateTODDataType& b, const DateTODDataType& a) {
            if (b.Date == a.Date)
                return b.Tod == a.Tod;
            return false;
        }

    };

    typedef struct DateTODDataType DateTODType;

    // Time data type. Contains seconds elapsed
    struct TIMEDataType {
        double SecsElapsed;


        //! Add operator for two time data types
        TIMEDataType& operator+(const TIMEDataType& a) {
            SecsElapsed += a.SecsElapsed;
            return *this;
        }

        //! Sub operator for two time data types
        TIMEDataType& operator-(const TIMEDataType& a) {
            SecsElapsed -= a.SecsElapsed;
            return *this;
        }

        //! Divide operator for two time data types. Does nothing.
        TIMEDataType& operator/(const TIMEDataType& a) {
            return *this;
        }

        //! Mod operator for two time data types. Does nothing.
        TIMEDataType& operator%(const TIMEDataType& a) {
            return *this;
        }

        //! Multiply operator for two time data types. Does nothing.
        TIMEDataType& operator*(const TIMEDataType& a) {
            return *this;
        }

        //! AND operator for two time data types. Does nothing.
        TIMEDataType& operator&(const TIMEDataType& a) {
           return *this;
        }

        //! OR operator for two time data types. Does nothing.
        TIMEDataType& operator|(const TIMEDataType& a) {
            return *this;
        }

        //! XOR operator for two time data types. Does nothing.
        TIMEDataType& operator^(const TIMEDataType& a) {
            return *this;
        }

        //! Greater than operator for two time data types
        friend bool operator>(const TIMEDataType& b, const TIMEDataType& a) {
            return b.SecsElapsed > a.SecsElapsed;
        }

        //! Greater than or equal operator for two time data types
        friend bool operator>=(const TIMEDataType& b, const TIMEDataType& a) {
            return b.SecsElapsed >= a.SecsElapsed;
        }

        //! Less than operator for two time data types
        friend bool operator<(const TIMEDataType& b, const TIMEDataType& a) {
            return b.SecsElapsed < a.SecsElapsed;
        }

        //! Less than or equal operator for two time data types
        friend bool operator<=(const TIMEDataType& b, const TIMEDataType& a) {
            return b.SecsElapsed <= a.SecsElapsed;
        }

        // Test for eqality of two time data types
        friend bool operator==(const TIMEDataType& b, const TIMEDataType& a) {
            return b.SecsElapsed == a.SecsElapsed;
        }
        
    };

    typedef struct TIMEDataType TimeType;
}

#endif
