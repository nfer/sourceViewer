#include "StandardItemListView.h"

void StandardItemListView::setModel(QStandardItemModel * model)
{
    QListView::setModel(model);
    mModel = model;
}

void StandardItemListView::currentChanged(const QModelIndex & current,
                            const QModelIndex & previous)
{
    mModel->itemFromIndex(current)->setBackground(QBrush(QColor(51,153,255)));

    if(previous.row() == -1)
    {
        // do nothing
    }
    else if(previous.row() % 2 == 1)
    {
        mModel->itemFromIndex(previous)->setBackground(QBrush(QColor(242,242,242)));
    }
    else{
        mModel->itemFromIndex(previous)->setBackground(QBrush(Qt::white));
    }
}
