/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>
   Copyright (C) 2003 Waldo Bastian <bastian@kde.org>

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


#include "konq_sidebartree.h"
//#include "konq_sidebartreepart.h"

CompatTreeItem::CompatTreeItem(KonqSidebarTree *ptree) : QTreeWidgetItem(ptree) {}
bool CompatTreeItem::isOpen() const
{
    return isExpanded(); //unsure
}

void CompatTreeItem::setOpen(bool expand)
{
    setExpanded(expand);
}

QPixmap* CompatTreeItem::pixmap(int col) const
{
    return new QPixmap(this->icon(col).pixmap());
}

void CompatTreeItem::setPixmap(int col, QPixmap pix)
{
    setIcon(col, QIcon(pix));
}

bool CompatTreeItem::isExpandable() const
{
    return this->childCount() > 0;
}

void CompatTreeItem::setExpandable(bool) {
}

void CompatTreeItem::repaint() {}

void CompatTreeItem::setVisible(bool) {
}

bool CompatTreeItem::isSelectable() const {
    return true;
}

KonqSidebarTreeItem::KonqSidebarTreeItem( KonqSidebarTreeItem *parentItem, KonqSidebarTreeTopLevelItem *topLevelItem )
    : CompatTreeItem( parentItem )
{
    initItem( topLevelItem );
}

KonqSidebarTreeItem::KonqSidebarTreeItem( KonqSidebarTree *parent, KonqSidebarTreeTopLevelItem *topLevelItem )
    : CompatTreeItem( parent )
{
    initItem( topLevelItem );
}

KonqSidebarTreeItem::~KonqSidebarTreeItem()
{
    KonqSidebarTree *t = tree();
    if (t)
       t->itemDestructed(this);
}

void KonqSidebarTreeItem::initItem( KonqSidebarTreeTopLevelItem *topLevelItem )
{
    m_topLevelItem = topLevelItem;
    m_bListable = true;
    m_bClickable = true;

    setExpandable( true );
}

void KonqSidebarTreeItem::middleButtonClicked()
{
    emit tree()->createNewWindow( externalURL() );
}

KonqSidebarTreeModule * KonqSidebarTreeItem::module() const
{
    return m_topLevelItem->module();
}

KonqSidebarTree * KonqSidebarTreeItem::tree() const
{
    //return static_cast<KonqSidebarTree *>(listView());
    return static_cast<KonqSidebarTree *>(this->treeWidget());
}
