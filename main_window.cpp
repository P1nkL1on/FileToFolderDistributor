#include "main_window.h"

#include <QToolBar>
#include <QMenuBar>
#include <QFileDialog>
#include <QDateTime>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QMessageBox>
#include <QHeaderView>

#include <QDebug>

#include "preview_text.h"
#include "preview_image.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setAcceptDrops(true);


    initActions();
    initLayout();
    initPreviewers();
}

MainWindow::~MainWindow()
{
    if (m_currentPreview)
        dettachPreview();
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
    m_filesList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_filesList->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_filesList->setFocusPolicy(Qt::NoFocus);

    QFrame *previewFrame = new QFrame;
    previewFrame->setContentsMargins(0, 0, 0, 0);
    m_previewLayout = new QVBoxLayout;
    previewFrame->setLayout(m_previewLayout);
    bl->addWidget(previewFrame, 1);

    bl->addWidget(m_foldersList = new QTableWidget);
    m_foldersList->setFocusPolicy(Qt::NoFocus);
    m_foldersList->setSelectionMode(QAbstractItemView::NoSelection);
    m_foldersList->setColumnCount(3);
    m_foldersList->verticalHeader()->hide();
    m_foldersList->setHorizontalHeaderLabels({"Shortcut", "Folder", "Moved"});
    m_foldersList->setColumnWidth(0, 100);
    m_foldersList->setColumnWidth(1, 200);
    m_foldersList->setColumnWidth(2, 50);
    m_foldersList->setFixedWidth(355);
    m_foldersList->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    connect(m_filesList, &QListWidget::currentRowChanged, this, &MainWindow::fileSelected);
}

void MainWindow::initActions()
{
    QMenuBar *menuBar = new QMenuBar;
    setMenuBar(menuBar);

    QMenu *files = menuBar->addMenu("Files");

    m_newFiles = files->addAction("Add Files", this, static_cast<void(MainWindow::*)(void)>(&MainWindow::addFiles), QKeySequence(Qt::CTRL + Qt::Key_O));
    files->addAction("Clear Files", this, &MainWindow::clearFiles, QKeySequence(Qt::CTRL + Qt::Key_N));
    files->addSeparator();
    m_newFolders = files->addAction("Add Folders", this, static_cast<void(MainWindow::*)(void)>(&MainWindow::addFolders), QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_O));
    files->addAction("Clear Folders", this, &MainWindow::clearFolders, QKeySequence(Qt::SHIFT + Qt::CTRL + Qt::Key_N));


    QMenu *sort = menuBar->addMenu("Sort");

    m_sortFilesNameAction = sort->addAction("Sort Files by Name", this, &MainWindow::sortFilesByName);
    m_sortFilesDateAction = sort->addAction("Sort Files by Date", this, &MainWindow::sortFilesByDate);
}

void MainWindow::initPreviewers()
{
    m_defaultPreview = new PreviewText;

    auto image = new PreviewImage;
    for (const QString &ext : {"png", "bmp", "jpg", "jpeg", "svg"})
        m_previews.insert(ext, image);
}


void MainWindow::updateFilesList()
{
    m_filesList->clear();
    for (const QFileInfo &f : m_files)
        m_filesList->addItem(f.fileName());
}

void MainWindow::updateFoldersList()
{
    int row = 0;
    m_foldersList->setRowCount(m_folders.length());

    for (const QFileInfo &f : m_folders){
        m_foldersList->setItem(row, 1, new QTableWidgetItem(f.fileName()));
        ++row;
    }
}

void MainWindow::sortFilesByName()
{
    const auto sortByName = [](const QFileInfo &a, const QFileInfo &b){
        return a.fileName() < b.fileName();
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
    m_currentPreview->hide();
    m_currentPreview = nullptr;
}

void MainWindow::attachPreview(PreviewFrame *p)
{
    Q_ASSERT(not m_currentPreview);
    Q_ASSERT(p);
    m_previewLayout->addWidget(m_currentPreview = p, 1);
    m_currentPreview->show();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();
    if (not mimeData->hasUrls())
        return;
    QList<QUrl> urlList = mimeData->urls();
    QStringList files;
    QStringList folders;
    QStringList ignored;

    for (const QUrl &url : urlList){
        const QString path = url.toLocalFile();
        QFileInfo info(path);
        if (info.isFile())
            files << path;
        else if (info.isDir())
            folders << path;
        else
            ignored << path;
    }
    if (not files.isEmpty())
        addFiles(files);
    if (not folders.isEmpty())
        addFolders(folders);

    if (not ignored.isEmpty()){
        QMessageBox msg(this);
        msg.setWindowTitle("Warning");
        const int count = ignored.length();
        const QString text = count > 1 ?
                    QString("%1 urls are unresolved!").arg(count)
                  : QString("'%1' url is unresolved!").arg(ignored.first());
        QString details =
                QString("They can't be threaten as files neither as diectories.");
        if (count > 1)
            for (const QString &p : ignored)
                details += QString("\n'%1'").arg(p);
        msg.setText(text);
        msg.setDetailedText(details);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.setIcon(QMessageBox::Warning);
        const int res = msg.exec();
    }
}

void MainWindow::addFiles()
{
    const QStringList newFilePathes = QFileDialog::getOpenFileNames(this, "Add Files", m_lastDialogDir);
    if (newFilePathes.isEmpty())
        return;
    addFiles(newFilePathes);
}

void MainWindow::addFiles(const QStringList &filePathes)
{
    for (const auto &path : filePathes){
        QFileInfo info(path);
        if (not m_files.contains(info))
            m_files << info;
    }
    updateFilesList();
}

void MainWindow::addFolders(const QStringList &filePathes)
{
    for (const auto &path : filePathes){
        QFileInfo info(path);
        if (not m_folders.contains(info))
            m_folders << info;
    }
    updateFoldersList();
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
