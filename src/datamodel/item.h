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

#pragma once

#include <QString>
#include <QVariant>

#include <memory>
#include <vector>

namespace CtqTool
{
    class ItemData
    {
    public:
        ItemData(QString, QString);
        
        void SetText(QString);
        QString GetText() const;

        void SetNote(QString);
        QString GetNote() const;

    private:
        size_t id = 0;
        QString text;
        QString note;
    };
    
    class TreeItem
    {
    public:
        explicit TreeItem(std::shared_ptr<ItemData> data, TreeItem* parentItem = nullptr);

        void Append(std::shared_ptr<TreeItem> child);
        bool InsertChildren(int position, int count, int columns);
        bool RemoveChildren(int position, int count);
        const std::shared_ptr<TreeItem>& GetChild(int row);
        int ChildCount() const;
        int ColumnCount() const;
        QVariant Data(int column) const;
        void SetData(int column, const QVariant&);
        void CloneDataFrom(const TreeItem&);
        int Row() const;

        TreeItem const* GetParent() const;
        TreeItem* GetParent();

        void PropagateRank();
        void SetRank(unsigned short);
        unsigned short GetRank() const;

    private:
        std::vector<std::shared_ptr<TreeItem>> children;
        std::shared_ptr<ItemData> data = nullptr;
        TreeItem* parentItem = nullptr;
        unsigned short rank = 0;
    };
}
