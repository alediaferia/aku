#include "mainwindow.h"

#include <KStandardAction>
#include <KActionCollection>
#include <KLocale>
#include <KApplication>
#include <KAction>
#include <KUrl>
#include <KFileDialog>

MainWindow::MainWindow (QWidget* parent): KXmlGuiWindow (parent)
{
  splitter = new QSplitter(this);
  setCentralWidget(splitter);
  tree = new QTreeView(this);
  splitter -> addWidget(tree);
  openArchive = new OpenArchive();
  setupActions();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupActions()
{
  KAction* newAction = new KAction(this);
  newAction = KStandardAction::openNew(this, SLOT(createNewArchive()), actionCollection());
  actionCollection() -> addAction("file_new", newAction);
  KAction* openAction = new KAction(this);
  openAction = KStandardAction::open(this, SLOT(openDialog()), actionCollection());
  actionCollection() -> addAction("file_open", openAction);
  KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
  setupGUI(QSize(650,460)); 
}

void MainWindow::openDialog()
{
  KUrl url = KFileDialog::getOpenUrl(KUrl("kfiledialog:///AkuOpenDir"), i18n("*.rar *.7z *.zip *.bz2 *.gz *.tar|All supported types\n*.rar|Rar archives\n*.7z|7-zip archives\n*.zip|Zip archives\n*.bz2|Tar archives (bzip)\n*.gz|Tar archives (gzip)\n*.tar|Tar archives\n*.*|All files"), this);
  if (!url.isEmpty()) {
    openArchive -> load(url);
  }
}
