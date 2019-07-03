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


class ExtensionModel : public QAbstractListModel
{

    Q_OBJECT

public:
    /* Safe copy of the extension_t struct */
    class ExtensionCopy
    {

    public:
        ExtensionCopy( extension_t * );
        ~ExtensionCopy();
        QVariant data( int role ) const;

    private:
        QString name, title, description, shortdesc, author, version, url;
        QPixmap *icon;
    };

    ExtensionModel(intf_thread_t *p_intf, QObject *parent = nullptr );
    virtual ~ExtensionModel();

    enum
    {
        SummaryRole = Qt::UserRole,
        VersionRole,
        AuthorRole,
        LinkRole,
        FilenameRole
    };

    QVariant data( const QModelIndex& index, int role ) const override;
    QModelIndex index( int row, int column = 0,
                       const QModelIndex& = QModelIndex() ) const override;

    QHash<int, QByteArray> roleNames() const override;
    int rowCount( const QModelIndex& = QModelIndex() ) const override;

protected slots:
    void updateList();
    Q_INVOKABLE void fun(int row);

private:
    ExtensionsManager *EM;
    QList<ExtensionCopy*> extensions;
};

#endif // EXTENSION_LIST_MODEL_HPP


