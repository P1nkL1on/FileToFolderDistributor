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
#include <QDesktopServices>
#include <QAbstractButton>
#include <QDirIterator>

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

    QVBoxLayout *pfl = new QVBoxLayout;
    pfl->addWidget(m_distribToolBar = new QToolBar);
    QFrame *previewFrame = new QFrame;
    pfl->addWidget(previewFrame, 1);
    previewFrame->setContentsMargins(0, 0, 0, 0);
    m_previewLayout = new QVBoxLayout;
    previewFrame->setLayout(m_previewLayout);
    bl->addLayout(pfl, 1);

    QVBoxLayout *fll = new QVBoxLayout;
    QToolBar *distribToolBar = new QToolBar;
    distribToolBar->addActions({m_apply, m_revert});
    fll->addWidget(distribToolBar);
    fll->addWidget(m_foldersList = new QTableWidget, 1);
    bl->addLayout(fll);
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
    m_foldersList->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(m_filesList, &QListWidget::currentRowChanged, this, &MainWindow::fileSelected);
    connect(m_foldersList, &QTableWidget::cellDoubleClicked, this, &MainWindow::cellDoubleClicked);
}

void MainWindow::initActions()
{
    m_folderShortcuts = {
        QKeySequence(Qt::Key_1), QKeySequence(Qt::Key_2), QKeySequence(Qt::Key_3), QKeySequence(Qt::Key_4), QKeySequence(Qt::Key_5), QKeySequence(Qt::Key_6),
        QKeySequence(Qt::Key_7), QKeySequence(Qt::Key_8), QKeySequence(Qt::Key_9), QKeySequence(Qt::Key_0), QKeySequence(Qt::Key_Q), QKeySequence(Qt::Key_W),
        QKeySequence(Qt::Key_E), QKeySequence(Qt::Key_R), QKeySequence(Qt::Key_T), QKeySequence(Qt::Key_Y), QKeySequence(Qt::Key_U), QKeySequence(Qt::Key_I)
    };

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

    m_distribMenu = menuBar->addMenu("Move to");
    m_distribMenu->setEnabled(false);

    m_distribMenu->addAction("Skip", this, &MainWindow::skipFile, QKeySequence(Qt::Key_Space));

    QMenu *distrib = menuBar->addMenu("Distribution");
    m_apply = distrib->addAction("Apply", this, &MainWindow::applyDistribPlan, QKeySequence(Qt::CTRL + Qt::Key_Enter));
    m_revert = distrib->addAction("Revert", this, &MainWindow::revertDistribPlan, QKeySequence(Qt::CTRL + Qt::Key_Z));
}

void MainWindow::initPreviewers()
{
    m_defaultPreview = new PreviewText;

    auto image = new PreviewImage;
    for (const QString &ext : {"png", "bmp", "jpg", "jpeg", "svg", "gif"})
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

    m_distribMenu->setEnabled(not m_folders.isEmpty() and m_fileSelectedInd >= 0);
    m_distribMenu->clear();
    QAction* skipAction = m_distribMenu->addAction("Skip", this, &MainWindow::skipFile, QKeySequence(Qt::Key_Space));
    m_distribToolBar->clear();
    m_distribToolBar->addAction(skipAction);

    for (const QFileInfo &f : m_folders){
        const QString dirName = f.fileName();

        const QString shortcut = m_folderShortcuts.at(row).toString();
        m_foldersList->setItem(row, 0, new QTableWidgetItem(shortcut));
        m_foldersList->setItem(row, 1, new QTableWidgetItem(dirName));

        QAction* action = m_distribMenu->addAction(QString("Move to '%1'").arg(dirName), this, [=](){
            if (m_fileSelectedInd >= 0 and row >= 0)
                planDistribFile(m_fileSelectedInd, row);
        }, m_folderShortcuts.at(row));
        m_distribToolBar->addAction(action);
        ++row;
    }
    updateFoldersCount();
}

void MainWindow::updateFoldersCount()
{
    int row = 0;
    for (const QFileInfo &f : m_folders){
        const QString dirName = f.fileName();

        const int moved = m_distrubution.value(dirName).length();
        const QString movedStr = moved ? QString::number(moved) : "--";
        m_foldersList->setItem(row, 2, new QTableWidgetItem(movedStr));
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
    if (not folders.isEmpty()){
        QMessageBox msg(this);
        msg.setWindowTitle("Folder Identification");
        msg.setText("Do you want to add folder(s) as a destination or\n"
                    "do you want to add file from inside them?");
        QString foldersList;
        for (const QString &f : folders)
            foldersList += QString("%1\n").arg(f);
        msg.setDetailedText(foldersList);
        msg.setIcon(QMessageBox::Question);
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg.button(QMessageBox::Yes)->setText("Add destination");
        msg.button(QMessageBox::No)->setText("Add files from inside");
        const int res = msg.exec();
        if (res == QMessageBox::Yes){
            addFolders(folders);
        } else if (res == QMessageBox::No) {
            for (const QString &f : folders){
                QDirIterator it(f, {}, QDir::Files, QDirIterator::Subdirectories);
                while (it.hasNext())
                    files << it.next();
            }
        } else {
            QMessageBox::information(this, "Folder Identification", "No folders was added.");
        }
    }
    if (not files.isEmpty())
        addFiles(files);

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
        msg.exec();
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
    int selectInd = m_files.length();
    bool addedFiles = false;
    for (const auto &path : filePathes){
        QFileInfo info(path);
        if (not m_files.contains(info)){
            m_files << info;
            addedFiles = true;
        }
    }
    updateFilesList();
    selectInd -= addedFiles ? 0 : 1;
    m_filesList->setCurrentRow(selectInd);
    m_distribMenu->setEnabled(m_fileSelectedInd >= 0 and not m_folders.isEmpty());
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
    m_files.clear();
    m_fileSelectedInd = -1;
    m_distribMenu->setEnabled(false);
    updateFilesList();
}

void MainWindow::clearFolders()
{
    m_folders.clear();
    updateFoldersList();
}

void MainWindow::fileSelected(int ind)
{
    m_fileSelectedInd = ind;

    if (ind < 0){
        if (m_currentPreview)
            dettachPreview();
        return;
    }

    Q_ASSERT(ind < m_files.length());
    const QFileInfo fileInfo = m_files[ind];
    previewFile(fileInfo);
}

void MainWindow::cellDoubleClicked(int row, int col)
{
    if (col == 1){
        const QString path = m_folders[row].absoluteFilePath();
        QDesktopServices::openUrl(path);
    }
}

void MainWindow::previewFile(QFileInfo fileInfo)
{
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

    m_currentPreview->setFile(fileInfo.filePath());
    m_distribMenu->setEnabled(not m_folders.isEmpty());
}

void MainWindow::skipFile()
{
    const int nextInd = m_fileSelectedInd + 1;
    if (nextInd < m_files.length())
        m_filesList->setCurrentRow(nextInd);
}

void MainWindow::planDistribFile(int fileInd, int dirInd)
{
    Q_ASSERT(fileInd >= 0 and fileInd < m_files.length());
    Q_ASSERT(dirInd >= 0 and dirInd < m_folders.length());

    QFileInfo fileInfo = m_files.takeAt(fileInd);
    QFileInfo dirInfo = m_folders[dirInd];

    qDebug() << "distrib" << fileInfo.fileName() << "to" << dirInfo.fileName();

    m_distrubution[dirInfo.fileName()] << fileInfo;
    updateFilesList();
    updateFoldersCount();

    m_filesList->focusWidget();
    m_filesList->setCurrentRow(fileInd < m_files.length() ? fileInd : (fileInd - 1));
}

void MainWindow::applyDistribPlan()
{
    QString text = QString("Please confirm next file to folder distribution:\n"), details;
    bool distribRequired = false;
    for (const QFileInfo &f : m_folders){
        const QString dirName = f.fileName();
        const QVector<QFileInfo> &infos = m_distrubution[dirName];
        if (not infos.length())
            continue;
        distribRequired = true;
        text += QString("  * %1 file(s) to '%2'\n").arg(infos.length()).arg(dirName);
        details += QString("%1:\n").arg(dirName);
        int ind = 0;
        for (const QFileInfo &fi : infos)
            details += QString("%1. %2\n")
                    .arg(QString::number(++ind).rightJustified(4))
                    .arg(fi.fileName());
    }
    if (not distribRequired)
        return;

    QMessageBox msg(this);
    msg.setWindowTitle("Confirmation");
    msg.setText(text);
    msg.setDetailedText(details);
    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if (msg.exec() != QMessageBox::Yes)
        return;

    if (QMessageBox::warning(this, "Confirmation",
                             "Are you sure? File(s) will be moved.\n"
                             "This action is undoable.",
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes)
        return;

    for (const QFileInfo &d : m_folders){
        const QString dirName = d.fileName();
        const QVector<QFileInfo> &infos = m_distrubution[dirName];
        for (const QFileInfo &f : infos){
            const QString newPath = QString("%1/%2")
                    .arg(d.filePath()).arg(f.fileName());
            QDir(f.filePath()).rename(f.filePath(), newPath);
        }
    }

    m_distrubution.clear();
    updateFoldersCount();
}

void MainWindow::revertDistribPlan()
{
    QStringList distributedFiles;
    for (const QString &d : m_distrubution.keys())
        for (const QFileInfo &i : m_distrubution[d])
            distributedFiles << i.filePath();
    if (distributedFiles.isEmpty())
        return;

    if (QMessageBox::warning(this, "Confirmation",
                             "Are you sure? File(s) will be added back\n"
                             "and all destribution plan will be erased.\n"
                             "This action is undoable.",
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        return;

    addFiles(distributedFiles);
    m_distrubution.clear();
    updateFoldersCount();
}
