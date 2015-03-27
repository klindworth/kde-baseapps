/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>
                 2000 Carsten Pfeiffer <pfeiffer@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KONQ_SIDEBARTREE_H
#define KONQ_SIDEBARTREE_H

#include <kparts/browserextension.h>
#include "konq_sidebartreetoplevelitem.h"
#include <QtCore/QMap>
#include <QtCore/QPoint>

//Added by qt3to4:
#include <QPixmap>
#include <QtCore/QByteRef>
#include <QKeyEvent>
#include <QtCore/QEvent>
#include <QTreeWidget>

#include <kdebug.h>

class KonqSidebarOldTreeModule;
class KonqSidebarTreeModule;
class KonqSidebarTreeItem;
class KActionCollection;

class QTimer;

class KonqSidebarTree_Internal;

enum ModuleType { VIRT_Link = 0,  // a single .desktop file
                  VIRT_Folder = 1 }; // a directory which is parsed for .desktop files

typedef KonqSidebarTreeModule*(*getModule)(KonqSidebarTree*, const bool);

struct DirTreeConfigData { // TODO make base class with two subclasses?
    KUrl dir; // only used for VIRT_Folder
    ModuleType type;
    QString relDir; // only used for VIRT_Folder
};

enum DropAcceptType {
    SidebarTreeMode, // used if the drop is accepted by a KonqSidebarTreeItem. otherwise
    K3ListViewMode    // use K3ListView's dnd implementation. accepts mime types set with setDropFormats()
};

class CompatTree : public QTreeWidget
{
    Q_OBJECT
public:
    CompatTree(QWidget * parent) : QTreeWidget(parent) {
        connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(slotItemExpanded(QTreeWidgetItem*)));
    }

    //compat functions
    KonqSidebarTreeItem* selectedItem() const {
        if(!selectedItems().empty())
            return static_cast<KonqSidebarTreeItem*>(selectedItems().first());
        else
            return NULL;
    }

    KonqSidebarTreeItem* firstChild() const {
        return static_cast<KonqSidebarTreeItem*>(topLevelItem(0));
    }

    void ensureItemVisible(QTreeWidgetItem * item)
    {
        scrollToItem(item);
    }

    void rename(KonqSidebarTreeItem*,int)
    {
        //TODO KF5 port
        //starts editing for the item?
    }

    void moveItem(QTreeWidgetItem* item, QTreeWidgetItem* parent, QTreeWidgetItem* after)
    {
        //TODO KF5 port, untested
        if(item->parent())
        {
            QTreeWidgetItem* oldparent = item->parent();
            oldparent->takeChild(oldparent->indexOfChild(item));
        }
        else
            takeTopLevelItem(indexOfTopLevelItem(item));

        if(parent)
            parent->insertChild(parent->indexOfChild(after)+1, item);
        else
            insertTopLevelItem(indexOfTopLevelItem(after)+1, item);
    }

    QPoint contentsToViewport ( const QPoint & p )
    {
        //TODO KF5 port
        return p;
    }

public slots:
    //compat
    void setSelected(QTreeWidgetItem *item, bool selected) {
        if(selected)
            setCurrentItem(item);
        else
            clearSelection();
    }

    void itemExpanded(QTreeWidgetItem *item) {
        kDebug(1201) << "itemExpanded";
        KonqSidebarTreeItem *kitem = dynamic_cast<KonqSidebarTreeItem*>(item);
        if(kitem)
            kitem->setOpen(true);
    }

protected:

    virtual void contentsDragEnterEvent( QDragEnterEvent *e ) {Q_UNUSED(e);}
    virtual void contentsDragMoveEvent( QDragMoveEvent *e ) {Q_UNUSED(e);}
    virtual void contentsDragLeaveEvent( QDragLeaveEvent *e ) {Q_UNUSED(e);}
    virtual void contentsDropEvent( QDropEvent *ev ) {Q_UNUSED(ev);}
    virtual bool acceptDrag(QDropEvent* e) const {Q_UNUSED(e); return false;} // used in K3ListView mode

    virtual void dragEnterEvent( QDragEnterEvent * event ) {
        contentsDragEnterEvent(event);
    }

    virtual void dragLeaveEvent( QDragLeaveEvent * event ) {
        contentsDragLeaveEvent(event);
    }

    virtual void dragMoveEvent ( QDragMoveEvent * event ) {
        contentsDragMoveEvent(event);
    }

    virtual void dropEvent ( QDropEvent * event ) {
        contentsDropEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) {
        QTreeWidget::mousePressEvent(event);
    }
};

/**
 * The multi-purpose tree view.
 * It parses its configuration (desktop files), each one corresponding to
 * a toplevel item, and creates the modules that will handle the contents
 * of those items.
 */
class KonqSidebarTree : public CompatTree // PORTING NOTE: DO NOT PORT TO QTreeWidget. See http://kde.aliax.net/mockups/konqueror/ first.
{
    Q_OBJECT
public:
    KonqSidebarTree( KonqSidebarOldTreeModule *parent, QWidget *parentWidget, ModuleType moduleType, const QString& path );
    virtual ~KonqSidebarTree();

    void followURL( const KUrl &url );

    /**
     * @return the current (i.e. selected) item
     */
    KonqSidebarTreeItem * currentItem() const;

    void startAnimation( KonqSidebarTreeItem * item, const char * iconBaseName = "kde", uint iconCount = 6, const QPixmap * originalPixmap = 0L );
    void stopAnimation( KonqSidebarTreeItem * item );

    KonqSidebarOldTreeModule * sidebarModule() { return m_sidebarModule; }

    KActionCollection *actionCollection() { return m_collection; }

    //void lockScrolling( bool lock ) { m_scrollingLocked = lock; }

    bool isOpeningFirstChild() const { return m_bOpeningFirstChild; }

    void enableActions(bool copy, bool cut, bool paste);

    void itemDestructed( KonqSidebarTreeItem *item );

    void setDropFormats( const QStringList &formats ); // used in K3ListView mode

    // Show context menu for toplevel items
    void showToplevelContextMenu();

    // Add an URL
    void addUrl(KonqSidebarTreeTopLevelItem* item, const KUrl&url);

public slots:
    // Connected to KDirNotify dbus signals
    void slotFilesAdded( const QString & dir );
    void slotFilesRemoved( const QStringList & urls );
    void slotFilesChanged( const QStringList & urls );

protected:
    virtual void contentsDragEnterEvent( QDragEnterEvent *e );
    virtual void contentsDragMoveEvent( QDragMoveEvent *e );
    virtual void contentsDragLeaveEvent( QDragLeaveEvent *e );
    virtual void contentsDropEvent( QDropEvent *ev );
    virtual bool acceptDrag(QDropEvent* e) const; // used in K3ListView mode

    virtual bool eventFilter(QObject* obj, QEvent* ev);
    virtual void leaveEvent( QEvent * );

    virtual QDrag* dragObject();


private slots:
    void slotDoubleClicked( QTreeWidgetItem *item );
    void slotExecuted( QTreeWidgetItem *item );
    void slotMouseButtonPressed(int _button, QTreeWidgetItem* _item, const QPoint&, int col);
	//void slotMouseButtonClicked(int _button, QTreeWidgetItem* _item, const QPoint&, int col);
	void slotMouseButtonClicked(QTreeWidgetItem* _item, int col);
    void slotSelectionChanged();

    void slotAnimation();

    void slotAutoOpenFolder();

    void rescanConfiguration();

    void slotItemRenamed(QTreeWidgetItem*, const QString &, int);

    void slotCreateFolder();
    void slotDelete();
    void slotTrash();
    void slotRename();
    void slotProperties();
    void slotOpenNewWindow();
    void slotOpenTab();
    void slotCopyLocation();

private:
    void clearTree();
    void scanDir( KonqSidebarTreeItem *parent, const QString &path, bool isRoot = false );
    void loadTopLevelGroup(KonqSidebarTreeItem *parent, const QString &path);
    void loadTopLevelItem(KonqSidebarTreeItem *parent, const QString &path);

    void loadModuleFactories();

    bool overrideShortcut(const QKeyEvent* e);

private:
    QList<KonqSidebarTreeTopLevelItem*> m_topLevelItems;
    KonqSidebarTreeTopLevelItem *m_currentTopLevelItem;

    QList<KonqSidebarTreeModule*> m_lstModules;

    KonqSidebarOldTreeModule *m_sidebarModule;

    struct AnimationInfo
    {
        AnimationInfo( const char * _iconBaseName, uint _iconCount, const QPixmap & _originalPixmap )
            : iconBaseName(_iconBaseName), iconCount(_iconCount), iconNumber(1), originalPixmap(_originalPixmap) {}
        AnimationInfo() : iconCount(0) {}
        QByteArray iconBaseName;
        uint iconCount;
        uint iconNumber;
        QPixmap originalPixmap;
    };
    typedef QMap<KonqSidebarTreeItem *, AnimationInfo> MapCurrentOpeningFolders;
    MapCurrentOpeningFolders m_mapCurrentOpeningFolders;

    QTimer *m_animationTimer;

    KonqSidebarTreeItem *m_currentBeforeDropItem; // The item that was current before the drag-enter event happened
    KonqSidebarTreeItem *m_dropItem; // The item we are moving the mouse over (during a drag)
    QStringList m_lstDropFormats;

    QTimer *m_autoOpenTimer;

    // The base URL for our configuration directory
    //KUrl m_dirtreeDir;
    DirTreeConfigData m_dirtreeDir;

    getModule getPluginFactory(const QString &name);

    QMap<QString, QString>   pluginInfo;
    QMap<QString, getModule> pluginFactories;

    bool m_bOpeningFirstChild;
    KActionCollection *m_collection;

    KonqSidebarTree_Internal *d;

Q_SIGNALS:
    void copy();
    void cut();
    void paste();

#ifndef Q_MOC_RUN
#undef signals
#define signals public
#endif
signals:
#ifndef Q_MOC_RUN
#undef signals
#define signals protected
#endif
    void openUrlRequest( const KUrl &url, const KParts::OpenUrlArguments& args = KParts::OpenUrlArguments(),
                          const KParts::BrowserArguments& browserArgs = KParts::BrowserArguments() );
    void createNewWindow( const KUrl &url, const KParts::OpenUrlArguments& args = KParts::OpenUrlArguments(),
                          const KParts::BrowserArguments& browserArgs = KParts::BrowserArguments() );
};

#endif // KONQ_SIDEBARTREE_H
