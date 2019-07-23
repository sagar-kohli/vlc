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

ExtensionManager::ExtensionManager(QObject *parent)
{
    if(m_mainCtx)
    {

        EM = ExtensionsManager::getInstance(m_mainCtx->getIntf());

        CONNECT( EM, extensionsUpdated(), this, updateList() );

        EM->loadExtensions();
    }
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
}

QList<ExtensionModel*> ExtensionManager::getExtensions() const
{
    return extensions;
}

QmlMainContext* ExtensionManager::getMainCtx()
{
    if(m_mainCtx)    
        return m_mainCtx;
    else 
        return nullptr;
}

void ExtensionManager::setMainCtx(QmlMainContext *ctx)
{
    if(ctx)
        m_mainCtx = ctx;
}
