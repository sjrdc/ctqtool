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

#include <QWidget>
#include "datamodel/ctqmodel.h"

class QTabWidget;
class QTableView;

namespace CtqTool
{
    class CtqTreeScene;
    class TreeView;
    class CtqModel;
    class CtqProxyModel;

    class CtqView : public QWidget
    {
        Q_OBJECT

    public:
        CtqView(QWidget* parent = nullptr);
        ~CtqView();

        void LoadFile(const QString& filename);
        
        void InsertChild();
        void InsertExistingChild();
        void InsertRow();
        void InsertExistingRow();
        void RemoveRow();
    private:

        void SetCurrentFile(const QString& fileName);
        void ShowMarkupFilters();
        void UpdateRecentFileActions();
        static void SetClipBoard(const QString&);
        void UpdateActions();


        CtqTreeScene* scene = nullptr;
        TreeView* treeView1 = nullptr;
        QTableView* needTable = nullptr;
        QTableView* driverTable = nullptr;
        QTableView* ctqTable = nullptr;
        QTabWidget* tabs = nullptr;

        std::unique_ptr<CtqModel> model;
        std::unique_ptr<CtqProxyModel> driversModel;
        std::unique_ptr<CtqProxyModel> needsModel;
        std::unique_ptr<CtqProxyModel> ctqsModel;
    };
}
