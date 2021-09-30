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
#include <QDebug>
#include <boost/bimap.hpp>
#include <map>

namespace
{       
    void buildIndex(QAbstractItemModel* source, QAbstractItemModel* proxy, int targetDepth, int currentDepth, int& row,
        boost::bimap<int, std::pair<int, QModelIndex>>& idx, std::map<int, QModelIndex>& parents, const QModelIndex& sourceIdx = QModelIndex())
    {
        if (currentDepth == targetDepth)
        {
            idx.insert({row, {sourceIdx.row(), sourceIdx.parent()}});
            row++;
        }
        else if (source->hasChildren(sourceIdx))
        {
            for (auto r = 0; r < source->rowCount(sourceIdx); ++r)
            {
                buildIndex(source, proxy, targetDepth, currentDepth + 1, row, idx, parents, source->index(r, 0, sourceIdx));
            }
        }
    }     
}

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

        int SourceToProxy(QModelIndex srcIdx)
        {
            for (auto it = index.right.begin(); it != index.right.end(); ++it)
            {
                qDebug() << it->first << it->second;
            }
            qDebug() << "looking for " << srcIdx.row() << srcIdx.parent();
            if (auto it = index.right.find(std::make_pair(srcIdx.row(), srcIdx.parent())); 
                it != index.right.end())
            {
                return it->second;
            }
            else
            {
                return -1;
            }
        }

        std::pair<int, QModelIndex> ProxyToSource(int proxyIdx)
        {
            return index.left.at(proxyIdx);
        }

        void Reset()
        {
            index.clear();
            parents.clear();
            if (instance != nullptr && instance->sourceModel() != nullptr)
            {
                auto row = 0;
                buildIndex(instance->sourceModel(), instance, offset, 0, row, index, parents);
            }
        }

        void SetAboutToRemoveRoots(bool b)
        {
            aboutToRemoveRoots = b;
        }

        bool IsAboutToRemoveRoots() const
        {
            return aboutToRemoveRoots;
        }

        int RowCount() const
        {
            return index.size();
        }

    private:
        CtqProxyModel* instance;
        boost::bimap<int, std::pair<int, QModelIndex>> index; 
        std::map<int, QModelIndex> parents;
        int offset = 0;
        bool aboutToRemoveRoots;

    };

    CtqProxyModel::CtqProxyModel(int offset, QObject* parent) : 
        QAbstractProxyModel(parent),
        impl(std::make_unique<CtqProxyModelImpl>(offset, this))
    {
    }

    CtqProxyModel::~CtqProxyModel() = default;

    void CtqProxyModel::setSourceModel(QAbstractItemModel* m)
    {
        if (const auto* model = sourceModel(); model != nullptr)
        {
            disconnect(model, &QAbstractItemModel::rowsAboutToBeInserted, this, &CtqProxyModel::SourceRowsAboutToBeInserted);
            disconnect(model, &QAbstractItemModel::rowsInserted, this, &CtqProxyModel::SourceRowsInserted);

            disconnect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &CtqProxyModel::SourceRowsAboutToBeRemoved);
            disconnect(model, &QAbstractItemModel::rowsRemoved, this, &CtqProxyModel::SourceRowsRemoved);

            disconnect(model, &QAbstractItemModel::dataChanged, this, &CtqProxyModel::SourceDataChanged);
            disconnect(model, &QAbstractItemModel::modelReset, this, &CtqProxyModel::SourceModelReset);

            disconnect(model, &QAbstractItemModel::layoutAboutToBeChanged, this, &CtqProxyModel::layoutAboutToBeChanged);
            disconnect(model, &QAbstractItemModel::layoutChanged, this , &CtqProxyModel::layoutChanged);
        }

        QAbstractProxyModel::setSourceModel(m);

        if (const auto* model = sourceModel(); model != nullptr) 
        {
            connect(model, &QAbstractItemModel::rowsAboutToBeInserted,this, &CtqProxyModel::SourceRowsAboutToBeInserted);
            connect(model, &QAbstractItemModel::rowsInserted, this, &CtqProxyModel::SourceRowsInserted);

            connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &CtqProxyModel::SourceRowsAboutToBeRemoved);
            connect(model, &QAbstractItemModel::rowsRemoved, this, &CtqProxyModel::SourceRowsRemoved);

            connect(model, &QAbstractItemModel::dataChanged, this, &CtqProxyModel::SourceDataChanged);
            connect(model, &QAbstractItemModel::modelReset, this, &CtqProxyModel::SourceModelReset);

            connect(model, &QAbstractItemModel::layoutAboutToBeChanged, this, &QAbstractItemModel::layoutAboutToBeChanged);
            connect(model, &QAbstractItemModel::layoutChanged, this, &QAbstractItemModel::layoutChanged);
        }

        SourceModelReset();
    }

    QModelIndex CtqProxyModel::mapFromSource(const QModelIndex& source) const
    {
        if (!sourceModel() || !source.parent().isValid())
        {
            return QModelIndex();
        }

        return index(impl->SourceToProxy(source), source.column());
    }

    QModelIndex CtqProxyModel::mapToSource(const QModelIndex& proxy) const
    {
        if (!sourceModel() || !proxy.isValid())
        {
            return QModelIndex();
        }
        auto p = impl->ProxyToSource(proxy.row());
        return sourceModel()->index(p.first, proxy.column(), p.second);
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
        
        return impl->RowCount();
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
    }

    void CtqProxyModel::SourceRowsInserted(const QModelIndex& p, int, int)
    {
        SourceModelReset();
    }

    void CtqProxyModel::SourceRowsAboutToBeRemoved(const QModelIndex& p, int from, int to)
    {
    }

    void CtqProxyModel::SourceRowsRemoved(const QModelIndex& p, int, int)
    {
        SourceModelReset();
    }

    void CtqProxyModel::SourceDataChanged(const QModelIndex& tl, const QModelIndex& br)
    {
        qDebug() << tl << br ;
        const auto p_tl = mapFromSource(tl);
        const auto p_br = mapFromSource(br);
        dataChanged(p_tl, p_br);
    }

    void CtqProxyModel::SourceModelReset()
    {
        impl->Reset();
        revert();
        layoutChanged();
    }
}