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
#include "itemdialog.h"
#include "treeview.h"
#include "utilities.h"

#include "datamodel/ctqmodel.h"
#include "datamodel/ctqproxymodel.h"

#include <QFile>
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>

namespace CtqTool
{
    CtqView::CtqView(QWidget* parent) :
        QWidget(nullptr),
        scene(new CtqTreeScene(this)),
        tree(new TreeView(this)),
        needTable(new QTableView(this)),
        driverTable(new QTableView(this)),
        ctqTable(new QTableView(this)),
        tabs(new QTabWidget(this))
    {
        QFile file("default.txt");
        file.open(QIODevice::ReadOnly);
        model = std::make_unique<CtqTool::CtqModel>(file.readAll());
                
        needsModel = std::make_unique<CtqProxyModel>(1, this);
        needsModel->setSourceModel(model.get());
        needTable->setModel(needsModel.get());

        driversModel = std::make_unique<CtqProxyModel>(2, this);
        driversModel->setSourceModel(model.get());
        driverTable->setModel(driversModel.get());
        
        file.close();
        
        tree->setModel(model.get());

        ctqsModel = std::make_unique<CtqProxyModel>(3, this);
        ctqsModel->setSourceModel(model.get());
        ctqTable->setModel(ctqsModel.get());

        for (int column = 0; column < model->columnCount(); ++column)
        {
            tree->resizeColumnToContents(column);
            needTable->resizeColumnToContents(column);
            driverTable->resizeColumnToContents(column);
            ctqTable->resizeColumnToContents(column);
        }

        auto* layout = new QVBoxLayout(this);
        layout->addWidget(tree);
        layout->addWidget(tabs);
        setLayout(layout);

        tabs->addTab(needTable, "needs");
        tabs->addTab(driverTable, "drivers");
        tabs->addTab(ctqTable, "CTQs");
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

    void CtqView::InsertExistingChild()
    {
        const auto index = tree->selectionModel()->currentIndex();
        auto *model = tree->model();

        if (model->columnCount(index) == 0) 
        {
            if (!model->insertColumn(0, index))
                return;
        }

        if (!model->insertRow(0, index))
            return;

        for (int column = 0; column < model->columnCount(index); ++column) 
        {
            const auto child = model->index(0, column, index);
            model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
            if (!model->headerData(column, Qt::Horizontal).isValid())
            {
                model->setHeaderData(column, Qt::Horizontal, QVariant(tr("[No header]")), Qt::EditRole);
            }
        }

        tree->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                                QItemSelectionModel::ClearAndSelect);
        UpdateActions();
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
