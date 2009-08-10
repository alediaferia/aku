 /* This file is part of the KDE project

   Copyright (C) 2008 Francesco Grieco <fgrieco@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "akuadddir.h"

#include <QSplitter>
#include <QHeaderView>
#include <QDir>
#include <QLabel>

#include <KVBox>
#include <KLocale>

#include "kfileplacesview.h"
#include "kfileplacesmodel.h"

akuAddDir::akuAddDir(QWidget *parent) : KDialog(parent)
{
   setModal(true);
   QSplitter *mainlayout = new QSplitter(this);
   setMainWidget(mainlayout);

   KVBox *v1layout = new KVBox(mainlayout);
   KVBox *v2layout = new KVBox(mainlayout);
 
   v1layout->setSpacing(10);
   v2layout->setSpacing(10);
    
   treeView = new KFileTreeView(v2layout);
   treeView->setColumnHidden (1, true);
   treeView->setColumnHidden (2, true);
   treeView->setColumnHidden (3, true);
   treeView->setColumnHidden (4, true);
   treeView->setColumnHidden (5, true);
   treeView->setColumnHidden (6, true);
   treeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
   treeView->resizeColumnToContents(0);
   treeView->setDirOnlyMode(true);
   treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
   treeView->setCurrentUrl(KUrl(QDir::homePath())); 

   QLabel *places = new QLabel(v1layout);
   places->setText("<b>" + i18n("Places") + "</b>");
   KFilePlacesView *list = new KFilePlacesView(v1layout);
   KFilePlacesModel *model = new KFilePlacesModel(this);
   list->setModel(model);
   list->setAutoResizeItemsEnabled(true);

   setInitialSize(QSize(450, 300));

   connect (list, SIGNAL(urlChanged(const KUrl&)), SLOT(urlSelected(const KUrl&)));
}

akuAddDir::~akuAddDir()
{}

void akuAddDir::urlSelected(const KUrl& url)
{
  QString string = url.pathOrUrl();
  updateTreeViewSelection(string);
}

void akuAddDir::updateTreeViewSelection(QString path)
{
  KUrl url(path);
  treeView -> setCurrentUrl(KUrl(path)); 
}
