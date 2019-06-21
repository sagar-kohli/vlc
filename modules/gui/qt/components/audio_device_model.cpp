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
#include "components/player_controller.hpp"

AudioDeviceModel::AudioDeviceModel(intf_thread_t *p_intf, QObject *parent)
    : QAbstractListModel(parent)
    , p_intf(p_intf)
{
}

AudioDeviceModel::~AudioDeviceModel()
{
    free( m_ids );
    free( m_names );
}

Qt::ItemFlags AudioDeviceModel::flags(const QModelIndex &) const
{
    return Qt::ItemIsUserCheckable;
}

int AudioDeviceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    PlayerController::AoutPtr aout = p_intf->p_sys->p_mainPlayerController->getAout();
    m_inputs = aout_DevicesList( aout.get(), &m_ids, &m_names);

    return m_inputs;
}

QVariant AudioDeviceModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (!index.isValid())
        return QVariant();
    const char *name = m_names[row];

    if (role == Qt::DisplayRole)
        return qfu(name);
    else if (role == Qt::CheckStateRole)
        return QVariant::fromValue<bool>((m_current && !strcmp( m_ids[row], m_current ) ) ||
            ( m_current == NULL && m_ids[row] && m_ids[row][0] == '\0' ) );

    return QVariant();
}

bool AudioDeviceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    if ( role != Qt::CheckStateRole )
        return false;
    if (!value.canConvert<bool>())
        return false;
    bool select = value.toBool();
    if (select)
    {
        PlayerController::AoutPtr aout = p_intf->p_sys->p_mainPlayerController->getAout();
        aout_DeviceSet( aout.get(), m_ids[row]);
    }
    return true;
}

void AudioDeviceModel::updateCurrent(const char *current)
{
    if ( !strcmp( current, m_current ) )
        return;
    const char *oldCurrent = m_current;
    m_current = current;

    int oldIndex;
    int currentIndex;

    for(int i=0; i<m_inputs; i++ )
    {
        if(!strcmp( m_ids[i], m_current )){
            currentIndex = i;
        }
        if(!strcmp( m_ids[i], oldCurrent )){
            oldIndex = i;
        }
    }

    if(oldIndex >= 0)
        emit dataChanged(index(oldIndex), index(oldIndex), { Qt::CheckStateRole });
    if(currentIndex >= 0)
        emit dataChanged(index(currentIndex), index(currentIndex), { Qt::CheckStateRole });
}

QHash<int, QByteArray> AudioDeviceModel::roleNames() const
{
    return QHash<int, QByteArray>{
        {Qt::DisplayRole, "display"},
        {Qt::CheckStateRole, "checked"}
    };
}
