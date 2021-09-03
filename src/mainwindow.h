#pragma once

#include <QMainWindow>

namespace CtqTool
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow();
        void LoadFile(const QString& filename);

        void dragEnterEvent(QDragEnterEvent* event);
        void dropEvent(QDropEvent* event);

    protected:
        void About();
        virtual void keyPressEvent(QKeyEvent* e);
        void OnLogWidgetStatusChanged(QString message);
        void Open();
        void OpenRecentFile();
        void OnReloadTriggered();
        void CopyLines();

    private:
        void MakeMenus();
        void MakeHelpMenu();
        void MakeFileMenu();
        void MakeEditMenu();
        void MakeTabMenu();
        void MakeViewMenu();
        void MakeStatusBar();

        void SetCurrentFile(const QString& fileName);
        void ShowMarkupFilters();
        void UpdateRecentFileActions();
        static void SetClipBoard(const QString&);

        static constexpr int numberOfRecentFiles = 10;

	QList<QAction*> recentFileActions;
    };
}
