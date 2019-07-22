/*****************************************************************************
 * Copyright (C) 2019 VLC authors and VideoLAN
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * ( at your option ) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef EXTENSION_MODEL_HPP
#define EXTENSION_MODEL_HPP

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <QAbstractListModel>
#include "extensions_manager.hpp"
#include <QObject>
#include <QStringList>
#include <vlc_extensions.h>

#include <QObject>

class ExtensionModel : public QObject
{
    Q_OBJECT

public:
    ExtensionModel(extension_t *p_ext, QObject *parent=0);


private:
    extension_t *m_ext;

    Q_PROPERTY(QString name READ name )
    Q_PROPERTY(QString title READ title )
    Q_PROPERTY(QString description READ description )
    Q_PROPERTY(QString shortdes READ shortdes )
    Q_PROPERTY(QString author READ author )
    Q_PROPERTY(QString version READ version )
    Q_PROPERTY(QPixmap icon READ icon )
    Q_PROPERTY(QUrl url READ url )

public:
    // ExtensionModel(QObject *parent=0);
    // ExtensionModel(extension_t *p_ext);

    QString name() const;
    QString title() const;
    QString description() const;
    QString shortdes() const;
    QString author() const;
    QString version() const;
    QPixmap icon() const;
    QUrl url() const;

};


class ExtensionManager: public QObject
{
    Q_OBJECT

public:
    ExtensionManager(intf_thread_t *p_intf, QObject *parent=0);
    virtual ~ExtensionManager();

    QList<ExtensionModel*> extensions;

protected slots:
    void updateList();

private:
    ExtensionsManager *EM;
};

#endif // EXTENSION_LIST_MODEL_HPP















    // void setName(const QString &name);
    // void setTitle(const QString &name);
//     void setDescription(const QString &name);
//     void setShortdes(const QString &name);
//     void setAuthor(const QString &name);
//     void setVersion(const QString &name);
//     void setIcon(const QString &name);
//     void setUrl(const QString &name);





// signals:
    // void nameChanged();
    // void titleChanged();
    // void descriptionChanged();
//     void shortdesChanged();
//     void authorChanged();
//     void versionChanged();
//     void iconChanged();
//     void urlChanged();

// private:
//     QString m_name, m_title, m_description, m_shortdes, m_author, m_version;
//     QPixmap *m_icon;
//     QUrl m_url;
// };





// class ExtensionModel : public QAbstractListModel
// {

//     Q_OBJECT

// public:
//     /* Safe copy of the extension_t struct */
//     class ExtensionCopy
//     {

//     public:
//         ExtensionCopy( extension_t * );
//         ~ExtensionCopy();
//         QVariant data( int role ) const;

//     private:
//         QString name, title, description, shortdesc, author, version, url;
//         QPixmap *icon;
//     };

    // ExtensionModel(intf_thread_t *p_intf, QObject *parent = nullptr );
    // virtual ~ExtensionModel();

//     enum
//     {
//         SummaryRole = Qt::UserRole,
//         VersionRole,
//         AuthorRole,
//         LinkRole,
//         FilenameRole
//     };

//     QVariant data( const QModelIndex& index, int role ) const override;
//     QModelIndex index( int row, int column = 0,
//                        const QModelIndex& = QModelIndex() ) const override;

//     QHash<int, QByteArray> roleNames() const override;
//     int rowCount( const QModelIndex& = QModelIndex() ) const override;

// protected slots:
//     void updateList();
//     Q_INVOKABLE void fun(int row);

// private:
//     ExtensionsManager *EM;
//     QList<ExtensionCopy*> extensions;
// };

// #endif // EXTENSION_LIST_MODEL_HPP


