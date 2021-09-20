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
                return 0;

            if (!sourceRootsCounts.contains(row))
            {
                sourceRootsCounts[row] = instance->sourceModel()->rowCount(
                    instance->sourceModel()->index(row,0)
                );
            }

            return sourceRootsCounts[row];
        }

        int RowFrom(int root, int row)
        {
            for (int r = 0; r< root; r++)
            row += SourceRootToCount(r);
            return row;
        }

        QPair<int,int> RowToSource(int row) 
        {
            int root = 0;
            for (int r = 0; r < instance->sourceModel()->rowCount(); r++) 
            {
                root = r;
                int rows_in_root = SourceRootToCount(r);
                if (row >= rows_in_root)
                    row -= rows_in_root;
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

    void CtqProxyModel::setSourceModel(QAbstractItemModel * m)
    {
        if (sourceModel())
        {
            disconnect(sourceModel(), SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)), 
                    this, SLOT(SourceRowsAboutToBeInserted(QModelIndex, int, int)));
            disconnect(sourceModel(), SIGNAL(rowsInserted(QModelIndex, int, int)), 
                    this, SLOT(SourceRowsInserted(QModelIndex, int, int)));

            disconnect(sourceModel(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), 
                    this, SLOT(SourceRowsAboutToBeRemoved(QModelIndex, int, int)));
            disconnect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex, int, int)), 
                    this, SLOT(SourceRowsRemoved(QModelIndex, int, int)));

            disconnect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), 
                    this, SLOT(SourceDataChanged(QModelIndex,QModelIndex)));
            disconnect(sourceModel(), SIGNAL(modelReset()), 
                    this, SLOT(SourceModelReset()));

            disconnect(sourceModel(), SIGNAL(layoutAboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()));
            disconnect(sourceModel(), SIGNAL(layoutChanged()), this, SIGNAL(layoutChanged()));
        }

        QAbstractProxyModel::setSourceModel(m);

        if (sourceModel()) 
        {
            connect(sourceModel(), SIGNAL(rowsAboutToBeInserted(QModelIndex, int, int)), 
                    this, SLOT(SourceRowsAboutToBeInserted(QModelIndex, int, int)));
            connect(sourceModel(), SIGNAL(rowsInserted(QModelIndex, int, int)), 
                    this, SLOT(SourceRowsInserted(QModelIndex, int, int)));

            connect(sourceModel(), SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), 
                    this, SLOT(SourceRowsAboutToBeRemoved(QModelIndex, int, int)));
            connect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex, int, int)), 
                    this, SLOT(SourceRowsRemoved(QModelIndex, int, int)));

            connect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), 
                    this, SLOT(SourceDataChanged(QModelIndex,QModelIndex)));
            connect(sourceModel(), SIGNAL(modelReset()), 
                    this, SLOT(SourceModelReset()));

            connect(sourceModel(), SIGNAL(layoutAboutToBeChanged()), this, SIGNAL(layoutAboutToBeChanged()));
            connect(sourceModel(), SIGNAL(layoutChanged()), this, SIGNAL(layoutChanged()));
        }

        revert();
    }

    QModelIndex CtqProxyModel::mapFromSource(const QModelIndex & source) const
    {
        if (!sourceModel() || !source.parent().isValid())
        {
            return QModelIndex();
        }

        return index(impl->RowFrom(source.parent().row(), source.row()), source.column());
    }

    QModelIndex CtqProxyModel::mapToSource(const QModelIndex & proxy) const
    {
        if (!sourceModel())
        {
            return QModelIndex();
        }    

        QPair<int, int> pos = impl->RowToSource(proxy.row());
        int root_row = pos.first;
        int row = pos.second;

        QModelIndex p = sourceModel()->index(root_row, proxy.column());
        return sourceModel()->index(row, proxy.column(), p);
    }

    QModelIndex	CtqProxyModel::parent(const QModelIndex &) const
    {
        return QModelIndex();
    }

    QModelIndex CtqProxyModel::index(int row, int column, const QModelIndex &) const
    {
        return createIndex(row, column);
    }

    int	CtqProxyModel::rowCount(const QModelIndex & p) const
    {
        if (p.isValid() || !sourceModel()) 
        {
            return 0;
        }

        int count = 0;
        for (int root_row = 0; root_row< sourceModel()->rowCount(); root_row++)
        {
            count += impl->SourceRootToCount(root_row);
        } 

        return count;
    }

    int	CtqProxyModel::columnCount(const QModelIndex & p) const
    {
        if (p.isValid() || !sourceModel()) 
        {
            return 0;
        }
        return sourceModel()->columnCount();
    }

    void CtqProxyModel::SourceRowsAboutToBeInserted(QModelIndex p, int from, int to)
    {
        if (!p.isValid()) 
        {
            return;
        }

        int f = impl->RowFrom(p.row(), from);
        int t = f + (from-to);
        beginInsertRows(QModelIndex(), f, t);
    }

    void CtqProxyModel::SourceRowsInserted(QModelIndex p, int, int)
    {
        impl->sourceRootsCounts.clear();
        if (!p.isValid())
        {
            return;
        }
        endInsertRows();
    }

    void CtqProxyModel::SourceRowsAboutToBeRemoved(QModelIndex p, int from, int to)
    {
        if (!p.isValid()) {
            //remove root items
            int f = impl->RowFrom(from,0);
            int t = impl->RowFrom(to,0)+ impl->SourceRootToCount(to);

            if (f != t) {
                beginRemoveRows(QModelIndex(), f, t-1);
                impl->aboutToRemoveRoots = true;
            }
            
            return;
        }

        int f = impl->RowFrom(p.row(), from);
        int t = f + (from-to);
        beginRemoveRows(QModelIndex(), f, t);
    }

    void CtqProxyModel::SourceRowsRemoved(QModelIndex p, int, int)
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

    void CtqProxyModel::SourceDataChanged(QModelIndex tl, QModelIndex br)
    {
        QModelIndex p_tl = mapFromSource(tl);
        QModelIndex p_br = mapFromSource(br);
        emit dataChanged(p_tl, p_br);
    }

    void CtqProxyModel::SourceModelReset()
    {
        impl->sourceRootsCounts.clear();
        revert();
    }
}