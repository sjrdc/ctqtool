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

#include <QAbstractItemModel>

#include <memory>

namespace CtqTool
{
    class TreeItem;
    class CtqModel : public QAbstractItemModel
    {
    Q_OBJECT
    public:
        explicit CtqModel(QObject* parent = nullptr);
        ~CtqModel();

        QVariant data(const QModelIndex& index, int role) const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const override;
        QModelIndex index(int row, int column,
                        const QModelIndex& parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex& index) const override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex& parent = QModelIndex()) const override;
        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
          

        bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override; 
        bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
        
        void Reset(const QString& data);
        
    private:
        void SetupModelData(const QStringList& lines, TreeItem& parent);
        TreeItem* GetItem(const QModelIndex &index) const;

        void OnDataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&);

        std::unique_ptr<TreeItem> rootItem;
        static constexpr int maxDepth = 3; // i.e. need, driver, ctq
    };
}
