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

#include "extension_model.hpp"
#include <QPixmap>

#include <QDebug>

ExtensionSubModel::ExtensionSubModel(extensions_manager_t *p_ext_mgr, extension_t *p_ex, QObject *parent)
    : QAbstractListModel(parent)
{   
    if( extension_GetMenu( p_ext_mgr, p_ex, &ppsz_titles, &pi_ids ) == VLC_SUCCESS )
    {
        for (int i=0; ppsz_titles[i]!=NULL; i++)
            ppsz_titles_size++;
    }
}

ExtensionSubModel::~ExtensionSubModel()
{
    if(ppsz_titles_size > 0)
    {
        for(int i=0; ppsz_titles[i] !=NULL; ++i){
            free(ppsz_titles[i]);
        }

        free(ppsz_titles);
        free(pi_ids);
    }
}

int ExtensionSubModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return ppsz_titles_size;
}

QVariant ExtensionSubModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int row = index.row();
    if (row < 0 || row >= ppsz_titles_size)
        return QVariant();

    const char *title = ppsz_titles[row];
    if (role == Qt::DisplayRole)
        return qfu(title);

    return QVariant();
}

QHash<int, QByteArray> ExtensionSubModel::roleNames() const
{
    return QHash<int, QByteArray>{
        {Qt::DisplayRole, "display"}
    };
}

ExtensionModel::ExtensionModel(extension_t *p_ext, QObject *parent)
    : QObject(parent), m_ext(p_ext)
{
}

QString ExtensionModel::name() const
{
    return qfu( m_ext->psz_name );
}

QString ExtensionModel::title() const
{
    return qfu( m_ext->psz_title );
}

QString ExtensionModel::description() const
{
    return qfu( m_ext->psz_description );
}

QString ExtensionModel::shortdes() const
{
    return qfu( m_ext->psz_shortdescription );
}

QString ExtensionModel::author() const
{
    return qfu( m_ext->psz_author );
}

QString ExtensionModel::version() const
{
    return qfu( m_ext->psz_version );
}

QUrl ExtensionModel::url() const
{
    return qfu( m_ext->psz_url );
}

ExtensionManager::ExtensionManager(QObject *)
{
}

ExtensionManager::~ExtensionManager()
{
    // Clear extensions list
    for(auto extension: extensions)
        delete extension;
}

void ExtensionManager::updateList()
{
    ExtensionModel *ext;

    // Clear extensions list
    for(auto extension: extensions)
        delete extension;

    extensions.clear();

    // Find new extensions
    extensions_manager_t *p_mgr = EM->getManager();
    if( !p_mgr )
        return;

    vlc_mutex_lock( &p_mgr->lock );
    extension_t *p_ext;
    ARRAY_FOREACH( p_ext, p_mgr->extensions )
    {
        ext = new ExtensionModel( p_ext, this );
        extensions.append( ext );
    }
    vlc_mutex_unlock( &p_mgr->lock );

    emit extensionsChanged();
}

const QList<ExtensionModel*> ExtensionManager::getExtensions()
{
    return extensions;
}

QmlMainContext* ExtensionManager::getMainCtx()
{
    if(m_mainCtx != nullptr)    
        return m_mainCtx;
    else 
        return nullptr;
}

void ExtensionManager::setMainCtx(QmlMainContext *ctx)
{
    m_mainCtx = ctx;
    EM = ExtensionsManager::getInstance(m_mainCtx->getIntf());
    CONNECT( EM, extensionsUpdated(), this, updateList() );
    EM->loadExtensions();
}
