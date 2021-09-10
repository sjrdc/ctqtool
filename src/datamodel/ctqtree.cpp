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

#include "ctqtree.h"
#include "userneed.h"

namespace CtqTool
{
    void CtqTree::Add(UserNeed need)
    {
        needs.emplace_back(std::move(need));
    }

    const UserNeed& CtqTree::GetNeed(size_t i) const
    {
    assert(i < needs.size());
        return *std::next(needs.begin(), i);
    }
    
    size_t CtqTree::GetNrOfNeeds() const
    {
        return needs.size();
    }
}
