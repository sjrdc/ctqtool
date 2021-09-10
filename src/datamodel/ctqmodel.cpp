#include "ctqmodel.h"
#include "item.h"

#include <QStringList>

#include <iostream>

namespace CtqTool
{
 
    CtqModel:: CtqModel(const QString& data, QObject* parent) :
        QAbstractItemModel(parent),
        rootItem(new TreeItem({tr("Title"), tr("Summary")}))
    {
        setupModelData(data.split('\n'), rootItem);
    }
    
    CtqModel::~CtqModel()
    {
        delete rootItem;
    }
    
    int  CtqModel::columnCount(const QModelIndex& parent) const
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
    
    Qt::ItemFlags  CtqModel::flags(const QModelIndex& index) const
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        return QAbstractItemModel::flags(index);
    }
    
    QVariant  CtqModel::headerData(int section, Qt::Orientation orientation,
                                int role) const
    {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
            return rootItem->Data(section);

        return QVariant();
    }
    
    QModelIndex  CtqModel::index(int row, int column, const QModelIndex& parent) const
    {
        if (!hasIndex(row, column, parent))
            return QModelIndex();

        TreeItem* parentItem;

        if (!parent.isValid())
            parentItem = rootItem;
        else
            parentItem = static_cast<TreeItem*>(parent.internalPointer());

        TreeItem* childItem = parentItem->GetChild(row);
        if (childItem)
            return createIndex(row, column, childItem);
        return QModelIndex();
    }
    
    QModelIndex  CtqModel::parent(const QModelIndex& index) const
    {
        if (!index.isValid())
            return QModelIndex();

        auto* childItem = static_cast<TreeItem*>(index.internalPointer());
        auto* parentItem = childItem->ParentItem();

        if (parentItem == rootItem)
            return QModelIndex();

        return createIndex(parentItem->Row(), 0, parentItem);
    }
    
    int  CtqModel::rowCount(const QModelIndex& parent) const
    {
        TreeItem* parentItem;
        if (parent.column() > 0)
            return 0;

        if (!parent.isValid())
            parentItem = rootItem;
        else
            parentItem = static_cast<TreeItem*>(parent.internalPointer());

        return parentItem->ChildCount();
    }
    
    void  CtqModel::setupModelData(const QStringList& lines, TreeItem* parent)
    {
        QVector<TreeItem*> parents;
        QVector<int> indentations;
        parents << parent;
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
                QVector<QVariant> columnData;
                columnData.reserve(columnStrings.count());

                for (const auto& columnString : columnStrings)
                {
                    columnData << columnString;
                    std::cout << columnString.toStdString() << std::endl;
                }

                if (position > indentations.last()) 
                {
                    // The last child of the current parent is now the new parent
                    // unless the current parent has no children.

                    if (parents.last()->ChildCount() > 0) 
                    {
                        parents << parents.last()->GetChild(parents.last()->ChildCount()-1);
                        indentations << position;
                    }
                }
                else
                {
                    while (position < indentations.last() && parents.count() > 0)
                     {
                        parents.pop_back();
                        indentations.pop_back();
                    }
                }

                // Append a new item to the current parent's list of children.
                parents.last()->Append(new TreeItem(columnData, parents.last()));
            }
            ++number;
        }
    }
}