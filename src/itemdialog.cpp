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
#include <QCheckBox>
#include <QColorDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace CtqTool
{
    ItemDialog::ItemDialog(QAbstractItemModel* m, QModelIndex i, QWidget* parent, Qt::WindowFlags flags) :
        QDialog(parent, flags),
        model(m),
        index(i),
        buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok)),
        titleEdit(new QLineEdit("<title>")),
        noteEdit(new QLineEdit("<note>"))
    {
        this->setWindowTitle("Item properties");
        MakeConnections();
        MakeLayout();
        SetPropertiesFromFilter(index);
    }

    ItemDialog::~ItemDialog() = default;

    void ItemDialog::MakeConnections() const
    {
        connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(titleEdit, &QLineEdit::textChanged, this, &ItemDialog::OnTitleEditTextChanged);
        connect(noteEdit, &QLineEdit::textChanged, this, &ItemDialog::OnNoteEditTextChanged);
    }

    void ItemDialog::MakeLayout()
    {
        auto* title = new QLabel("Title", this);
        auto* note = new QLabel("Note", this);

        auto* filterlayout = new QGridLayout;
        auto row = 0;
        filterlayout->addWidget(title, row, 0, 1, 1);
        filterlayout->addWidget(titleEdit, row++, 1, 1, 1);
        filterlayout->addWidget(note, row, 0, 1, 1);
        filterlayout->addWidget(noteEdit, row++, 1, 1, 1);

        auto* layout = new QVBoxLayout;
        layout->addLayout(filterlayout);
        layout->addWidget(buttonBox);

        this->setLayout(layout);
    }

    void ItemDialog::OnListItemSelectionChanged()
    {
    }

    void ItemDialog::AddEmptyFilter()
    {
    }

    void ItemDialog::RemoveFilter()
    {

    }

    void ItemDialog::SetPropertiesFromFilter(QModelIndex index)
    {
        if (index.row() >= 0/*  && index < filters->size() */)
        {
            titleEdit->setText(model->data(index, 0).toString());
            const auto noteIndex = model->index(index.row(), 1);
            noteEdit->setText(model->data(noteIndex, 0).toString());
        }
        else
        {
            titleEdit->clear();
            noteEdit->clear();
        }
    }

    void ItemDialog::OnTitleEditTextChanged(const QString& s)
    {
        const auto titleIndex = model->index(index.row(), 0, index.parent());
        model->setData(titleIndex, titleEdit->text());
    }

    void ItemDialog::OnNoteEditTextChanged(const QString& s)
    {
        const auto noteIndex = model->index(index.row(), 1, index.parent());
        model->setData(noteIndex, noteEdit->text()); 
    }
}
