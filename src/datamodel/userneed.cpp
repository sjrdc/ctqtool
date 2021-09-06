#include "userneed.h"

namespace CtqTool
{
    void UserNeed::Add(std::shared_ptr<Driver> ctq)
    {
        drivers.emplace_back(std::move(ctq));
    }

    std::shared_ptr<Driver> UserNeed::GetDriver(size_t i) const
    {
        return *std::next(drivers.begin(), i);
    }
    
    size_t UserNeed::GetNrOfDrivers() const
    {
        return drivers.size();
    }
}
