#include "ctqmodel.h"
#include "item.h"

#include <QStringList>

#include <iostream>

namespace CtqTool
{
 
    CtqModel::CtqModel(const QString& data, QObject* parent) :
        QAbstractItemModel(parent),
        rootItem(std::make_unique<TreeItem>(std::vector<QVariant>({tr("Title"), tr("Note")}), nullptr))
    {
        setupModelData(data.split('\n'), *rootItem);
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

        if (role != Qt::DisplayRole)
            return QVariant();

        TreeItem* item = static_cast<TreeItem*>(index.internalPointer());

        return item->Data(index.column());
    }
    
    Qt::ItemFlags CtqModel::flags(const QModelIndex& index) const
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        return QAbstractItemModel::flags(index);
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
    
    void CtqModel::setupModelData(const QStringList& lines, TreeItem& parent)
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
                std::vector<QVariant> columnData;
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
                parents.back()->Append(std::make_unique<TreeItem>(columnData, parents.back()));
            }
            ++number;
        }
    }

    bool CtqModel::setData(const QModelIndex& index, const QVariant &value, int role)
    {
        if (index.isValid())
        {
            static_cast<TreeItem*>(index.internalPointer())->SetData(index.column(), value);
            dataChanged(index, index);            
            return true;
        }
        else
        {
            return false;
        }
    }
}