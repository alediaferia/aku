 /* This file is part of the KDE project

   Copyright (C) 2008 Francesco Grieco <fgrieco@gmail.com>
   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <KRecentFilesAction>
#include <KUrl>
#include <KAction>
#include <KActionMenu>
#include <QSplitter>

class KDialog;
class AkuPlugin;
class PluginView;
class QTreeView;
class QListView;
class AkuTreeView;
class AkuIconWidget;
class AkuTreeModel;
class AkuViewOptionWidget;
class KPluginInfo;
class QProgressBar;
class AkuExtractionDialog;
class QLabel;

class MainWindow : public KXmlGuiWindow
{
  Q_OBJECT

  public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

  private:
    QSplitter *splitter;
    AkuTreeView *treeView;
    AkuIconWidget *iconWidget;
    AkuTreeModel *m_model;
    KDialog *m_optionDialog;
    PluginView *m_pluginView;
    QMap<QString, AkuPlugin*> m_plugins;
    QVector<QStringList> currentArchive;
    KRecentFilesAction *actionRecentFiles;
    KActionMenu *actionViewChoose;
    KActionMenu *actionAdd;
    KActionMenu *actionExtract;
    KAction *viewTree;
    KAction *viewIcon;
    KAction *addFile;
    KAction *addDir;
    QAction *actionHome;
    QAction *actionDesktop;
    AkuViewOptionWidget *m_optionView;
    QProgressBar *m_progressBar;
    AkuExtractionDialog *m_extractionDialog;
    QString m_currentPlugin;
    KUrl m_currentUrl;
    QLabel *m_statusLabel;
    QLabel *m_statusIcon;

    KAction *actionDelete;
    KAction *actionEncrypt;
    KAction *actionAddComment;
    KAction *actionLock;
    KAction *actionRename;

    void setupActions();
    void setupOptionsWidget();
    void setupConnections();

  public slots:
    void load(const KUrl &);

  private slots:
    void openDialog();
    void addPlugin(AkuPlugin*, const KPluginInfo &);
    void showArchive(const QVector<QStringList> &archive);
    void handleError(const QString &);
    void addRecentFile(KUrl);
    void changeView();
    void handleProgress(double current, double total);
    void completeOperations();
    void extractionSlot();
    void doExtraction(const KUrl &destination);
    void extractionCompleteSlot();
    void loadSettings();
    void slotPluginStateChanged();
    void recentDirSlot();
    void lockArchive();
    void addFileToArchive();
    void addDirToArchive();
};

#endif
