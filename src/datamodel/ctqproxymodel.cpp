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

#include "ctqproxymodel.h"

namespace CtqTool
{
    class CtqProxyModel::CtqProxyModelImpl 
    {
    public:
        CtqProxyModelImpl(int offset, CtqProxyModel* p) :
            instance(p),
            offset(offset)
        {
        }         

        int SourceRootToCount(int row) 
        {
            if (!instance->sourceModel())
            {
                return 0;
            }   

            if (!sourceRootsCounts.contains(row))
            {
                sourceRootsCounts[row] = instance->sourceModel()->rowCount(
                    instance->sourceModel()->index(row,0));
            }

            return sourceRootsCounts[row];
        }

        int RowFrom(int root, int row)
        {
            for (auto r = 0; r < root; r++)
            {
                row += SourceRootToCount(r);
            }
            return row;
        }

        QPair<int,int> RowToSource(int row) 
        {
            auto root = 0;
            for (auto r = 0; r < instance->sourceModel()->rowCount(); r++) 
            {
                root = r;
                const auto rowsInRoot = SourceRootToCount(r);
                if (row >= rowsInRoot)
                    row -= rowsInRoot;
                else break;
            }
            
            return qMakePair(root, row);
        }

        QHash<int, int> sourceRootsCounts; 
        bool aboutToRemoveRoots;

    private:
        CtqProxyModel* instance;
        int offset = 0;
    };

    CtqProxyModel::CtqProxyModel(int offset, QObject* parent) : 
        QAbstractProxyModel(parent),
        impl(std::make_unique<CtqProxyModelImpl>(offset, this))
    {
    }

    CtqProxyModel::~CtqProxyModel() = default;

    void CtqProxyModel::setSourceModel(QAbstractItemModel* m)
    {
        if (sourceModel())
        {
            disconnect(sourceModel(),&QAbstractItemModel::rowsAboutToBeInserted, this,&CtqProxyModel::SourceRowsAboutToBeInserted);
            disconnect(sourceModel(),&QAbstractItemModel::rowsInserted, this,&CtqProxyModel::SourceRowsInserted);

            disconnect(sourceModel(),&QAbstractItemModel::rowsAboutToBeRemoved, this,&CtqProxyModel::SourceRowsAboutToBeRemoved);
            disconnect(sourceModel(),&QAbstractItemModel::rowsRemoved, this,&CtqProxyModel::SourceRowsRemoved);

            disconnect(sourceModel(),&QAbstractItemModel::dataChanged, this,&CtqProxyModel::SourceDataChanged);
            disconnect(sourceModel(),&QAbstractItemModel::modelReset, this,&CtqProxyModel::SourceModelReset);

            disconnect(sourceModel(),&QAbstractItemModel::layoutAboutToBeChanged, this,&CtqProxyModel::layoutAboutToBeChanged);
            disconnect(sourceModel(),&QAbstractItemModel::layoutChanged, this ,&CtqProxyModel::layoutChanged);
        }

        QAbstractProxyModel::setSourceModel(m);

        if (sourceModel()) 
        {
            connect(sourceModel(),&QAbstractItemModel::rowsAboutToBeInserted,this,&CtqProxyModel::SourceRowsAboutToBeInserted);
            connect(sourceModel(),&QAbstractItemModel::rowsInserted, this,&CtqProxyModel::SourceRowsInserted);

            connect(sourceModel(),&QAbstractItemModel::rowsAboutToBeRemoved, this,&CtqProxyModel::SourceRowsAboutToBeRemoved);
            connect(sourceModel(),&QAbstractItemModel::rowsRemoved, this,&CtqProxyModel::SourceRowsRemoved);

            connect(sourceModel(),&QAbstractItemModel::dataChanged, this,&CtqProxyModel::SourceDataChanged);
            connect(sourceModel(),&QAbstractItemModel::modelReset, this,&CtqProxyModel::SourceModelReset);

            connect(sourceModel(),&QAbstractItemModel::layoutAboutToBeChanged, this,&QAbstractItemModel::layoutAboutToBeChanged);
            connect(sourceModel(),&QAbstractItemModel::layoutChanged, this,&QAbstractItemModel::layoutChanged);
        }

        revert();
    }

    QModelIndex CtqProxyModel::mapFromSource(const QModelIndex& source) const
    {
        if (!sourceModel() || !source.parent().isValid())
        {
            return QModelIndex();
        }

        return index(impl->RowFrom(source.parent().row(), source.row()), source.column());
    }

    QModelIndex CtqProxyModel::mapToSource(const QModelIndex& proxy) const
    {
        if (!sourceModel())
        {
            return QModelIndex();
        }    

        const auto pos = impl->RowToSource(proxy.row());
        const auto rootRow = pos.first;
        const auto row = pos.second;

        QModelIndex p = sourceModel()->index(rootRow, proxy.column());
        return sourceModel()->index(row, proxy.column(), p);
    }

    QModelIndex	CtqProxyModel::parent(const QModelIndex&) const
    {
        return QModelIndex();
    }

    QModelIndex CtqProxyModel::index(int row, int column, const QModelIndex&) const
    {
        return createIndex(row, column);
    }

    int	CtqProxyModel::rowCount(const QModelIndex& p) const
    {
        if (p.isValid() || !sourceModel()) 
        {
            return 0;
        }

        auto count = 0;
        for (auto root = 0; root < sourceModel()->rowCount(); root++)
        {
            count += impl->SourceRootToCount(root);
        } 

        return count;
    }

    int	CtqProxyModel::columnCount(const QModelIndex& p) const
    {
        if (p.isValid() || !sourceModel()) 
        {
            return 0;
        }
        return sourceModel()->columnCount(p);
    }

    void CtqProxyModel::SourceRowsAboutToBeInserted(const QModelIndex& p, int from, int to)
    {
        if (!p.isValid()) 
        {
            return;
        }

        const auto f = impl->RowFrom(p.row(), from);
        const auto t = f + (from - to);
        beginInsertRows(QModelIndex(), f, t);
    }

    void CtqProxyModel::SourceRowsInserted(const QModelIndex& p, int, int)
    {
        impl->sourceRootsCounts.clear();
        if (!p.isValid())
        {
            return;
        }
        endInsertRows();
    }

    void CtqProxyModel::SourceRowsAboutToBeRemoved(const QModelIndex& p, int from, int to)
    {
        if (!p.isValid()) {
            // remove root items
            const auto f = impl->RowFrom(from, 0);
            const auto t = impl->RowFrom(to, 0) + impl->SourceRootToCount(to);

            if (f != t) 
            {
                beginRemoveRows(QModelIndex(), f, t-1);
                impl->aboutToRemoveRoots = true;
            }
            
            return;
        }

        const auto f = impl->RowFrom(p.row(), from);
        const auto t = f + (from - to);
        beginRemoveRows(QModelIndex(), f, t);
    }

    void CtqProxyModel::SourceRowsRemoved(const QModelIndex& p, int, int)
    {
        impl->sourceRootsCounts.clear();

        if (!p.isValid()) 
        {
            //remove root items
            if (impl->aboutToRemoveRoots) 
            {
                impl->aboutToRemoveRoots = false;
                endRemoveRows();
            }        
            return;
        }

        endRemoveRows();
    }

    void CtqProxyModel::SourceDataChanged(const QModelIndex& tl, const QModelIndex& br)
    {
        const auto p_tl = mapFromSource(tl);
        const auto p_br = mapFromSource(br);
        dataChanged(p_tl, p_br);
    }

    void CtqProxyModel::SourceModelReset()
    {
        impl->sourceRootsCounts.clear();
        revert();
    }
}