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


static QPixmap *loadPixmapFromData( char *, int size );

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

QPixmap ExtensionModel::icon() const
{
    return QPixmap();//loadPixmapFromData( m_ext->p_icondata, m_ext->i_icondata_size );
}

ExtensionManager::ExtensionManager(QObject *parent=0)
{
    EM = ExtensionsManager::getInstance(m_intf);

    CONNECT( EM, extensionsUpdated(), this, updateList() );

    EM->loadExtensions();
}

ExtensionManager::~ExtensionManager()
{
    // Clear extensions list
    while( !extensions.isEmpty() )
        delete extensions.takeLast();
}

void ExtensionManager::updateList()
{

    ExtensionModel *ext;

    // Clear extensions list
    while( !extensions.isEmpty() )
    {
        ext = extensions.takeLast();
        delete ext;
    }

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

QList<ExtensionModel*> ExtensionManager::getExtension() const
{
    return extensions;
}

void ExtensionManager::setIntf(intf_thread_t *intf)
{
    m_intf = intf;
}

intf_thread_t*ExtensionManager::getIntf()
{
    return m_intf;
}
