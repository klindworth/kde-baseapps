/*
   Copyright (C) 2000-2011 Dawit Alemayehu <adawit@kde.org>

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

#ifndef DIR_FILTER_PLUGIN_H
#define DIR_FILTER_PLUGIN_H

#include <QSet>
#include <QPointer>
#include <QStringList>
#include <QWidget>

#include <kurl.h>
#include <kparts/plugin.h>
#include <kparts/listingextension.h>

class QPushButton;
class KUrl;
class KDirLister;
class KFileItemList;
class KLineEdit;
namespace KParts  {
    class ReadOnlyPart;
}

class FilterBar : public QWidget
{
    Q_OBJECT

public:
    explicit FilterBar(QWidget* parent = 0);
    virtual ~FilterBar();
    void selectAll();

    QMenu* typeFilterMenu();
    void setTypeFilterMenu(QMenu*);

    bool typeFilterMenuEnabled() const;
    void setEnableTypeFilterMenu(bool);

    void setNameFilter(const QString&);

public Q_SLOTS:
    void clear();

Q_SIGNALS:
    void filterChanged(const QString& nameFilter);
    void closeRequest();

protected:
    virtual void showEvent(QShowEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);

private:
    KLineEdit* m_filterInput;
    QPushButton* m_typeFilterButton;
};

class SessionManager
{
public:
    struct Filters {
        QStringList typeFilters;
        QString nameFilter;
    };

    SessionManager();
    ~SessionManager();
    Filters restore(const KUrl& url);
    void save(const KUrl& url, const Filters& filters);

    bool showCount;
    bool useMultipleFilters;

protected:
    void loadSettings();
    void saveSettings();

private:
    bool m_bSettingsLoaded;
    QMap<QString, Filters> m_filters;
};


class DirFilterPlugin : public KParts::Plugin
{
    Q_OBJECT

public:

    DirFilterPlugin(QObject* parent, const QVariantList&);
    ~DirFilterPlugin();

private Q_SLOTS:
    void slotReset();
    void slotOpenURL();
    void slotOpenURLCompleted();
    void slotShowPopup();
    void slotShowCount();
    void slotShowFilterBar();
    void slotMultipleFilters();
    void slotItemSelected(QAction*);
    void slotNameFilterChanged(const QString&);
    void slotCloseRequest();
    void slotListingEvent(KParts::ListingNotificationExtension::NotificationEventType, const KFileItemList&);

private:
    void setFilterBar();

    struct MimeInfo {
        MimeInfo() : action(0), useAsFilter(false) {}

        QAction* action;
        bool useAsFilter;

        QString iconName;
        QString mimeComment;

        QSet<QString> filenames;
    };
    typedef QMap<QString, MimeInfo> MimeInfoMap;

    FilterBar* m_filterBar;
    QWidget* m_focusWidget;
    QPointer<KParts::ReadOnlyPart> m_part;
    QPointer<KParts::ListingFilterExtension> m_listingExt;
    MimeInfoMap m_pMimeInfo;
};

#endif

