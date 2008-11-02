 /*

   Copyright (C) 2008 Alessandro Diaferia <alediaferia@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#ifndef AKUICONWIDGET_H
#define AKUICONWIDGET_H

#include <KVBox>

class AkuIconView;
class QLabel;

class AkuIconWidget : public KVBox
{
    Q_OBJECT
    public:
        AkuIconWidget(QWidget *parent = 0);
        ~AkuIconWidget();

    private:
        AkuIconView *m_view;
        QLabel *m_currentPathLabel;
};

#endif