 /* This file is part of the KDE project

   Copyright (C) 2008 Francesco Grieco <fgrieco@gmail.com>
   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "mainwindow.h"
#include "pluginloader.h"
#include "pluginview.h"
#include "akutreemodel.h"
#include "akutreeview.h"
#include "akuiconview.h"
#include "akuviewoptionwidget.h"
#include "akuiconwidget.h"
#include "akuextractiondialog.h"
#include "akutooltip.h"
#include "akutooltipmanager.h"
#include "akuadddir.h"

#include <akuplugin.h>

#include <KFileDialog>
#include <KStandardAction>
#include <KActionCollection>
#include <KApplication>
#include <KLocale>
#include <KFileDialog>
#include <KPageWidget>
#include <KPageWidgetItem>
#include <KDialog>
#include <KMimeType>
#include <KDebug>
#include <KIcon>
#include <KMessageBox>
#include <KConfigGroup>
#include <KStatusBar>
#include <KFileItem>
#include <KIconLoader>
#include <KVBox>

#include <QListView>
#include <QTreeView>
#include <QProgressBar>
#include <QLabel>

MainWindow::MainWindow (QWidget* parent): KXmlGuiWindow (parent),
                                          m_pluginView(new PluginView(this)),
                                          m_progressBar(new QProgressBar(this)),
                                          m_statusLabel(new QLabel(this)),
                                          m_statusIcon(new QLabel(this))
{
  m_model = new AkuTreeModel(QVector<QStringList>(), this);
  PluginLoader *loader = new PluginLoader(this);

  connect(loader, SIGNAL(pluginLoaded(AkuPlugin*, const KPluginInfo &)),
          this, SLOT(addPlugin(AkuPlugin*, const KPluginInfo &)));

  KVBox *baseWd = new KVBox(this);
  setCentralWidget(baseWd);

  loader->loadAllPlugins();

  splitter = new QSplitter(baseWd);
  setCentralWidget(baseWd);

  treeView = new AkuTreeView(splitter);
  iconWidget = new AkuIconWidget(splitter);
  treeView->setModel(m_model);
  iconWidget->view()->setModel(m_model);

  statusBar()->insertPermanentWidget(0, m_progressBar);
  m_progressBar->setMinimum(0);
  m_progressBar->setMaximum(100);
  m_progressBar->hide();

  statusBar()->insertPermanentWidget(0, m_statusLabel);
  m_statusLabel->hide();

  statusBar()->insertPermanentWidget(0, m_statusIcon);
  m_statusIcon->hide();

  setupOptionsWidget();
  setupActions();
  setupConnections();
  setupGUI(QSize(650,460)); 
  loadSettings();
}

MainWindow::~MainWindow()
{
   actionRecentFiles->saveEntries(KGlobal::config()->group("RecentFiles"));
}

void MainWindow::setupActions()
{
  KStandardAction::openNew(this, SLOT(createNewArchive()), actionCollection());

  KStandardAction::open(this, SLOT(openDialog()), actionCollection());

  KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

  KStandardAction::preferences(m_optionDialog, SLOT(exec()), actionCollection());

  // Open Recent Files
  actionRecentFiles = KStandardAction::openRecent( this, SLOT(load(const KUrl&)), actionCollection());
  actionRecentFiles->loadEntries(KConfigGroup(KGlobal::config()->group("RecentFiles")));

  actionViewChoose = new KActionMenu(this);
  actionViewChoose->setIcon(KIcon("view-choose"));
  actionViewChoose->setText(i18n("View"));
  actionCollection()->addAction("view", actionViewChoose);
  actionViewChoose->setDelayed(false);
  viewTree = new KAction(i18n("Tree View"), this);
  viewTree->setIcon(KIcon("view-list-tree"));
  viewIcon = new KAction(i18n("Icon View"), this);
  viewIcon->setIcon(KIcon("view-list-icons"));
  actionViewChoose->addAction(viewTree);
  actionViewChoose->addAction(viewIcon);

  actionExtract = new KActionMenu(this);
  actionExtract->setIcon(KIcon("archive-extract.png"));
  actionExtract->setText(i18n("Extract"));
  actionCollection()->addAction("extract", actionExtract);

  actionAdd = new KActionMenu(this);
  actionAdd->setIcon(KIcon("archive-insert"));
  actionAdd->setText(i18n("Add to archive"));
  actionCollection()->addAction("add", actionAdd);
  actionAdd->setDelayed(false);
  addFile = new KAction(i18n("Add file(s)"), this);
  addFile->setIcon(KIcon("archive-insert"));
  addDir = new KAction(i18n("Add dir"), this);
  addDir->setIcon(KIcon("archive-insert-directory"));
  actionAdd->addAction(addFile);
  actionAdd->addAction(addDir);

  actionDelete = new KAction(this);
  actionDelete->setIcon(KIcon("archive-remove.png"));
  actionDelete->setText(i18n("Remove"));
  actionCollection()->addAction("delete", actionDelete);

  actionEncrypt = new KAction(this);
  actionEncrypt->setText(i18n("Encrypt this archive"));
  actionEncrypt->setIcon(KIcon("dialog-password"));
  actionCollection()->addAction("encryptArchive", actionEncrypt);

  actionLock = new KAction(this);
  actionLock->setText(i18n("Lock Archive"));
  actionLock->setIcon(KIcon("document-encrypt"));
  actionCollection()->addAction("lock", actionLock);
  
  actionAddComment = new KAction(this);
  actionAddComment->setText(i18n("Add Archive Comment"));
  actionAddComment->setIcon(KIcon("view-pim-notes"));
  actionCollection()->addAction("addComment", actionAddComment);

  actionRename = new KAction(this);
  actionRename->setText(i18n("Rename"));
  //actionRename->setIcon(KIcon(""));
  actionCollection()->addAction("rename", actionRename);

  actionCollection()->addAction("addDir", addDir);
  actionCollection()->addAction("addFile", addFile);

  addDir->setEnabled(false);
  addFile->setEnabled(false);
  actionRename->setEnabled(false);
  actionAddComment->setEnabled(false);
  actionEncrypt->setEnabled(false);
  actionExtract->setEnabled(false);
  actionDelete->setEnabled(false);
  actionAdd->setEnabled(false);
  actionLock -> setEnabled(false);
}

void MainWindow::setupConnections()
{
   connect(actionExtract, SIGNAL(triggered()), this, SLOT(extractionSlot()));
   connect(viewTree, SIGNAL(triggered()), this, SLOT(changeView()));
   connect(viewIcon, SIGNAL(triggered()), this, SLOT(changeView()));
   connect(actionLock, SIGNAL(triggered()), this, SLOT(lockArchive()));
   connect(addFile, SIGNAL(triggered()), this, SLOT(addFileToArchive()));
   connect(addDir, SIGNAL(triggered()), this, SLOT(addDirToArchive()));
}

void MainWindow::loadSettings()
{  
   actionHome = new QAction(QDir::homePath(), this);
   actionDesktop = new QAction(KGlobalSettings::desktopPath(), this);
    
   actionHome->setIcon(KIcon("user-home"));
   actionDesktop->setIcon(KIcon("user-desktop"));
   
   actionHome->setData(QDir::homePath());
   actionDesktop->setData(KGlobalSettings::desktopPath());   
   KAction *actionLabel = new KAction(this);
   actionLabel->setText(i18n("Quick extracto to"));
   actionLabel->setIcon(KIcon("archive-extract"));
   actionLabel->setEnabled(false);
   actionExtract->addAction(actionLabel);
   actionExtract->addSeparator();
   actionExtract->addAction(actionHome);
   actionExtract->addAction(actionDesktop);
   connect(actionHome, SIGNAL(triggered()), this, SLOT(recentDirSlot()));
   connect(actionDesktop, SIGNAL(triggered()), this, SLOT(recentDirSlot()));

   QStringList actionPaths;
   actionPaths = KConfigGroup(KGlobal::config(), "Favourite Dirs").readEntry("destinationDirs", QStringList());
   foreach (const KUrl &path, actionPaths) {
       KAction *recentDir = new KAction(this);
       recentDir->setData(QVariant(path.pathOrUrl()));
       //kDebug() << recentDir->data().toString();
       recentDir->setText(path.pathOrUrl());
       recentDir->setIcon(KIcon("inode-directory"));
       actionExtract->addAction(recentDir);
       connect(recentDir, SIGNAL(triggered()), this, SLOT(recentDirSlot()));
   }
//    // connect (recentDir, SIGNAL(triggered()) 

}

void MainWindow::openDialog()
{
    KUrl url;
    KFileDialog openDialog(KUrl(), QString(), this);
    QStringList filters;

    foreach (AkuPlugin *plugin, m_plugins) {
        foreach (const QString &mime, plugin->mimeTypeNames()) {
            if (!filters.contains(mime)) {
                filters << mime;
            }
        }
    }

    openDialog.setMimeFilter(filters);
    openDialog.setOperationMode(KFileDialog::Opening);

    if (openDialog.exec()) {
        url = openDialog.selectedUrl();
    }

    if (!url.isEmpty()) {
        m_currentUrl = url;
        load(url);
    }
}

void MainWindow::load(const KUrl &url)
{
    if (!QFile(url.pathOrUrl()).open(QIODevice::ReadOnly)) {
        KMessageBox::error(this, i18n("Could not open %1.\nCheck your file permissions", url.prettyUrl()), i18n("Load error"));
        return;
    }

    KMimeType::Ptr mimetype = KMimeType::findByUrl(url);
    kDebug() << mimetype -> name();

    if (!m_plugins.contains(mimetype->name())) {
        KMessageBox::detailedSorry(this,
                           i18n("Sorry, no available plugin to open: <b>%1</b>.", url.pathOrUrl() ),
                           i18n("Install a plugin for <b>%1</b> mimetype in order to load the archive."
                           , mimetype->name()), 
                           i18n("Unable to load archive"));
        return;
    }

    if (!m_plugins[mimetype->name()]->isWorkingProperly()) {
        KMessageBox::sorry(this,
                       i18n("The correct plugin to open <b>%1</b> mimetype was found but appears to not be working properly. "
                            "Please check the installation", mimetype->name()), 
                       i18n("Unable to load archive"));
        return;
    }

    // this emit adds the url of the file loaded to recent files
    m_currentPlugin = mimetype->name();
    m_plugins[mimetype->name()]->load(url);
    addRecentFile(url);
}

void MainWindow::addRecentFile(KUrl recent)
{
// FIXME: does not work
//     if (actionRecentFiles->maxItems() > actionRecentFiles->urls().count() && !actionRecentFiles->urls().contains(recent)) {
//         actionRecentFiles->removeUrl(actionRecentFiles->urls().first());
//     }
    actionRecentFiles -> addUrl(recent);
}

void MainWindow::setupOptionsWidget()
{
    m_optionDialog = new KDialog(this);
    m_optionDialog->setButtons(KDialog::Ok | KDialog::Apply | KDialog::Cancel | KDialog::Default);

    KPageWidget *optionsWidget = new KPageWidget(m_optionDialog);

    KPageWidgetItem *plugins = new KPageWidgetItem( m_pluginView, i18n( "Plugins" ) );
    plugins->setHeader( i18n( "Aku Loaded Plugins" ) );
    plugins->setIcon( KIcon( "configure" ) );

    m_optionView = new AkuViewOptionWidget(m_optionDialog);
    m_optionView->setViews(treeView, iconWidget);

    connect(m_optionDialog, SIGNAL(applyClicked()), m_optionView, SLOT(applySettings()));
    connect(m_optionDialog, SIGNAL(okClicked()), m_optionView, SLOT(applySettings()));
    connect(m_optionDialog, SIGNAL(defaultClicked()), m_optionView, SLOT(restoreDefaults()));

    KPageWidgetItem *viewopt = new KPageWidgetItem(m_optionView, i18n( "Setup the View" ) );
    viewopt->setHeader( i18n( "Tree View Settings" ) );
    viewopt->setIcon( KIcon( "view-choose" ) );

    optionsWidget->addPage(plugins);
    optionsWidget->addPage(viewopt);
    m_optionDialog->setMainWidget(optionsWidget);
    m_optionDialog->setCaption(i18n("Configuration"));

    m_optionDialog->resize(600, 400);
}

void MainWindow::extractionSlot()
{
    m_extractionDialog = new AkuExtractionDialog(this);
    connect(m_extractionDialog, SIGNAL(extractionClicked(const KUrl &)), this, SLOT(doExtraction(const KUrl &)));
    m_extractionDialog->setAdvancedWidget(m_plugins[m_currentPlugin]->extractionWidget());
    m_extractionDialog->exec();
}

void MainWindow::recentDirSlot()
{
    KAction *sender = static_cast<KAction*>(this->sender());
    KUrl url(sender->data().toString());
    kDebug() << url;
    kDebug() << sender->data().toString();
    doExtraction(url);
}

void MainWindow::doExtraction(const KUrl &destination)
{
    // TODO: retrieve selected files to extract
    kDebug() << "extracting to" << destination;
    QStringList files = treeView->selectedPaths();

    if (iconWidget->isVisible()) {
       files = iconWidget->view()->selectedPaths();
    }

    m_plugins[m_currentPlugin]->extract(m_currentUrl, destination, files);
    
    KConfigGroup options(KGlobal::config()->group("Favourite Dirs"));
    QStringList urlList = options.readEntry("destinationDirs", QStringList());

    if (urlList.contains(destination.pathOrUrl()) || 
        destination == KUrl(KGlobalSettings::desktopPath()) ||
        destination == KUrl(QDir::homePath())) {
        kDebug() << destination.pathOrUrl();
        return;
    }

    urlList.prepend(destination.path());
    kDebug() << urlList;
    options.writeEntry("destinationDirs", urlList);
}

void MainWindow::changeView()
{
    if (dynamic_cast<QAction*>(sender()) == viewTree) {
        m_optionView->showView(0);
    }

    if (dynamic_cast<QAction*>(sender()) == viewIcon) {
        m_optionView->showView(1);
    }

    m_optionView->applySettings();
}

void MainWindow::addPlugin(AkuPlugin *plugin, const KPluginInfo &info)
{
    connect(plugin, SIGNAL(archiveLoaded(const QVector<QStringList> &)),
            this, SLOT(showArchive(const QVector<QStringList> &)));
    connect(plugin, SIGNAL(error(const QString &)), this, SLOT(handleError(const QString &)));
    connect(plugin, SIGNAL(progressUpdate(double, double)), this, SLOT(handleProgress(double, double)));
    connect(plugin, SIGNAL(operationCompleted()), this, SLOT(completeOperations()));
    connect(plugin, SIGNAL(notifyExtractionComplete()), this, SLOT(extractionCompleteSlot()));
    connect(plugin, SIGNAL(stateChanged()), this, SLOT(slotPluginStateChanged()));

    foreach (const QString &mimeName, plugin->mimeTypeNames()) {
        KMimeType::Ptr mime = KMimeType::mimeType(mimeName);

        if (!mime) {
            kError()<<"Could not retrieve mimetype. Maybe wrong plugin implementation.";
            continue;
        }

        m_plugins.insert(mime->name(), plugin);

        m_pluginView->addPluginInfo(
                      mime->name(),
                      mime->comment(),
                      plugin->canExtract(),
                      plugin->canCreate(),
                      plugin->canAdd(),
                      plugin->canDelete(),
                      plugin->canRename(),
                      plugin->canEncrypt(),
                      plugin->canAddComment(),
                      plugin->canLock(),
                      plugin->isWorkingProperly(),
                      info,
                      plugin->extractionWidget()
                     );
    }
}

void MainWindow::slotPluginStateChanged()
{
    kDebug() << "state changed";
    AkuPlugin *sender = static_cast<AkuPlugin *>(this->sender());
    switch (sender->currentOperation()) {
        case AkuPlugin::Extracting :
            m_statusLabel->setText("<b>"+i18n("Extraction in progress...")+"</b>");
            m_statusLabel->show();
            m_statusIcon->setPixmap(KIconLoader::global()->loadIcon("document-open", KIconLoader::Small));
            m_statusIcon->show();
            break;
        case AkuPlugin::Loading :
            m_statusLabel->setText("<b>"+i18n("Loading archive...")+"</b>");
            m_statusLabel->show();
            m_statusIcon->setPixmap(KIconLoader::global()->loadIcon("archive-extract", KIconLoader::Small));
            m_statusIcon->show();
            break;
        default : ;
    }
}

void MainWindow::showArchive(const QVector<QStringList> &archive)
{
    if (archive.isEmpty()) {
        return;
    }
   
    actionExtract->setEnabled(false);
    actionExtract->setToolTip(i18n("Not supported by the current plugin"));
    actionDelete->setEnabled(false);
    actionDelete->setToolTip(i18n("Not supported by the current plugin"));
    actionAdd->setEnabled(false);
    actionAdd->setToolTip(i18n("Not supported by the current plugin"));
    addFile->setEnabled(false);
    addFile->setToolTip(i18n("Not supported by the current plugin"));
    addDir->setEnabled(false);
    addDir->setToolTip(i18n("Not supported by the current plugin"));
    actionRename->setEnabled(false);
    actionRename->setToolTip(i18n("Not supported by the current plugin"));
    actionAddComment->setEnabled(false);
    actionAddComment->setToolTip(i18n("Not supported by the current plugin"));
    actionEncrypt->setEnabled(false);
    actionEncrypt->setToolTip(i18n("Not supported by the current plugin"));
    actionLock->setEnabled(false);
    actionLock->setToolTip(i18n("Not supported by the current plugin"));
   
    // here we set additional per-plugin headers
    AkuPlugin *sender = static_cast<AkuPlugin*>(this->sender());
    m_model->setAdditionalHeaders(sender->additionalHeaderStrings());
 
    if (sender->canExtract()) {
        actionExtract->setEnabled(true);
        actionExtract->setToolTip("");
    }

    if (sender->canDelete()) {
        actionDelete->setEnabled(true);
        actionDelete->setToolTip("");
    }  

    if (sender->canRename()) {
        actionRename->setEnabled(true);
        actionRename->setToolTip("");
    }  
  
    if (sender->canAdd()) {
        actionAdd->setEnabled(true);
        addDir->setEnabled(true);
        addFile->setEnabled(true);
        actionAdd->setToolTip("");
    }

    if (sender->canLock()) {
        actionLock->setEnabled(true);
        actionLock->setToolTip("");
    }    
  
    if (sender->canAddComment()) {
        actionAddComment->setEnabled(true);
        actionAddComment->setToolTip("");
    }  
  
    if (sender->canEncrypt()) {
        actionEncrypt->setEnabled(true);
        actionEncrypt->setToolTip("");
    }  

    currentArchive = archive;

    m_model->setSourceData(currentArchive);
}

void MainWindow::handleError(const QString &error)
{
    AkuPlugin *sender = static_cast<AkuPlugin *>(this->sender());
    KMessageBox::error(this, error, i18n("Plugin error"));
}

void MainWindow::extractionCompleteSlot()
{
    AkuTooltip *tip = new AkuTooltip(centralWidget());
    tip->setTooltip(i18n("Extraction process completed"));
    AkuTooltipManager::instance()->showTip(tip);
}

void MainWindow::handleProgress(double current, double total)
{
    if (!m_progressBar->isVisible()) {
        m_progressBar->show();
    }
    kDebug() << current << total;
    double percent = ((double)100 * current) / total;
    m_progressBar->setValue((uint)percent);
    kDebug() << (uint)percent;
}

void MainWindow::completeOperations()
{
    m_progressBar->hide();
    m_statusLabel->hide();
    m_statusIcon->hide();
}

void MainWindow::lockArchive()
{   
    m_plugins[m_currentPlugin]->lock(m_currentUrl);
}

void MainWindow::addFileToArchive()
{
    KFileDialog *addFileDialog = new KFileDialog(KUrl(QDir::homePath()), "*.*", this);
    addFileDialog->show();
}

void MainWindow::addDirToArchive()
{
    akuAddDir *dirDialog = new akuAddDir(this);
    // we do not need to have the dialog in memory also after closing it.
    dirDialog->setWindowFlags(Qt::WA_DeleteOnClose);
    dirDialog->show(); 
}
