 /* This file is part of the KDE project

   Copyright (C) 2008 Francesco Grieco <fgrieco@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef AKUADDDIR_H
#define AKUADDDIR_H

#include <KDialog>
#include <KFileTreeView>

class akuAddDir : public KDialog
{
 Q_OBJECT

  public:
    akuAddDir(QWidget *parent = 0);
    ~akuAddDir();

    KFileTreeView *treeView;

  public slots:
    virtual void updateTreeViewSelection(QString);

  private slots:
    void urlSelected(const KUrl&);
};

#endif
