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

#include <QAbstractProxyModel>
#include <memory>
namespace CtqTool
{
    class CtqProxyModel : public QAbstractProxyModel 
    {
    Q_OBJECT
    public:
        CtqProxyModel(int offset, QObject* parent = nullptr);
        virtual ~CtqProxyModel();

        virtual void setSourceModel(QAbstractItemModel *);

        virtual QModelIndex mapFromSource(const QModelIndex &) const;
        virtual QModelIndex mapToSource(const QModelIndex &) const;

        virtual QModelIndex	parent(const QModelIndex &) const;
        virtual QModelIndex index(int, int, const QModelIndex & p = QModelIndex()) const;
        
        virtual int	rowCount(const QModelIndex & p = QModelIndex()) const;
        virtual int	columnCount(const QModelIndex & p = QModelIndex()) const;

    private slots:
        void SourceRowsAboutToBeInserted(QModelIndex, int, int);
        void SourceRowsAboutToBeRemoved(QModelIndex, int, int);
        void SourceRowsInserted(QModelIndex, int, int);
        void SourceRowsRemoved(QModelIndex, int, int);
        void SourceDataChanged(QModelIndex, QModelIndex);
        void SourceModelReset();

    private:
        class CtqProxyModelImpl;
        std::unique_ptr<CtqProxyModelImpl> impl;
    };
}