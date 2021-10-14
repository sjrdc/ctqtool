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

#include "item.h"

namespace CtqTool
{
    ItemData::ItemData(QString text, QString note) :
        text(text),
        note(note)
    {
        static auto counter = 0u;
        id = counter++;
    }

    void ItemData::SetRank(unsigned short w)
    {
        weight = w;
    }

    unsigned short ItemData::GetRank() const
    {
        return weight;
    }

    void ItemData::SetText(QString t)
    {
        text = t;
    }

    QString ItemData::GetText() const
    {
        return text;
    }

    void ItemData::SetNote(QString t)
    {
        note = t;
    }

    QString ItemData::GetNote() const
    {
       return note;
    }

    TreeItem::TreeItem(std::shared_ptr<ItemData> data, TreeItem* parent) :
        data(std::move(data)), 
        parentItem(parent)
    {
    }

    void TreeItem::Append(std::shared_ptr<TreeItem> item)
    {
        children.push_back(std::move(item));
    }

    const std::shared_ptr<TreeItem>& TreeItem::GetChild(int row)
    {
        if (row < 0 || row >= children.size())
        {
            throw std::runtime_error("wrong index");
        }
        return children.at(row);
    }
    
    int TreeItem::ChildCount() const
    {
        return static_cast<int>(children.size());
    }
    
    int TreeItem::ColumnCount() const
    {
        constexpr auto count = 3;
        return count;
    }
    
    QVariant TreeItem::Data(int column) const
    {
        if (column < 0 || column >= ColumnCount() || data == nullptr)
        {
            return QVariant();
        }

        if (column == 0)
            return data->GetText();
        else if (column == 1)
            return data->GetNote();
        else if (column == 2)
            return data->GetRank();
        else
            return QVariant();
    }

    TreeItem const* TreeItem::GetParent() const
    {
        return parentItem;
    }

    TreeItem* TreeItem::GetParent()
    {
        return parentItem;
    }

    int TreeItem::Row() const
    {
        if (parentItem)
        {
            const auto it = std::find_if(parentItem->children.cbegin(), parentItem->children.cend(), 
            [this](const auto& item) 
            {
                return item->Data(0) == this->Data(0);
            });
            return std::distance(parentItem->children.cbegin(), it);
        }
        return 0;
    }

    void TreeItem::SetData(int col, const QVariant& d)
    {
        if (data != nullptr)
        {
            if (col == 0)
            {
                data->SetText(d.toString());
            }
            else if (col == 1)
            {
                data->SetNote(d.toString());
            }
            else if (col == 2)
            {
                data->SetRank(d.toInt());
            }
        }
    }

    bool TreeItem::InsertChildren(int position, int count, int columns)
    {
        if (position < 0 || position > children.size())
            return false;

        for (int row = 0; row < count; ++row) 
        {
            auto item = std::make_shared<TreeItem>(std::make_shared<ItemData>("[not set]", "[not set]"), this);
            children.insert(children.begin() + position, item);
        }

        return true;
    }
    
    bool TreeItem::RemoveChildren(int position, int count)
    {
        if (position < 0 || position + count > children.size())
            return false;

        for (int row = 0; row < count; ++row)
            children.erase(children.begin() + position);

        return true;
    }

    void TreeItem::CloneDataFrom(const TreeItem& item)
    {
        data = item.data;
    }

    void TreeItem::PropagateRank()
    {
        constexpr auto rankColumn = 2;
        auto rank = Data(rankColumn);
        for (auto& child : children)
        {
            child->SetData(rankColumn, rank);
            child->PropagateRank();
        }
    }
}
