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

#include "itemdialog.h"

#include <QAbstractItemModel>
#include <QDialogButtonBox>
#include <QListView>
#include <QVBoxLayout>

namespace CtqTool
{
    PickDialog::PickDialog(QAbstractItemModel* m, QWidget* parent, Qt::WindowFlags flags) :
        QDialog(parent, flags),
        model(m),
        buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel)),
        view(new QListView(this))
    {
        this->setWindowTitle("Item properties");
        view->setModel(model);
        MakeLayout();
        
        connect(view, &QListView::clicked, [this](const auto& idx) { this->selectedIndex = idx; });
        connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }

    PickDialog::~PickDialog() = default;

    void PickDialog::MakeLayout()
    {
        auto* layout = new QVBoxLayout;
        layout->addWidget(view);
        layout->addWidget(buttonBox);
        this->setLayout(layout);
    }

    QModelIndex PickDialog::GetSelection() const
    {
        return selectedIndex;
    }
}
