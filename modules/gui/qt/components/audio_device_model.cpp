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

#include "audio_device_model.hpp"

AudioDeviceModel::AudioDeviceModel(intf_thread_t *p_intf, QObject *parent)
    : QAbstractListModel(parent)
    // , m_player(player)
{
    // assert(p_intf);

    i_inputs = aout_DevicesList( p_intf->p_sys->p_mainPlayerController->getAout().get(), &ids, &names);

    // connect ( ?????, SIGNAL(audio_device_changed()), this, SLOT(updateCurrent()) );
}

AudioDeviceModel::~AudioDeviceModel()
{
    free( ids );
    free( names );
    // free( m_current );
}

Qt::ItemFlags AudioDeviceModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsUserCheckable;
}

int AudioDeviceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return i_inputs;
}

QVariant AudioDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();

    const char *name = names[row];

    if (role == Qt::DisplayRole)
        return qfu(name);
    else if (role == Qt::CheckStateRole)
        return QVariant::fromValue<bool>(false);
            // (m_current && !strcmp( ids[row], m_current ) ) ||
            // ( m_current == NULL && ids[row] && ids[row][0] == '\0' ) );

    return QVariant();
}

// bool AudioDeviceModel::setData(const QModelIndex &index, const QVariant &value, int role)
// {
//     int row = index.row();
//     if ( role != Qt::CheckStateRole )
//         return false;
//     if (!value.canConvert<bool>())
//         return false;
//     bool select = value.toBool();
//     if (select)
//     {
//         aout_DeviceSet( p_intf->p_sys->p_mainPlayerController->getAout().get(), ids[row]);
//     }
//     return true;
// }

// void AudioDeviceModel::updateCurrent()
// {
//     char *current = aout_DeviceGet( aout.get() );
//     if ( !strcmp( current, m_current ) )
//         return;
//     char *oldCurrent = m_current;
//     m_current = current;

//     QModelIndex oldIndex;
//     QModelIndex currentIndex;

//     for(int i=0; i<i_inputs; i++ )
//     {
//         if(!strcmp( ids[i], m_current )){
//             currentIndex = index(i);
//         }
//         if(!strcmp( ids[i], oldCurrent )){
//             oldIndex = index(i);
//         }
//     }

//     emit dataChanged(oldIndex, oldIndex, { Qt::CheckStateRole });
//     emit dataChanged(currentIndex, currentIndex, { Qt::CheckStateRole });
// }

// void AudioDeviceModel::updateCurrent(){
//     printf("******************************");
// }

QHash<int, QByteArray> AudioDeviceModel::roleNames() const
{
    return QHash<int, QByteArray>{
        {Qt::DisplayRole, "display"},
        {Qt::CheckStateRole, "checked"}
    };
}
