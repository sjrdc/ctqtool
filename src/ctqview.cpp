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

#include "ctqview.h"

#include "ctqtreescene.h"
#include "treeview.h"
#include "utilities.h"
#include "itemdialog.h"

#include "datamodel/ctqmodel.h"
#include "datamodel/ctqproxymodel.h"
#include "datamodel/item.h"

#include <QFile>
#include <QSplitter>
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>

namespace
{
    auto getDepth(const QModelIndex& idx)
    {
        auto depth = 0;
        auto q = idx;
        while (q.parent().isValid())
        {
            q = q.parent();
            depth++;
        }
        return depth;
    }
}

namespace CtqTool
{
    CtqView::CtqView(QWidget* parent) :
        QWidget(parent),
        scene(new CtqTreeScene(this)),
        tree(new TreeView(this)),
        needTable(new QTableView(this)),
        driverTable(new QTableView(this)),
        ctqTable(new QTableView(this)),
        tabs(new QTabWidget(this))
    {                
        needsModel = std::make_unique<CtqProxyModel>(1, this);
        needTable->setModel(needsModel.get());

        driversModel = std::make_unique<CtqProxyModel>(2, this);
        driverTable->setModel(driversModel.get());
         
        tree->setModel(model.get());

        ctqsModel = std::make_unique<CtqProxyModel>(3, this);
        ctqTable->setModel(ctqsModel.get());

        model = std::make_unique<CtqModel>();
        tree->setModel(model.get());
        needsModel->setSourceModel(model.get());
        driversModel->setSourceModel(model.get());
        ctqsModel->setSourceModel(model.get());
        for (int column = 0; column < model->columnCount(); ++column)
        {
            tree->resizeColumnToContents(column);
            needTable->resizeColumnToContents(column);
            driverTable->resizeColumnToContents(column);
            ctqTable->resizeColumnToContents(column);
        }

        tabs->addTab(needTable, "needs");
        tabs->addTab(driverTable, "drivers");
        tabs->addTab(ctqTable, "CTQs");

        auto* splitter = new QSplitter(Qt::Vertical, this);
        splitter->addWidget(tree);
        splitter->addWidget(tabs);
        splitter->setStretchFactor(0, 2);
        splitter->setStretchFactor(1, 1);

        auto* layout = new QVBoxLayout(this);
        layout->addWidget(splitter);
        setLayout(layout);
    }

    CtqView::~CtqView() = default;

    void CtqView::InsertRow()
    {
        const auto index = tree->selectionModel()->currentIndex();
        auto* model = tree->model();

        if (!model->insertRow(index.row()+1, index.parent()))
            return;

        UpdateActions();

        for (int column = 0; column < model->columnCount(index.parent()); ++column) 
        {
            const auto child = model->index(index.row() + 1, column, index.parent());
            model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
        }
    }

    void CtqView::InsertExistingRow()
    {
        const auto index = tree->selectionModel()->currentIndex();
        auto* model = tree->model();

        if (!model->insertRow(index.row()+1, index.parent()))
            return;

        UpdateActions();

        for (int column = 0; column < model->columnCount(index.parent()); ++column) 
        {
            const auto child = model->index(index.row() + 1, column, index.parent());
            model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
        }
    }

    void CtqView::RemoveRow()
    {
        const auto index = tree->selectionModel()->currentIndex();
        auto* model = tree->model();
        if (model->removeRow(index.row(), index.parent()))
            UpdateActions();
    }
        
    void CtqView::InsertChild()
    {
        const auto currentIndex = tree->selectionModel()->currentIndex();
        auto *model = tree->model();

        if (model->columnCount(currentIndex) == 0) 
        {
            if (!model->insertColumn(0, currentIndex))
                return;
        }

        if (!model->insertRow(0, currentIndex))
            return;

        for (int column = 0; column < model->columnCount(currentIndex); ++column) 
        {
            const auto child = model->index(0, column, currentIndex);
            model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
            if (!model->headerData(column, Qt::Horizontal).isValid())
            {
                model->setHeaderData(column, Qt::Horizontal, QVariant(tr("[No header]")), Qt::EditRole);
            }
        }

        tree->selectionModel()->setCurrentIndex(model->index(0, 0, currentIndex),
            QItemSelectionModel::ClearAndSelect);
        UpdateActions();
    }

    int getDepth(const QModelIndex& idx)
    {
        auto depth = 1;
        auto q = idx;
        while (q.parent().isValid())
        {
            q = q.parent();
            depth++;
        }
        return depth;
    }

    auto makeProxy(QAbstractItemModel* m, QModelIndex i)
    {
        auto* proxy = new CtqProxyModel(1 + getDepth(i));
        proxy->setSourceModel(m);
        return proxy;
    }

    auto getItemData(QAbstractItemModel* model, const QModelIndex& idx)
    {
        QVector<QMap<int, QVariant>> data;
        data.reserve(model->columnCount());
        for (int c = 0; c < model->columnCount(); ++c)
        {
            data.push_back(model->itemData(model->index(idx.row(), c, idx.parent())));
        }
        return data;
    }
    
    void CtqView::InsertExistingChild()
    {
        const auto index = tree->selectionModel()->currentIndex();
        auto* model = tree->model();

        auto* proxy = makeProxy(model, index);
        auto dialog = PickDialog(proxy, this);
        if (dialog.exec() == QDialog::Accepted)
        {
            const auto currentIndex = tree->selectionModel()->currentIndex();
            const auto idx = proxy->mapToSource(dialog.GetSelection());
            auto data = getItemData(model, proxy->mapToSource(dialog.GetSelection()));

            if (model->columnCount(currentIndex) == 0) 
            {
                if (!model->insertColumn(0, currentIndex))
                    return;
            }

            if (!model->insertRow(0, currentIndex))
                return;
    
            const auto& existingItem = *static_cast<TreeItem*>(idx.internalPointer());
            auto& child = *static_cast<TreeItem*>(model->index(0, 0, currentIndex).internalPointer());
            child.CloneDataFrom(existingItem);
            
            tree->selectionModel()->setCurrentIndex(model->index(0, 0, currentIndex),
                QItemSelectionModel::ClearAndSelect);
            UpdateActions();
        }
    }
    
    void CtqView::UpdateActions()
    {
        const auto hasSelection = !tree->selectionModel()->selection().isEmpty();
        const auto hasCurrent = tree->selectionModel()->currentIndex().isValid();
        if (hasCurrent) 
        {
            tree->closePersistentEditor(tree->selectionModel()->currentIndex());
        } 
    }

}
