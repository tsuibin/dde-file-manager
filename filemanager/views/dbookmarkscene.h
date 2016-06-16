#ifndef DBOOKMARKSCENE_H
#define DBOOKMARKSCENE_H

#include <QGraphicsScene>
#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>

#define BOOKMARK_ITEM_HEIGHT 30
#define SEPARATOR_ITEM_HEIGHT 6
#define BOOKMARK_ITEM_WIDTH 200
#define BOOKMARK_ITEM_SPACE 5

class DBookmarkItem;
class DBookmarkRootItem;
class DBookmarkItemGroup;
class FMEvent;
class UDiskDeviceInfo;
class DUrl;
class DiskInfo;

class DBookmarkScene : public QGraphicsScene
{
    Q_OBJECT
public:
    DBookmarkScene();
    void addItem(DBookmarkItem *item);
    void insert(int index, DBookmarkItem *item);
    void insert(DBookmarkItem * before, DBookmarkItem *item);
    void remove(int index);
    void remove(DBookmarkItem * item);
    void setSceneRect(qreal x, qreal y, qreal w, qreal h);
    void addSeparator();
    void insertSeparator(int index);
    DBookmarkItemGroup * getGroup();
    int count();
    int getCustomBookmarkItemInsertIndex();
    int windowId();
    DBookmarkItem * hasBookmarkItem(const DUrl &url);
    DBookmarkItem * itemAt(const QPointF &point);
    int indexOf(DBookmarkItem * item);
    void setTightMode(bool v);
protected:
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
signals:
    void urlChanged(const QString &url);
    void dragEntered();
    void dragLeft();
    void dropped();
public slots:
    void doDragFinished(const QPointF &point, const QPointF &scenePoint, DBookmarkItem *item);
    void currentUrlChanged(const FMEvent &event);

    void setDefaultDiskItem(DBookmarkItem* item);
    void setNetworkDiskItem(DBookmarkItem* item);
    void doBookmarkRemoved(const FMEvent &event);
    void bookmarkRename(const FMEvent &event);
    void doBookmarkRenamed(const QString &oldname, const QString &newname, const FMEvent &event);
    void doBookmarkAdded(const QString &name, const FMEvent &event);
    void doMoveBookmark(int from, int to, const FMEvent &event);

    void volumeAdded(UDiskDeviceInfo * device);
    void volumeRemoved(UDiskDeviceInfo * device);
    void mountAdded(UDiskDeviceInfo * device);
    void mountRemoved(UDiskDeviceInfo * device);
private:
    bool isBelowLastItem(const QPointF &point);
    void increaseSize();
    void decreaseSize();
    void moveBefore(DBookmarkItem * from, DBookmarkItem* to);
    void moveAfter(DBookmarkItem * from, DBookmarkItem* to);
    int m_defaultCount = 0;
    DBookmarkRootItem * m_rootItem;
    DBookmarkItem* m_defaultDiskItem;
    DBookmarkItem* m_networkDiskItem;
    DBookmarkItemGroup * m_itemGroup;
    QList<DBookmarkItem *> m_customItems;
    QMap<QString, DBookmarkItem *> m_diskItems;
    double m_totalHeight = 0;
    bool m_acceptDrop;
    bool m_isTightMode = false;

//    QGraphicsWidget * m_defaultWidget;
//    QGraphicsWidget * m_bookmarkWidget;
    QGraphicsLinearLayout * m_defaultLayout;
//    QGraphicsLinearLayout * m_bookmarkLayout;
};

#endif // DBOOKMARKSCENE_H
