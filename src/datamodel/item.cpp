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
    Item::Item(QString text, QString note) :
        text(text),
        note(note)
    {
    }

    void Item::SetWeight(unsigned short w)
    {
        weight = w;
    }

    unsigned short Item::GetWeight() const
    {
        return weight;
    }

    void Item::SetText(QString t)
    {
        text = t;
    }

    QString Item::GetText() const
    {
        return text;
    }

    void Item::SetNote(QString t)
    {
        note = t;
    }

    QString Item::GetNote() const
    {
       return note;
    }

    TreeItem::TreeItem(std::shared_ptr<Item> data, TreeItem* parent) :
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
        return 2;
    }
    
    QString TreeItem::Data(int column) const
    {
        if (column < 0 || column >= ColumnCount() || data == nullptr)
        {
            return QString();
        }

        if (column == 0)
            return data->GetText();
        else if (column == 1)
            return data->GetNote();
        else
            return QString();
    }

    TreeItem* TreeItem::GetParent() const
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

    void TreeItem::SetData(int col, const QString& d)
    {
        if (data != nullptr)
        {
            if (col == 0)
            {
                data->SetText(d);
            }
            else if (col == 1)
            {
                data->SetNote(d);
            }
        }
    }

    bool TreeItem::InsertChildren(int position, int count, int columns)
    {
        if (position < 0 || position > children.size())
            return false;

        for (int row = 0; row < count; ++row) 
        {
            std::vector<QString> data(columns);
            auto item = std::make_shared<TreeItem>(std::make_shared<Item>(data[0], data[1]), this);
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
}
