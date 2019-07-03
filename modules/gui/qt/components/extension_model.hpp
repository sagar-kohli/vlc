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
    Q_PROPERTY(QList extnsn READ getExtensions);
    Q_PROPERTY(intf_thread_t *p_intfid READ getIntf SET setIntf);

public:
    ExtensionManager(QObject *parent=0);
    virtual ~ExtensionManager();

    QList<ExtensionModel*> getExtensions();
    intf_thread_t* getIntf();
    void setIntf(intf_thread_t *intf);

    intf_thread_t *m_intf;

protected slots:
    void updateList();

private:
    ExtensionsManager *EM;
    QList<ExtensionModel*> extensions;
    
};

#endif // EXTENSION_LIST_MODEL_HPP
