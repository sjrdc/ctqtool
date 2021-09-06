#pragma once

#include "item.h"

#include <list>
#include <memory>

namespace CtqTool
{
    class Driver;
    class UserNeed : public Item
    {
    public:
        void Add(std::shared_ptr<Driver>);
        std::shared_ptr<Driver> GetDriver(size_t i) const;
        size_t GetNrOfDrivers() const;
        
    private:
        std::list<std::shared_ptr<Driver>> drivers;

    };    
}
