#ifndef AKUMETAWIDGET_H
#define AKUMETAWIDGET_H

#include "akuratiowidget.h"

#include <QWidget>
#include <QLabel>
#include <QFrame>
#include <QGridLayout>
#include <QSpacerItem>
#include <QPixmap>
#include <QFont>
#include <QGroupBox>
#include <KIcon>
#include <QList>
#include <KMimeType>
#include <KLocale>
#include <QScrollArea>
#include <QList>
#include <QTreeWidgetItem>
#include <QPainter>

class akuMetaWidget : public QWidget
{

Q_OBJECT

public:
  akuMetaWidget(QWidget *parent = 0);
  ~akuMetaWidget();

public slots:
  virtual void setMimeIcon(QPixmap);
  virtual void setFileName(QString, bool folder = false);
  virtual void setFileSize(QString);
  virtual void clear();
  virtual void setMime(QString);
  virtual void setPreview(QByteArray);
  virtual void handleItemSelections(QList<QTreeWidgetItem*>);
  virtual void setRatio(float);
private:
  QGridLayout *singleFileLayout;
  QGridLayout *archiveLayout;
  QGridLayout *metaLayout;
  QLabel *iconMap;
  QLabel *metaName;
  QLabel *metaSize;
  QLabel *metaMime;
  QLabel *archiveName;
  QLabel *archiveDetails;
  QLabel *notificationIcon;
  QScrollArea *scrollArea;
  akuRatioWidget *ratio;
};
#endif
