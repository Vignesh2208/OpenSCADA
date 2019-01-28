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

namespace pc_emulator {

    struct DateDataType {
        int Year;
        int Month;
        int Day;  
        DateDataType& operator+(const DateDataType& a) {
            return *this;
        }

        DateDataType& operator-(const DateDataType& a) {
            return *this;
        }

        DateDataType& operator/(const DateDataType& a) {
            return *this;
        }

        DateDataType& operator%(const DateDataType& a) {
            return *this;
        }

        DateDataType& operator*(const DateDataType& a) {
            return *this;
        }

        DateDataType& operator&(const DateDataType& a) {
            return *this;
        }

        DateDataType& operator|(const DateDataType& a) {
            return *this;
        }

        DateDataType& operator^(const DateDataType& a) {
            return *this;
        }

        friend bool operator>(const DateDataType& b, const DateDataType& a) {
            if (b.Year > a.Year)
                return true;
            if (b.Year == a.Year && b.Month > a.Month)
                return true;
            if (b.Year == a.Year && b.Month == a.Month && b.Day > a.Day)
                return true;
            return false;
        }

        friend bool operator>=(const DateDataType& b, const DateDataType& a) {
            if (b.Year > a.Year)
                return true;
            if (b.Year == a.Year && b.Month > a.Month)
                return true;
            if (b.Year == a.Year && b.Month == a.Month && b.Day >= a.Day)
                return true;
            return false;
        }

        friend bool operator<(const DateDataType& b, const DateDataType& a) {
            if (b.Year < a.Year)
                return true;
            if (b.Year == a.Year && b.Month < a.Month)
                return true;
            if (b.Year == a.Year && b.Month == a.Month && b.Day < a.Day)
                return true;
            return false;
        }

        friend bool operator<=(const DateDataType& b, const DateDataType& a) {
            if (b.Year < a.Year)
                return true;
            if (b.Year == a.Year && b.Month < a.Month)
                return true;
            if (b.Year == a.Year && b.Month == a.Month && b.Day <= a.Day)
                return true;
            return false;
        }

        friend bool operator==(const DateDataType& b, const DateDataType& a) {
            if (b.Year == a.Year && b.Month == a.Month && b.Day == a.Day)
                return true;
            return false;
        }

    };

    typedef struct DateDataType DateType;

    struct TODDataType {
        int Hr;
        int Min;
        int Sec;

        TODDataType& operator+(const TODDataType& a) {
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            tot1_secs += tot2_secs;
            Hr = tot1_secs / 3600;
            Min = (tot1_secs - Hr*3600)/60;
            Sec = (tot1_secs - Hr*3600 - Min*60);
            return *this;
        }

        TODDataType& operator-(const TODDataType& a) {
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;
            auto tot1_secs = Hr*3600 + Min*60 + Sec;
            tot1_secs -= tot2_secs;
            Hr = tot1_secs / 3600;
            Min = (tot1_secs - Hr*3600)/60;
            Sec = (tot1_secs - Hr*3600 - Min*60);
            return *this;
        }

        TODDataType& operator/(const TODDataType& a) {
            return *this;
        }

        TODDataType& operator%(const TODDataType& a) {
            return *this;
        }

        TODDataType& operator*(const TODDataType& a) {
            return *this;
        }

        TODDataType& operator&(const TODDataType& a) {
            return *this;
        }

        TODDataType& operator|(const TODDataType& a) {
            return *this;
        }

        TODDataType& operator^(const TODDataType& a) {
            return *this;
        }


        friend bool operator>(const TODDataType& b, const TODDataType& a) {
            auto tot1_secs = b.Hr*3600 + b.Min*60 + b.Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs > tot2_secs;
        }

        friend bool operator>=(const TODDataType& b, const TODDataType& a) {
            auto tot1_secs = b.Hr*3600 + b.Min*60 + b.Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs > tot2_secs;
        }

        friend bool operator<(const TODDataType& b, const TODDataType& a) {
            auto tot1_secs = b.Hr*3600 + b.Min*60 + b.Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs < tot2_secs;
        }

        friend bool operator<=(const TODDataType& b, const TODDataType& a) {
            auto tot1_secs = b.Hr*3600 + b.Min*60 + b.Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs <= tot2_secs;
        }

        friend bool operator==(const TODDataType& b, const TODDataType& a) {
            auto tot1_secs = b.Hr*3600 + b.Min*60 + b.Sec;
            auto tot2_secs = a.Hr*3600 + a.Min*60 + a.Sec;

            return tot1_secs > tot2_secs;
        }

    };

    typedef struct TODDataType TODType;

    struct DateTODDataType {
        DateType Date;
        TODType Tod;

    
        DateTODDataType& operator+(const DateTODDataType& a) {
            return *this;
        }

        DateTODDataType& operator-(const DateTODDataType& a) {
            return *this;
        }

        DateTODDataType& operator/(const DateTODDataType& a) {
            return *this;
        }

        DateTODDataType& operator%(const DateTODDataType& a) {
            return *this;
        }

        DateTODDataType& operator*(const DateTODDataType& a) {
            return *this;
        }

        DateTODDataType& operator&(const DateTODDataType& a) {
            return *this;
        }

        DateTODDataType& operator|(const DateTODDataType& a) {
            return *this;
        }

        DateTODDataType& operator^(const DateTODDataType& a) {
            return *this;
        }

        friend bool operator<(const DateTODDataType& b, const DateTODDataType& a) {
            if (b.Date < a.Date)
                return true;
            if (b.Date == a.Date)
                return b.Tod < a.Tod;
            return false;
        }

        friend bool operator<=(const DateTODDataType& b, const DateTODDataType& a) {
            if (b.Date < a.Date)
                return true;
            if (b.Date == a.Date)
                return b.Tod <= a.Tod;
            return false;
        }

        friend bool operator>(const DateTODDataType& b, const DateTODDataType& a) {
            if (b.Date > a.Date)
                return true;
            if (b.Date == a.Date)
                return b.Tod > a.Tod;
            return false;
        }

        friend bool operator>=(const DateTODDataType& b, const DateTODDataType& a) {
            if (b.Date > a.Date)
                return true;
            if (b.Date == a.Date)
                return b.Tod >= a.Tod;
            return false;
        }

        friend bool operator==(const DateTODDataType& b, const DateTODDataType& a) {
            if (b.Date == a.Date)
                return b.Tod == a.Tod;
            return false;
        }

    };

    typedef struct DateTODDataType DateTODType;

    struct TIMEDataType {
        unsigned long SecsElapsed;


        TIMEDataType& operator+(const TIMEDataType& a) {
            SecsElapsed += a.SecsElapsed;
            return *this;
        }

        TIMEDataType& operator-(const TIMEDataType& a) {
            SecsElapsed -= a.SecsElapsed;
            return *this;
        }

        TIMEDataType& operator/(const TIMEDataType& a) {
            return *this;
        }

        TIMEDataType& operator%(const TIMEDataType& a) {
            return *this;
        }

        TIMEDataType& operator*(const TIMEDataType& a) {
            return *this;
        }

        TIMEDataType& operator&(const TIMEDataType& a) {
           return *this;
        }

        TIMEDataType& operator|(const TIMEDataType& a) {
            return *this;
        }

        TIMEDataType& operator^(const TIMEDataType& a) {
            return *this;
        }


        friend bool operator>(const TIMEDataType& b, const TIMEDataType& a) {
            return b.SecsElapsed > a.SecsElapsed;
        }

        friend bool operator>=(const TIMEDataType& b, const TIMEDataType& a) {
            return b.SecsElapsed >= a.SecsElapsed;
        }

        friend bool operator<(const TIMEDataType& b, const TIMEDataType& a) {
            return b.SecsElapsed < a.SecsElapsed;
        }

        friend bool operator<=(const TIMEDataType& b, const TIMEDataType& a) {
            return b.SecsElapsed <= a.SecsElapsed;
        }

        friend bool operator==(const TIMEDataType& b, const TIMEDataType& a) {
            return b.SecsElapsed == a.SecsElapsed;
        }
        
    };

    typedef struct TIMEDataType TimeType;
}

#endif
