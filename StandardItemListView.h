#ifndef STANDARDITEMLISTVIEW_H
#define STANDARDITEMLISTVIEW_H

#include "stable.h"

class StandardItemListView : public QListView
{
    Q_OBJECT

public:
    void setModel(QStandardItemModel *model);

private:
    void currentChanged(const QModelIndex & current, const QModelIndex & previous);
    QStandardItemModel * mModel;
};

#endif // STANDARDITEMLISTVIEW_H
