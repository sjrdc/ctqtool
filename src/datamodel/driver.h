#pragma once

#include "item.h"

#include <list>
#include <memory>

namespace CtqTool
{
    class Ctq;
    
    class Driver : public Item
    {
    public:
        void Add(std::shared_ptr<Ctq>);
        std::shared_ptr<Ctq> GetCtq(size_t i) const;
        size_t GetNrOfCtqs() const;
    private:
        std::list<std::shared_ptr<Ctq>> ctqs;
    };
}
