#include "driver.h"

namespace CtqTool
{
    void Driver::Add(std::shared_ptr<Ctq> ctq)
    {
        ctqs.emplace_back(std::move(ctq));
    }

    std::shared_ptr<Ctq> Driver::GetCtq(size_t i) const
    {
        return *std::next(ctqs.begin(), i);
    }
    
    size_t Driver::GetNrOfCtqs() const
    {
        return ctqs.size();
    }
}
