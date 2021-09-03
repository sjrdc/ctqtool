#include "mainwindow.h"
#include "utilities.h"

#include <QtWidgets>

#define _CTQ_VERSION_ "0.0.1"

namespace
{
    auto* MakeAction(const QString& title, QObject* parent, const QKeySequence& shortcut)
    {
        auto* action = new QAction(title, parent);
        action->setShortcut(shortcut);
        return action;
    }
}

namespace CtqTool
{
    MainWindow::MainWindow()
    {
        this->setAcceptDrops(true);

        MakeMenus();
        MakeStatusBar();

        setUnifiedTitleAndToolBarOnMac(true);

        constexpr auto defaultWidth = 1200;
        constexpr auto defaultHeight = 800;
        resize(defaultWidth, defaultHeight);
    }

    void MainWindow::About()
    {
        QMessageBox::about(this, tr("About x"),
            tr("<p style=\"text-align: center;\"><b>CTQ tool</b> demonstrates a CTQ explorer in Qt.<br>"
                "Version " _CTQ_VERSION_ ", " __DATE__ "<br>"
                "&copy; Sjoerd Crijns</p>"));
    }

    void MainWindow::MakeMenus()
    {
        MakeFileMenu();
        MakeEditMenu();
        MakeTabMenu();
        MakeViewMenu();
        MakeHelpMenu();
    }

    void MainWindow::MakeHelpMenu()
    {
        auto* helpMenu = menuBar()->addMenu(tr("&Help"));
        auto* about = new QAction(tr("&About"), this);
        about->setStatusTip(tr("Show the application's About box"));
        connect(about, &QAction::triggered, this, &MainWindow::About);
        helpMenu->addAction(about);

        auto* aboutQt = new QAction(tr("About& Qt"), (dynamic_cast<QApplication *>(QCoreApplication::instance())));
        aboutQt->setStatusTip(tr("Show the Qt library's About box"));
        connect(aboutQt, &QAction::triggered, (dynamic_cast<QApplication *>(QCoreApplication::instance())), &QApplication::aboutQt);
        helpMenu->addAction(aboutQt);
    }

    void MainWindow::MakeEditMenu()
    {
        auto* editMenu = menuBar()->addMenu(tr("&Edit"));
    }

    void MainWindow::SetClipBoard(const QString& text)
    {
        if (!text.isEmpty())
        {
            QApplication::clipboard()->setText(text);
        }
    }
    
    void MainWindow::CopyLines()
    {
    }

    
    void MainWindow::MakeTabMenu()
    {
        auto* tabMenu = menuBar()->addMenu(tr("&Tabs"));
    }

    void MainWindow::MakeFileMenu()
    {
        auto* fileMenu = menuBar()->addMenu(tr("&File"));
        
        auto* open = MakeAction(tr("&Open..."), this, QKeySequence::Open);
        connect(open, &QAction::triggered, this, &MainWindow::Open);
        fileMenu->addAction(open);

        auto* reloadAction = MakeAction(tr("&Reload"), this, QKeySequence(QKeySequence::Refresh));
        connect(reloadAction, &QAction::triggered, this, &MainWindow::OnReloadTriggered);
        fileMenu->addAction(reloadAction);

        fileMenu->addSeparator();

        for (auto i = 0; i < numberOfRecentFiles; ++i)
        {
            recentFileActions.push_back(new QAction(this));
            recentFileActions[i]->setVisible(false);
            connect(recentFileActions[i], &QAction::triggered, this, &MainWindow::OpenRecentFile);
            fileMenu->addAction(recentFileActions[i]);
        }

        fileMenu->addSeparator();
        auto* quit = MakeAction(tr("&Quit"), this, QKeySequence::Quit);
        connect(quit, &QAction::triggered, this, &QWidget::close);
        fileMenu->addAction(quit);

        UpdateRecentFileActions();
    }

    void MainWindow::MakeViewMenu()
    {
        auto* viewMenu = menuBar()->addMenu(tr("&View"));
    }

    void MainWindow::MakeStatusBar()
    {
        statusBar()->showMessage(tr("Ready"));
    }

    void MainWindow::Open()
    {
        QFileDialog dialog(this, "Open log file...");
        dialog.setFileMode(QFileDialog::ExistingFile);
        dialog.setNameFilter(tr("Log file (*.log)"));
        dialog.setViewMode(QFileDialog::Detail);

        if (dialog.exec() == QDialog::Accepted)
        {
	    // todo implement
        }
    }

    void MainWindow::LoadFile(const QString& filename)
    {
        if (!FileExists(filename))
        {
            QMessageBox::critical(this, "Error opening file...", "File " + filename + " does not exist.");
            return;
        }

	// todo implement
	
        SetCurrentFile(filename);
    }

    void MainWindow::dragEnterEvent(QDragEnterEvent* event)
    {
        if (event->mimeData()->hasUrls())
        {
            event->acceptProposedAction();
        }
    }

    void MainWindow::dropEvent(QDropEvent* event)
    {
        const auto mimeData = event->mimeData();

        // check for our needed mime type, here a file or a list of files
        if (mimeData->hasUrls())
        {
            const auto urlList = mimeData->urls();
            QStringList pathList;
            // extract the local paths of the files
            for (int i = 0; i < urlList.size() && i < 32; ++i)
            {
                pathList.append(urlList.at(i).toLocalFile());
            }

            // call a function to open the files
            LoadFile(pathList[0]);
        }
    }

    void MainWindow::UpdateRecentFileActions()
    {
        // auto recentFiles = settings->GetRecentFiles();
        // const auto numRecentFiles = qMin(recentFiles.size(), numberOfRecentFiles);
        // for (auto i = 0; i < numRecentFiles; ++i)
        // {
        //     const auto text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(recentFiles[i]).fileName());
        //     recentFileActions[i]->setText(text);
        //     recentFileActions[i]->setData(recentFiles[i]);
        //     recentFileActions[i]->setVisible(true);
        // }
        // for (auto j = numRecentFiles; j < numberOfRecentFiles; ++j)
        // {
        //     recentFileActions[j]->setVisible(false);
        // }
    }

    void MainWindow::OpenRecentFile()
    {
        const auto action = qobject_cast<QAction*>(sender());
        if (action)
        {
            LoadFile(action->data().toString());
        }
    }

    void MainWindow::SetCurrentFile(const QString& fileName)
    {
        setWindowTitle(fileName);

        // auto recentFiles = settings->GetRecentFiles();
        // recentFiles.removeAll(fileName);
        // recentFiles.prepend(fileName);
        // while (recentFiles.size() > numberOfRecentFiles)
        // {
        //     recentFiles.removeLast();
        // }

        // settings->SetRecentFiles(recentFiles);

        for (const auto& widget : QApplication::topLevelWidgets())
        {
            const auto mainWin = qobject_cast<MainWindow*>(widget);
            if (mainWin)
            {
                mainWin->UpdateRecentFileActions();
            }
        }
    }

    void MainWindow::keyPressEvent(QKeyEvent* e)
    {
        switch (e->key())
        {
        default:
            e->ignore();
            break;
        }
    }

    void MainWindow::OnLogWidgetStatusChanged(QString message)
    {
        statusBar()->showMessage(message);
    }

    void MainWindow::OnReloadTriggered()
    {
    }
}
