#include "ctq.h"

namespace CtqTool
{
    const Measurement& Ctq::GetMeasurement() const
    {
        return measurement;
    }

    Measurement& Ctq::GetMeasurement()
    {
        return measurement;
    }
}
