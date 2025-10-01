#ifndef MEASUREMENT_H
#define MEASUREMENT_H

namespace Measurement
{

/* Usage example:
    Measurement::kilometer valueKm (1);
    Measurement::meter valueM (1);

    Measurement::day      valueDay (1);
    Measurement::hour     valueHour (1);
    Measurement::minute   valueMinute (1);
    Measurement::second   valueSecond (1);

    valueKm += valueM;
    valueMinute -= valueSecond;
*/

typedef double measure_t;

template <int MEASURE_TYPE_ID, int MULTIPLIER_VALUE_GRADE>
class MeasValue
{
    measure_t m_selfValue {0};
public:
    MeasValue(const measure_t& initV) :
        m_selfValue {initV} {
    }
    MeasValue(const MeasValue&) = default;
    MeasValue(MeasValue&&) = default;

     void setValue(measure_t v) {
         m_selfValue = v;
     }

     measure_t getValue() const {
         return m_selfValue;
     }

     // Get value in unified metrics
     measure_t getUnifiedValue() const {
         return m_selfValue * MULTIPLIER_VALUE_GRADE;
     }

     // Multiplier – relative to value
     template <int O_MULTIPLIER_VALUE_GRADE>
     measure_t getValueRelative(const MeasValue<MEASURE_TYPE_ID, O_MULTIPLIER_VALUE_GRADE>& multiplier) const {
         return m_selfValue * MULTIPLIER_VALUE_GRADE / O_MULTIPLIER_VALUE_GRADE;
     }

     MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE>& operator =(const MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE>& _otherMeasure) = default;

     template <int O_MULTIPLIER_VALUE_GRADE>
     MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE>& operator +=(const MeasValue<MEASURE_TYPE_ID, O_MULTIPLIER_VALUE_GRADE>& _otherMeasure) {
         m_selfValue += _otherMeasure.getValueRelative(*this);
         return *this;
     }

     template <int O_MULTIPLIER_VALUE_GRADE>
     MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE>& operator -=(const MeasValue<MEASURE_TYPE_ID, O_MULTIPLIER_VALUE_GRADE>& _otherMeasure) {
         m_selfValue -= _otherMeasure.getValueRelative(*this);
         return *this;
     }

     template <int O_MULTIPLIER_VALUE_GRADE>
     MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE> operator +(const MeasValue<MEASURE_TYPE_ID, O_MULTIPLIER_VALUE_GRADE>& _otherMeasure) {
         return (m_selfValue + _otherMeasure.getValueRelative(*this));
     }

     template <int O_MULTIPLIER_VALUE_GRADE>
     MeasValue<MEASURE_TYPE_ID, MULTIPLIER_VALUE_GRADE> operator -(const MeasValue<MEASURE_TYPE_ID, O_MULTIPLIER_VALUE_GRADE>& _otherMeasure) {
         return (m_selfValue - _otherMeasure.getValueRelative(*this));
     }
};

enum PredefinedTypes
{
LENGTH_TYPE = 0,
TIME_TYPE,
};

using meter           = MeasValue<PredefinedTypes::LENGTH_TYPE, 1>;
using kilometer       = MeasValue<PredefinedTypes::LENGTH_TYPE, 1000>;

using second  = MeasValue<PredefinedTypes::TIME_TYPE, 1>;
using minute  = MeasValue<PredefinedTypes::TIME_TYPE, 60>;
using hour    = MeasValue<PredefinedTypes::TIME_TYPE, 60*60>;
using day     = MeasValue<PredefinedTypes::TIME_TYPE, 60*60*24>;

}


#endif // MEASUREMENT_H
