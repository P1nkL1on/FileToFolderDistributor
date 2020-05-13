#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

#include <QBoxLayout>
#include <QListWidget>
#include <QFileInfo>

#include "distributor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    QVector<QFileInfo> m_files;
    QStringList m_folderPathes;
    QList<QKeySequence> m_folderShortcuts;
    QList<QAction*> m_separateActions;
    QHash<QString, Distributor *> m_distribs;

    QAction *m_newFiles;
    QAction *m_addFiles;
    QAction *m_newFolders;

    QListWidget *m_filesList;
    QListWidget *m_foldersList;
    QVBoxLayout *m_previewLayout;

    QString m_lastDialogDir;

    void updateFoldersList();
    void updateFilesList();

    void sortFilesByName();
    void sortFilesByDate();

protected slots:
    void addFiles();
    void addFolders();
    void clearFiles();
    void clearFolders();
};

#endif // MAINWINDOW_H
