#include "main_window.h"

#include <QToolBar>
#include <QMenuBar>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>

#include "preview_text.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initActions();
    initLayout();
    initPreviewers();
}

MainWindow::~MainWindow()
{
    if (m_currentPreview)
        dettachPreview();
    qDeleteAll(m_previews);
    delete m_defaultPreview;
}

void MainWindow::initLayout()
{
    QWidget *cw = new QWidget;
    setCentralWidget(cw);

    QHBoxLayout *bl = new QHBoxLayout;
    cw->setLayout(bl);
    QVBoxLayout *fvl = new QVBoxLayout;
    bl->addLayout(fvl);
    QToolBar *sortFilesToolBar = new QToolBar;
    sortFilesToolBar->addActions({m_sortFilesNameAction, m_sortFilesDateAction});
    fvl->addWidget(sortFilesToolBar);
    fvl->addWidget(m_filesList = new QListWidget, 1);

    QFrame *previewFrame = new QFrame;
    previewFrame->setContentsMargins(0, 0, 0, 0);
    m_previewLayout = new QVBoxLayout;
    previewFrame->setLayout(m_previewLayout);
    bl->addWidget(previewFrame, 1);

    bl->addWidget(m_foldersList = new QListWidget);

    connect(m_filesList, &QListWidget::currentRowChanged, this, &MainWindow::fileSelected);
}

void MainWindow::initActions()
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

    m_sortFilesNameAction = sort->addAction("Sort Files by Name", this, &MainWindow::sortFilesByName);
    m_sortFilesDateAction = sort->addAction("Sort Files by Date", this, &MainWindow::sortFilesByDate);
}

void MainWindow::initPreviewers()
{
    m_defaultPreview = new PreviewText;


}

void MainWindow::updateFilesList()
{
    m_filesList->clear();
    for (const QFileInfo &f : m_files)
        m_filesList->addItem(f.fileName());
}

void MainWindow::sortFilesByName()
{
    const auto sortByName = [](const QFileInfo &a, const QFileInfo &b){
        return a.fileName() > b.fileName();
    };
    std::sort(m_files.begin(), m_files.end(), sortByName);
    updateFilesList();
}

void MainWindow::sortFilesByDate()
{
    const auto sortByDate = [](const QFileInfo &a, const QFileInfo &b){
        const auto aDate = a.lastModified();
        const auto bDate = b.lastModified();
        const qint64 delta = aDate.secsTo(bDate);
        return delta > 0 ? 1 : -1;
    };
    std::sort(m_files.begin(), m_files.end(), sortByDate);
    updateFilesList();
}

void MainWindow::dettachPreview()
{
    Q_ASSERT(m_currentPreview);
    m_previewLayout->removeWidget(m_currentPreview);
    m_currentPreview = nullptr;
}

void MainWindow::attachPreview(PreviewFrame *p)
{
    Q_ASSERT(not m_currentPreview);
    Q_ASSERT(p);
    m_previewLayout->addWidget(m_currentPreview = p, 1);
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

void MainWindow::fileSelected(int ind)
{
    if (ind < 0){
        if (m_currentPreview)
            dettachPreview();
        return;
    }

    Q_ASSERT(ind < m_files.length());
    const QFileInfo fileInfo = m_files[ind];
    const QString ext = fileInfo.completeSuffix().toLower();


    // preview
    // find exact previewer
    PreviewFrame *preview = m_previews.value(ext, m_defaultPreview);
    Q_ASSERT(preview);

    // change preview if its another one
    if (m_currentPreview != preview){
        if (m_currentPreview)
            dettachPreview();
        attachPreview(preview);
    }

    m_currentPreview->previewFile(fileInfo.filePath());
}
