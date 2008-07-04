#include "quickextract.h"

quickExtract::quickExtract(QWidget *parent) : KDialog(parent)
{  
  KVBox *vlayout = new KVBox(this);
  setMainWidget(vlayout);
  KHBox *hlayout = new KHBox(vlayout);
  
  treeView = new KFileTreeView(hlayout); 
  treeView -> setColumnHidden (1, true);
  treeView -> setColumnHidden (2, true);
  treeView -> setColumnHidden (3, true);
  treeView -> setColumnHidden (4, true);
  treeView -> setColumnHidden (5, true);
  treeView -> setColumnHidden (6, true);
  treeView -> header() -> setResizeMode ( 0, QHeaderView::ResizeToContents );
  treeView -> resizeColumnToContents (0);
  treeView -> setDirOnlyMode(true);
  treeView -> setEditTriggers(QAbstractItemView::NoEditTriggers);
  treeView -> setCurrentUrl(KUrl(QDir::homePath()));
  
  showhiddenAction = new KAction(treeView);
  showhiddenAction -> setText(i18n("Show hidden files"));
  showhiddenAction -> setCheckable(true);
  treeView -> addAction (showhiddenAction);

  KUrlCompletion *comp = new KUrlCompletion(KUrlCompletion::DirCompletion);
  khistory = new KHistoryComboBox(vlayout);
  khistory -> setCompletionObject(comp);
  khistory -> setAutoDeleteCompletionObject(true);
  khistory -> setCompletionMode(KGlobalSettings::CompletionPopupAuto);

  connect (showhiddenAction, SIGNAL (toggled(bool)), this, SLOT (hiddenFiles(bool)));
  connect (treeView, SIGNAL (currentChanged (KUrl)), this, SLOT (updateCombo(KUrl)));
  connect (khistory, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateTreeViewSelection(QString)));
  connect (khistory, SIGNAL(returnPressed(QString)), this, SLOT(updateTreeViewSelection(QString)));
}

quickExtract::~quickExtract()
{
}

void quickExtract::hiddenFiles(bool status)
{
  KUrl current = treeView -> currentUrl();
  kDebug() << current;
  if (status) treeView -> setShowHiddenFiles(true);
  else treeView -> setShowHiddenFiles(false);
  treeView -> setCurrentUrl(current);
}

void quickExtract::updateCombo(KUrl url)
{
  khistory -> setCurrentItem(url.pathOrUrl(), true);
}

void quickExtract::updateTreeViewSelection(QString path)
{
  KUrl url(path);
  treeView -> setCurrentUrl(KUrl(path)); 
}
