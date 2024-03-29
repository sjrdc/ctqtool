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

#include <QMainWindow>
#include "datamodel/ctqmodel.h"

namespace CtqTool
{
    class CtqTreeScene;
    class CtqView;

    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow();
        void LoadFile(const QString& filename);

        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);
    
    private:
        void About();
        void Save();
        void SaveAs();
        virtual void keyPressEvent(QKeyEvent* e);
        void OnLogWidgetStatusChanged(QString message);
        void Open();
        void OpenRecentFile();
        void OnReloadTriggered();
        void CopyLines();

        void MakeMenus();
        void MakeHelpMenu();
        void MakeFileMenu();
        void MakeEditMenu();
        void MakeViewMenu();
        void MakeStatusBar();

        void SetCurrentFile(const QString& fileName);
        void ShowMarkupFilters();
        void UpdateRecentFileActions();
        static void SetClipBoard(const QString&);

        static constexpr int numberOfRecentFiles = 10;

        QList<QAction*> recentFileActions;
        CtqTreeScene* scene;
        CtqView* view;
    };
}
