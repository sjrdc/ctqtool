#include "ctqmodel.h"
#include "item.h"

#include <QItemSelection>
#include <QStringList>

#include <iostream>

namespace CtqTool
{
 
    CtqModel::CtqModel(const QString& data, QObject* parent) :
        QAbstractItemModel(parent),
        rootItem(std::make_unique<TreeItem>(std::make_shared<Item>("Title", "Note"), nullptr))
    {
        SetupModelData(data.split('\n'), *rootItem);
    }

    CtqModel::~CtqModel() = default;

    int CtqModel::columnCount(const QModelIndex& parent) const
    {
        if (parent.isValid())
            return static_cast<TreeItem*>(parent.internalPointer())->ColumnCount();
        return rootItem->ColumnCount();
    }
    
    QVariant CtqModel::data(const QModelIndex& index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        if (role != Qt::DisplayRole && role != Qt::EditRole)
            return QVariant();

        TreeItem* item = static_cast<TreeItem*>(index.internalPointer());

        return item->Data(index.column());
    }
    
    Qt::ItemFlags CtqModel::flags(const QModelIndex& index) const
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    }
    
    QVariant CtqModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return rootItem->Data(section);

        return QVariant();
    }
    
    QModelIndex CtqModel::index(int row, int column, const QModelIndex& parent) const
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        TreeItem* parentItem;

        if (!parent.isValid())
            parentItem = rootItem.get();
        else
            parentItem = static_cast<TreeItem*>(parent.internalPointer());

        const auto& childItem = parentItem->GetChild(row);
        return (childItem) ? createIndex(row, column, childItem.get()) : QModelIndex();
    }
    
    QModelIndex CtqModel::parent(const QModelIndex& index) const
    {
        if (!index.isValid())
            return QModelIndex();

        auto* childItem = static_cast<TreeItem*>(index.internalPointer());
        auto* parentItem = childItem->GetParent();

        if (parentItem == rootItem.get())
            return QModelIndex();

        return createIndex(parentItem->Row(), 0, parentItem);
    }
    
    int CtqModel::rowCount(const QModelIndex& parent) const
    {
        TreeItem* parentItem;
        if (parent.column() > 0)
            return 0;

        if (!parent.isValid())
            parentItem = rootItem.get();
        else
            parentItem = static_cast<TreeItem*>(parent.internalPointer());

        return parentItem->ChildCount();
    }
    
    void CtqModel::SetupModelData(const QStringList& lines, TreeItem& parent)
    {
        std::vector<TreeItem*> parents;
        QVector<int> indentations;        
        parents.push_back(&parent);
        indentations << 0;

        auto number = 0;
        while (number < lines.count()) 
        {
            int position = 0;
            while (position < lines[number].length())
            {
                if (lines[number].at(position) != ' ')
                    break;
                position++;
            }

            const QString lineData = lines[number].mid(position).trimmed();

            if (!lineData.isEmpty()) 
            {
                // Read the column data from the rest of the line.
                const QStringList columnStrings =lineData.split(QLatin1Char('\t'), Qt::SkipEmptyParts);
                std::vector<QString> columnData;
                columnData.reserve(columnStrings.count());

                for (const auto& columnString : columnStrings)
                {
                    columnData.push_back(columnString);
                }

                if (position > indentations.back()) 
                {
                    // The last child of the current parent is now the new parent
                    // unless the current parent has no children.
                    const auto& lastParent = parents.back();
                    if (lastParent->ChildCount() > 0) 
                    {
                        parents.push_back(lastParent->GetChild(lastParent->ChildCount() - 1).get());
                        indentations << position;
                    }
                }
                else
                {
                    while (position < indentations.back() && parents.size() > 0)
                    {
                        parents.pop_back();
                        indentations.pop_back();
                    }
                }

                // Append a new item to the current parent's list of children.
                parents.back()->Append(std::make_unique<TreeItem>(std::make_shared<Item>(columnData[0], columnData[1]), parents.back()));
            }
            ++number;
        }
    }

    bool CtqModel::setData(const QModelIndex& index, const QVariant &value, int role)
    {
        if (index.isValid())
        {
            static_cast<TreeItem*>(index.internalPointer())->SetData(index.column(), value.toString());
            dataChanged(index, index);            
            return true;
        }
        else
        {
            return false;
        }
    }

    TreeItem* CtqModel::GetItem(const QModelIndex &index) const
    {
        if (index.isValid()) 
        {
            TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
            if (item)
                return item;
        }
        return rootItem.get();
    }

    bool CtqModel::insertRows(int position, int rows, const QModelIndex &parent)
    {
        auto* parentItem = GetItem(parent);
        if (!parentItem)
            return false;

        beginInsertRows(parent, position, position + rows - 1);
        const auto success = parentItem->InsertChildren(position,
                                                        rows,
                                                        rootItem->ColumnCount());
        endInsertRows();

        return success;
    }

    bool CtqModel::removeRows(int position, int rows, const QModelIndex &parent)
    {
        TreeItem *parentItem = GetItem(parent);
        if (!parentItem)
            return false;

        beginRemoveRows(parent, position, position + rows - 1);
        const bool success = parentItem->RemoveChildren(position, rows);
        endRemoveRows();

        return success;
    }

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