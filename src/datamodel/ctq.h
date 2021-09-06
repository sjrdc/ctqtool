#pragma once

#include "item.h"
#include "measurement.h"

namespace CtqTool
{
    class Ctq : public Item
    {
    public:
        Measurement& GetMeasurement();
        const Measurement& GetMeasurement() const;

    private:
        Measurement measurement;
    };
}
