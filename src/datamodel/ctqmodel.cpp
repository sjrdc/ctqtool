#include "ctqmodel.h"
#include "item.h"

#include <QItemSelection>
#include <QStringList>

namespace
{
    constexpr auto textColumn = 0;
    constexpr auto noteColumn = 1;
    constexpr auto rankColumn = 2;
}
namespace CtqTool
{
    size_t depth(const QModelIndex& idx)
    {
        auto parent = idx.parent();
        size_t depth = 1; // starting from invalid root item
        while (parent.isValid())
        {
            parent = parent.parent();
            ++depth;
        }

        return depth;
    }

    CtqModel::CtqModel(const QString& data, QObject* parent) :
        QAbstractItemModel(parent),
        rootItem(std::make_unique<TreeItem>(std::make_shared<ItemData>("Title", "Note"), nullptr))
    {
        SetupModelData(data.split('\n'), *rootItem);
        connect(this, &QAbstractItemModel::dataChanged, this, &CtqModel::OnDataChanged);
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
        {
            return QVariant();
        }

        if (role != Qt::DisplayRole && role != Qt::EditRole)
            return QVariant();

        TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
        return (item != nullptr) ? item->Data(index.column()) : QVariant();
    }
    
    Qt::ItemFlags CtqModel::flags(const QModelIndex& index) const
    {
        Qt::ItemFlags flags;

        if (!index.isValid())
            flags = Qt::NoItemFlags;
        else if (depth(index) == 2 && index.column() == rankColumn)
            flags = QAbstractItemModel::flags(index);
        else
            flags = QAbstractItemModel::flags(index) | Qt::ItemIsEditable;

        return flags;
    }
    
    QVariant CtqModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        {
            switch (section)
            {
            case textColumn:
                return "Text";
            case noteColumn:
                return "Note";
            case rankColumn:
                return "Rank";
            default:
                return QVariant();
            }
        }
        
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

        return (parentItem != nullptr) ? parentItem->ChildCount() : 0;
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
                parents.back()->Append(std::make_unique<TreeItem>(std::make_shared<ItemData>(columnData[0], columnData[1]), parents.back()));
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
        if (depth(parent) == maxDepth)
        {
            return false;
        }
        auto* parentItem = GetItem(parent);
        if (!parentItem)
        {
            return false;
        }

        beginInsertRows(parent, position, position + rows - 1);
        const auto success = parentItem->InsertChildren(position,
                                                        rows,
                                                        rootItem->ColumnCount());
        endInsertRows();

        return success;
    }

    bool CtqModel::removeRows(int position, int rows, const QModelIndex &parent)
    {
        auto* parentItem = GetItem(parent);
        if (!parentItem)
            return false;

        beginRemoveRows(parent, position, position + rows - 1);
        const auto success = parentItem->RemoveChildren(position, rows);
        endRemoveRows();

        return success;
    }

    void CtqModel::OnDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>&)
    {
        if (topLeft.column() <= rankColumn && bottomRight.column() >= rankColumn)
        {
            auto* item = GetItem(bottomRight);
            item->PropagateRank();
        }
    }
}
