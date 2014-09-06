#ifndef HEAPDUMPFORM_H
#define HEAPDUMPFORM_H

#include <QWidget>
#include <QTableView>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QContextMenuEvent>
#include <QPoint>
#include <QAction>
#include <QMenu>

#include "objectstore.h"
#include "heapdatamodel.h"
#include "heapitem.h"
#include "sqlconnectionpool.h"

namespace Ui {
class HeapDumpForm;
}

class RbkitMainWindow;

class HeapDumpForm : public QWidget
{
    Q_OBJECT
    QAction *viewRefAct;
    RBKit::HeapItem *rootItem;
    RBKit::HeapDataModel *model;
    QSortFilterProxyModel *proxyModel;
    RBKit::HeapItem *selecteItem;
    RbkitMainWindow *parentWindow;
public:
    explicit HeapDumpForm(QWidget *parent = 0, int _snapShotVersion = 0);
    ~HeapDumpForm();
    void loaData();
    void loadSelectedReferences(RBKit::HeapItem* _selectedItem);
    void adjustColumnWidth();
    RbkitMainWindow *getParentWindow() const;
    void setParentWindow(RbkitMainWindow *value);

private:
    Ui::HeapDumpForm *ui;
    int snapShotVersion;
public slots:
    void onCustomContextMenu(const QPoint& point);
    void viewReferences();
};

#endif // HEAPDUMPFORM_H