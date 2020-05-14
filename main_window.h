#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

#include <QBoxLayout>
#include <QListWidget>
#include <QTableWidget>
#include <QFileInfo>
#include <QToolBar>

#include "preview_frame.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void initLayout();
    void initActions();
    void initPreviewers();

protected:
    QVector<QFileInfo> m_files;
    QVector<QFileInfo> m_folders;
    QList<QKeySequence> m_folderShortcuts;
    QList<QAction*> m_separateActions;
    QHash<QString, QVector<QFileInfo>> m_distrubution;
    QMenu *m_distribMenu;
    QToolBar *m_distribToolBar;
    int m_fileSelectedInd = -1;

    QHash<QString, PreviewFrame *> m_previews;
    PreviewFrame *m_defaultPreview = nullptr;
    PreviewFrame *m_currentPreview = nullptr;

    QAction *m_newFiles;
    QAction *m_addFiles;
    QAction *m_newFolders;
    QAction *m_sortFilesNameAction;
    QAction *m_sortFilesDateAction;
    QAction *m_apply;
    QAction *m_revert;

    QListWidget *m_filesList;
    QTableWidget *m_foldersList;
    QVBoxLayout *m_previewLayout;

    QString m_lastDialogDir;

    void addFiles(const QStringList &filePathes);
    void addFolders(const QStringList &filePathes);

    void updateFilesList();
    void updateFoldersList();
    void updateFoldersCount();

    void sortFilesByName();
    void sortFilesByDate();

    void dettachPreview();
    void attachPreview(PreviewFrame *p);

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;

    void previewFile(QFileInfo fileInfo);

protected slots:
    void addFiles();
    void addFolders();
    void clearFiles();
    void clearFolders();
    void fileSelected(int ind);
    void cellDoubleClicked(int row, int col);
    void skipFile();
    void planDistribFile(int fileInd, int dirInd);
    void applyDistribPlan();
    void revertDistribPlan();
};

#endif // MAINWINDOW_H
