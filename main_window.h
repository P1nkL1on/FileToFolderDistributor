#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

#include <QBoxLayout>
#include <QListWidget>
#include <QTableWidget>
#include <QFileInfo>

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

    QHash<QString, PreviewFrame *> m_previews;
    PreviewFrame *m_defaultPreview = nullptr;
    PreviewFrame *m_currentPreview = nullptr;

    QAction *m_newFiles;
    QAction *m_addFiles;
    QAction *m_newFolders;
    QAction *m_sortFilesNameAction;
    QAction *m_sortFilesDateAction;

    QListWidget *m_filesList;
    QTableWidget *m_foldersList;
    QVBoxLayout *m_previewLayout;

    QString m_lastDialogDir;

    void addFiles(const QStringList &filePathes);
    void addFolders(const QStringList &filePathes);

    void updateFilesList();
    void updateFoldersList();

    void sortFilesByName();
    void sortFilesByDate();

    void dettachPreview();
    void attachPreview(PreviewFrame *p);

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent* event) override;

protected slots:
    void addFiles();
    void addFolders();
    void clearFiles();
    void clearFolders();
    void fileSelected(int ind);
};

#endif // MAINWINDOW_H
