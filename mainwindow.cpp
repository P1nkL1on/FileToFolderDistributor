#include "mainwindow.h"

#include <QToolBar>
#include <QMenuBar>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>


const auto sortByDate = [](const QFileInfo &a, const QFileInfo &b){
    const auto aDate = a.lastModified();
    const auto bDate = b.lastModified();
    const qint64 delta = aDate.secsTo(bDate);
    return delta > 0 ? 1 : -1;
};

const auto sortByName = [](const QFileInfo &a, const QFileInfo &b){
    return a.fileName() > b.fileName();
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenuBar *menuBar = new QMenuBar;
    setMenuBar(menuBar);

    QMenu *files = menuBar->addMenu("Files");

    m_newFiles = files->addAction("Add Files", this, &MainWindow::addFiles, QKeySequence(Qt::CTRL + Qt::Key_O));
    files->addAction("Clear Files", this, &MainWindow::clearFiles, QKeySequence(Qt::CTRL + Qt::Key_N));
    files->addSeparator();
    m_newFolders = files->addAction("Add Folders", this, &MainWindow::addFolders, QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_O));
    files->addAction("Clear Folders", this, &MainWindow::clearFolders, QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_N));


    QMenu *sort = menuBar->addMenu("Sort");

    auto sortFilesNameAction = sort->addAction("Sort Files by Name", this, &MainWindow::sortFilesByName);
    auto sortFilesDateAction = sort->addAction("Sort Files by Date", this, &MainWindow::sortFilesByDate);
    QToolBar *sortFilesToolBar = new QToolBar;
    sortFilesToolBar->addActions({sortFilesNameAction, sortFilesDateAction});

    QWidget *cw = new QWidget;
    setCentralWidget(cw);

    QHBoxLayout *bl = new QHBoxLayout;
    cw->setLayout(bl);
    QVBoxLayout *fvl = new QVBoxLayout;
    bl->addLayout(fvl);
    fvl->addWidget(sortFilesToolBar);
    fvl->addWidget(m_filesList = new QListWidget, 1);

    QFrame *previewFrame = new QFrame;
    previewFrame->setContentsMargins(0, 0, 0, 0);
    m_previewLayout = new QVBoxLayout;
    previewFrame->setLayout(m_previewLayout);
    bl->addWidget(previewFrame, 1);

    bl->addWidget(m_foldersList = new QListWidget);

}

MainWindow::~MainWindow()
{

}

void MainWindow::updateFilesList()
{
    m_filesList->clear();
    for (const QFileInfo &f : m_files)
        m_filesList->addItem(f.fileName());
}

void MainWindow::sortFilesByName()
{
    std::sort(m_files.begin(), m_files.end(), sortByName);
    updateFilesList();
}

void MainWindow::sortFilesByDate()
{
    std::sort(m_files.begin(), m_files.end(), sortByDate);
    updateFilesList();
}

void MainWindow::addFiles()
{
    const QStringList newFilePathes = QFileDialog::getOpenFileNames(this, "Add Files", m_lastDialogDir);
    if (newFilePathes.isEmpty())
        return;
    for (const auto &path : newFilePathes){
        QFileInfo info(path);
        if (not m_files.contains(info))
            m_files << info;
    }
    updateFilesList();
}

void MainWindow::addFolders()
{

}

void MainWindow::clearFiles()
{

}

void MainWindow::clearFolders()
{

}
