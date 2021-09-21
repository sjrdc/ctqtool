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

#pragma once

#include <QDialog>
#include <QModelIndex>

class QAbstractItemModel;
class QCheckBox;
class QDialogButtonBox;
class QListWidget;
class QLabel;
class QLineEdit;
class QListWidgetItem;

namespace CtqTool
{
    class ItemDialog : public QDialog
    {
        Q_OBJECT

    public:
        ItemDialog(QAbstractItemModel*, QModelIndex, QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
        virtual ~ItemDialog();

    private:
        void MakeConnections() const;
        void MakeLayout();
        void OnListItemSelectionChanged();
        void OnFilterWidgetNameTextChanged(const QString& s);
        void OnTitleEditTextChanged(const QString& s);
        void OnNoteEditTextChanged(const QString& s);
        void OnActiveCheckBoxStateChanged(int state);
        void OnDownButtonClicked();
        void OnUpButtonClicked();
        void SetPropertiesFromFilter(QModelIndex index);

        static void SetButtonColor(QPushButton* b, const QColor& c);
        
        QAbstractItemModel* model = nullptr;
        QModelIndex index;
        QDialogButtonBox* buttonBox = nullptr;
        QLineEdit* titleEdit = nullptr;
        QLineEdit* noteEdit = nullptr;
    };
}
