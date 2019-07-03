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

#define MENU_GET_EXTENSION(a) ( (uint16_t)( ((uint32_t)a) & 0xFFFF ) )

static QPixmap *loadPixmapFromData( char *, int size );



/* Safe copy of the extension_t struct */
ExtensionModel::ExtensionCopy::ExtensionCopy( extension_t *p_ext )
{
    name = qfu( p_ext->psz_name );
    description = qfu( p_ext->psz_description );
    shortdesc = qfu( p_ext->psz_shortdescription );
    if( description.isEmpty() )
        description = shortdesc;
    if( shortdesc.isEmpty() && !description.isEmpty() )
        shortdesc = description;
    title = qfu( p_ext->psz_title );
    author = qfu( p_ext->psz_author );
    version = qfu( p_ext->psz_version );
    url = qfu( p_ext->psz_url );
    icon = loadPixmapFromData( p_ext->p_icondata, p_ext->i_icondata_size );
}

ExtensionModel::ExtensionCopy::~ExtensionCopy()
{
    delete icon;
}

QVariant ExtensionModel::ExtensionCopy::data( int role ) const
{
    switch( role )
    {
    case Qt::DisplayRole:
        return title;
    case Qt::DecorationRole:
        if ( !icon ) return QPixmap( ":/logo/vlc48.png" );
        return *icon;
    case Qt::CheckStateRole:
        return QVariant::fromValue<bool>(true);
    case SummaryRole:
        return shortdesc;
    case VersionRole:
        return version;
    case AuthorRole:
        return author;
    case LinkRole:
        return url;
    case FilenameRole:
        return name;
    default:
        return QVariant();
    }
}

ExtensionModel::ExtensionModel( intf_thread_t *p_intf, QObject *parent )
        : QAbstractListModel( parent )
{

    EM = ExtensionsManager::getInstance(p_intf);

    // Connect to ExtensionsManager::extensionsUpdated()
    CONNECT( EM, extensionsUpdated(), this, updateList() );

    // Load extensions now if not already loaded
    EM->loadExtensions();
}

ExtensionModel::~ExtensionModel()
{
    // Clear extensions list
    while( !extensions.isEmpty() )
        delete extensions.takeLast();
}

void ExtensionModel::updateList()
{
    ExtensionCopy *ext;

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
        ext = new ExtensionCopy( p_ext );
        extensions.append( ext );
    }
    vlc_mutex_unlock( &p_mgr->lock );

    emit dataChanged( index( 0 ), index( rowCount() - 1 ) );
}

int ExtensionModel::rowCount( const QModelIndex& ) const
{
    int count = 0;
    extensions_manager_t *p_mgr = EM->getManager();
    if( !p_mgr )
        return 0;

    vlc_mutex_lock( &p_mgr->lock );
    count = p_mgr->extensions.i_size;
    vlc_mutex_unlock( &p_mgr->lock );

    return count;
}

QHash<int, QByteArray> ExtensionModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(Qt::DisplayRole, "display");
    roleNames.insert(Qt::CheckStateRole, "checked");
    return roleNames;
}

void ExtensionModel::fun(int row)
{

    uint16_t i_ext = MENU_GET_EXTENSION( row );

    // printf("*******************%d\n", i_ext);

    extensions_manager_t *p_mgr = EM->getManager();

    vlc_mutex_lock( &p_mgr->lock );

    extension_t *p_ext = ARRAY_VAL( p_mgr->extensions, i_ext );
    assert( p_ext != NULL);

    vlc_mutex_unlock( &p_mgr->lock );

    if( extension_TriggerOnly( p_mgr, p_ext ) )
    {
        extension_Trigger( p_mgr, p_ext );
    }
    else
    {
        if( !extension_IsActivated( p_mgr, p_ext ) )
            extension_Activate( p_mgr, p_ext );
        else
            extension_Deactivate( p_mgr, p_ext );
    }
            
}

QVariant ExtensionModel::data( const QModelIndex& index, int role ) const
{
    if( !index.isValid() )
        return QVariant();

    ExtensionCopy * extension =
            static_cast<ExtensionCopy *>(index.internalPointer());
    if ( !extension )
        return QVariant();
    else
        return extension->data( role );
}

QModelIndex ExtensionModel::index( int row, int column,
                                       const QModelIndex& ) const
{
    if( column != 0 )
        return QModelIndex();
    if( row < 0 || row >= extensions.count() )
        return QModelIndex();

    return createIndex( row, 0, extensions.at( row ) );
}


static QPixmap *loadPixmapFromData( char *data, int size )
{
    if( !data || size <= 0 )
        return NULL;
    QPixmap *pixmap = new QPixmap();
    if( !pixmap->loadFromData( (const uchar*) data, (uint) size ) )
    {
        delete pixmap;
        return NULL;
    }
    return pixmap;
}