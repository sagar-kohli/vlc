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
#include <QQmlListProperty>
#include <QObject>
#include <QStringList>
#include <vlc_extensions.h>
#include "qml_main_context.hpp"
#include <QUrl>
#include <QObject>
#include <QList>

class ExtensionSubModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ExtensionSubModel(extensions_manager_t *p_ext_mgr, extension_t *p_ex, QObject *parent = nullptr);
    ~ExtensionSubModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;
private:
    char **ppsz_titles = NULL;
    uint16_t *pi_ids = NULL;
    int ppsz_titles_size = 0;
};


class ExtensionModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name )
    Q_PROPERTY(QString title READ title )
    Q_PROPERTY(QString description READ description )
    Q_PROPERTY(QString shortdes READ shortdes )
    Q_PROPERTY(QString author READ author )
    Q_PROPERTY(QString version READ version )
    Q_PROPERTY(QUrl url READ url )

private:
    extension_t *m_ext;

public:
    ExtensionModel(extension_t *p_ext, QObject *parent=0);

    QString name() const;
    QString title() const;
    QString description() const;
    QString shortdes() const;
    QString author() const;
    QString version() const;
    QUrl url() const;

};

class ExtensionManager: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<ExtensionModel> extnsn READ getExtensions);
    Q_PROPERTY(QmlMainContext* mainCtx READ getMainCtx WRITE setMainCtx);

public:
    ExtensionManager(QObject *parent = nullptr);
    virtual ~ExtensionManager();

    QQmlListProperty<ExtensionModel> getExtensions();
    QmlMainContext* getMainCtx();
    void setMainCtx(QmlMainContext*);

protected slots:
    void updateList();
    Q_INVOKABLE void activate(int row);

private:
    ExtensionsManager *EM;
    QList<ExtensionModel*> extensions;
    QmlMainContext* m_mainCtx;
    
};

#endif // EXTENSION_LIST_MODEL_HPP
