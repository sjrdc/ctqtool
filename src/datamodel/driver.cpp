/*
 * this file is part of CTQ tool - a tool to explore critical to quality trees
 * Copyright (C) 2021 Sjoerd Crijns
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
