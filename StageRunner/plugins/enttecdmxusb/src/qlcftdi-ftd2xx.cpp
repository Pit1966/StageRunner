/*
  Q Light Controller
  qlcftdi-ftd2xx.cpp

  Copyright (C) Heikki Junnila

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  Version 2 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details. The license is
  in the file "COPYING".

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <QSettings>
#include <QVariant>
#include <QDebug>
#include <QMap>

#include "enttecdmxusbwidget.h"
#include "enttecdmxusbprotx.h"
#include "enttecdmxusbprorx.h"
#include "enttecdmxusbopen.h"
#include "qlcftdi.h"

/**
 * Get some interesting strings from the device.
 *
 * @param deviceIndex The device index, whose strings to get
 * @param vendor Returned vendor string
 * @param description Returned description string
 * @param serial Returned serial string
 * @return FT_OK if strings were extracted successfully
 */
static FT_STATUS qlcftdi_get_strings(DWORD deviceIndex,
                                     QString& vendor,
                                     QString& description,
                                     QString& serial)
{
    char cVendor[256];
    char cVendorId[256];
    char cDescription[256];
    char cSerial[256];

    FT_HANDLE handle;

    FT_STATUS status = FT_Open(deviceIndex, &handle);
    if (status != FT_OK)
        return status;

    FT_PROGRAM_DATA pData;
    pData.Signature1 = 0;
    pData.Signature2 = 0xFFFFFFFF;
    pData.Version = 0x00000005;
    pData.Manufacturer = cVendor;
    pData.ManufacturerId = cVendorId;
    pData.Description = cDescription;
    pData.SerialNumber = cSerial;
    status = FT_EE_Read(handle, &pData);
    if (status == FT_OK)
    {
        vendor = QString(cVendor);
        description = QString(cDescription);
        serial = QString(cSerial);
    }

    FT_Close(handle);

    return status;
}

QLCFTDI::QLCFTDI(const QString& serial, const QString& name, quint32 id)
    : m_serial(serial)
    , m_name(name)
    , m_id(id)
    , m_handle(NULL)
{
}

QLCFTDI::~QLCFTDI()
{
    if (isOpen() == true)
        close();
}

QList <EnttecDMXUSBWidget*> QLCFTDI::widgets()
{
    QList <EnttecDMXUSBWidget*> list;

    /* Find out the number of FTDI devices present */
    DWORD num = 0;
    FT_STATUS status = FT_CreateDeviceInfoList(&num);
    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << "CreateDeviceInfoList:" << status;
        return list;
    }
    else if (num <= 0)
    {
        return list;
    }

    // Allocate storage for list based on numDevices
    FT_DEVICE_LIST_INFO_NODE* devInfo = new FT_DEVICE_LIST_INFO_NODE[num];

	// Get a map of user-forced serials and their types
    QMap <QString,QVariant> types(typeMap());

    // Get the device information list
    if (FT_GetDeviceInfoList(devInfo, &num) == FT_OK)
    {
        for (DWORD i = 0; i < num; i++)
        {
            QString vendor, description, serial;
			FT_STATUS s = qlcftdi_get_strings(i, vendor, description, serial);
            if (s != FT_OK || description.isEmpty() || serial.isEmpty())
            {
				// Seems that some otherwise working devices don't provide
				// FT_PROGRAM_DATA struct used by qlcftdi_get_strings().
				description = QString(devInfo[i].Description);
				serial = QString(devInfo[i].SerialNumber);
				vendor = QString();
			}

            if (types.contains(serial) == true)
            {
                // Force a widget with a specific serial to either type
                EnttecDMXUSBWidget::Type type = (EnttecDMXUSBWidget::Type)
                                                    types[serial].toInt();
                switch (type)
                {
                case EnttecDMXUSBWidget::OpenTX:
                    list << new EnttecDMXUSBOpen(serial, description, i);
                    break;
                case EnttecDMXUSBWidget::ProRX:
                    list << new EnttecDMXUSBProRX(serial, description, i);
                    break;
                default:
                case EnttecDMXUSBWidget::ProTX:
                    list << new EnttecDMXUSBProTX(serial, description, i);
                    break;
                }
            }
            else if (vendor.toUpper().contains("FTDI") == true || vendor.isEmpty())
            {
                /* This is probably an Open DMX USB widget */
                list << new EnttecDMXUSBOpen(serial, description, i);
            }
            else
            {
                /* This is probably a DMX USB Pro widget in TX mode */
                list << new EnttecDMXUSBProTX(serial, description, i);
            }
        }
    }

    delete [] devInfo;
    return list;
}

bool QLCFTDI::open()
{
    FT_STATUS status = FT_Open(m_id, &m_handle);
    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << name() << status;
        return false;
    }
    else
    {
        return true;
    }
}

bool QLCFTDI::close()
{
    FT_STATUS status = FT_Close(m_handle);
    m_handle = NULL;
    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << name() << status;
        return false;
    }
    else
    {
        return true;
    }
}

bool QLCFTDI::isOpen() const
{
    return (m_handle != NULL) ? true : false;
}

bool QLCFTDI::reset()
{
    FT_STATUS status = FT_ResetDevice(m_handle);
    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << name() << status;
        return false;
    }
    else
    {
        return true;
    }
}

bool QLCFTDI::setLineProperties()
{
    FT_STATUS status = FT_SetDataCharacteristics(m_handle, 8, 2, 0);
    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << name() << status;
        return false;
    }
    else
    {
        return true;
    }
}

bool QLCFTDI::setBaudRate()
{
    FT_STATUS status = FT_SetBaudRate(m_handle, 250000);
    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << name() << status;
        return false;
    }
    else
    {
        return true;
    }
}

bool QLCFTDI::setFlowControl()
{
    FT_STATUS status = FT_SetFlowControl(m_handle, 0, 0, 0);
    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << name() << status;
        return false;
    }
    else
    {
        return true;
    }
}

bool QLCFTDI::clearRts()
{
    FT_STATUS status = FT_ClrRts(m_handle);
    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << name() << status;
        return false;
    }
    else
    {
        return true;
    }
}

bool QLCFTDI::purgeBuffers()
{
    FT_STATUS status = FT_Purge(m_handle, FT_PURGE_RX | FT_PURGE_TX);
    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << name() << status;
        return false;
    }
    else
    {
        return true;
    }
}

bool QLCFTDI::setBreak(bool on)
{
    FT_STATUS status;
    if (on == true)
        status = FT_SetBreakOn(m_handle);
    else
        status = FT_SetBreakOff(m_handle);

    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << name() << status;
        return false;
    }
    else
    {
        return true;
    }
}

bool QLCFTDI::write(const QByteArray& data)
{
    DWORD written = 0;
    FT_STATUS status = FT_Write(m_handle, (char*) data.data(), data.size(), &written);
    if (status != FT_OK)
    {
        qWarning() << Q_FUNC_INFO << name() << status;
        return false;
    }
    else
    {
        return true;
    }
}

QByteArray QLCFTDI::read(int size, uchar* userBuffer)
{
    uchar* buffer = NULL;

    if (userBuffer == NULL)
        buffer = (uchar*) malloc(sizeof(uchar) * size);
    else
        buffer = userBuffer;
    Q_ASSERT(buffer != NULL);

    int read = 0;
    QByteArray array;
    FT_Read(m_handle, buffer, size, (LPDWORD) &read);
    if (userBuffer == NULL)
    {
        for (int i = 0; i < read; i++)
            array.append((char) buffer[i]);
    }
    else
    {
        array = QByteArray::fromRawData((char*) buffer, read);
    }

    if (userBuffer == NULL)
        free(buffer);

    return array;
}

uchar QLCFTDI::readByte(bool* ok)
{
    uchar byte = 0;
    int read = 0;
    FT_Read(m_handle, &byte, 1, (LPDWORD) &read);
    if (read == 1)
    {
        if (ok)
            *ok = true;
        return byte;
    }
    else
    {
        if (ok)
            *ok = false;
        return 0;
    }
}
