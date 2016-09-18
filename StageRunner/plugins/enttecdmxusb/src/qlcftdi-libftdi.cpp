/*
  Q Light Controller
  qlcftdi-libftdi.cpp

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
#include <ftdi.h>
#include <QDebug>
#include <QMap>

#include "enttecdmxusbwidget.h"
#include "enttecdmxusbprotx.h"
#include "enttecdmxusbprorx.h"
#include "enttecdmxusbopen.h"
#include "qlcftdi.h"

QLCFTDI::QLCFTDI(const QString& serial, const QString& name, quint32 id)
	: m_serial(serial)
	, m_name(name)
	, m_id(id)
{
	bzero(&m_handle, sizeof(struct ftdi_context));
	ftdi_init(&m_handle);
}

QLCFTDI::~QLCFTDI()
{
	if (isOpen() == true)
		close();
	ftdi_deinit(&m_handle);
}

QList <EnttecDMXUSBWidget*> QLCFTDI::widgets()
{
	QList <EnttecDMXUSBWidget*> widgetList;
	struct ftdi_device_list* list = 0;
	struct ftdi_context ftdi;
	ftdi_init(&ftdi);
	ftdi_usb_find_all(&ftdi, &list, QLCFTDI::VID, QLCFTDI::PID);
	while (list != NULL)
	{
#ifdef LIBFTDI_1_2
		struct libusb_device* dev = list->dev;
#else
		struct usb_device* dev = list->dev;
#endif
		Q_ASSERT(dev != NULL);

		char serial[256];
		char name[256];
		char vendor[256];

		ftdi_usb_get_strings(&ftdi, dev,
							 vendor, sizeof(vendor),
							 name, sizeof(name),
							 serial, sizeof(serial));

		QString ser(serial);
		QString ven(vendor);
		QMap <QString,QVariant> types(typeMap());

		if (types.contains(ser) == true)
		{
			// Force a widget with a specific serial to either type
			EnttecDMXUSBWidget::Type type = (EnttecDMXUSBWidget::Type) types[ser].toInt();
			switch (type)
			{
			case EnttecDMXUSBWidget::OpenTX:
				widgetList << new EnttecDMXUSBOpen(serial, name);
				break;
			case EnttecDMXUSBWidget::ProRX:
			{
				EnttecDMXUSBProRX* prorx = new EnttecDMXUSBProRX(serial, name, widgetList.size());
				widgetList << prorx;
				break;
			}
			default:
			case EnttecDMXUSBWidget::ProTX:
				widgetList << new EnttecDMXUSBProTX(serial, name);
				break;
			}
		}
		else if (ven.toUpper().contains("FTDI") == true)
		{
			/* This is probably an Open DMX USB widget */
			widgetList << new EnttecDMXUSBOpen(serial, name, 0);
		}
		else
		{
			/* This is probably a DMX USB Pro widget in TX mode */
			widgetList << new EnttecDMXUSBProTX(serial, name);
		}

		list = list->next;
	}

	ftdi_deinit(&ftdi);
	return widgetList;
}

bool QLCFTDI::open()
{
	if (ftdi_usb_open_desc(&m_handle, QLCFTDI::VID, QLCFTDI::PID,
						   name().toLocal8Bit(), serial().toLocal8Bit()) < 0)
	{
		qWarning() << Q_FUNC_INFO << name() << ftdi_get_error_string(&m_handle);
		return false;
	}
	else
	{
		return true;
	}
}

bool QLCFTDI::close()
{
	if (ftdi_usb_close(&m_handle) < 0)
	{
		qWarning() << Q_FUNC_INFO << name() << ftdi_get_error_string(&m_handle);
		return false;
	}
	else
	{
		return true;
	}
}

bool QLCFTDI::isOpen() const
{
	return (m_handle.usb_dev != NULL) ? true : false;
}

bool QLCFTDI::reset()
{
	if (ftdi_usb_reset(&m_handle) < 0)
	{
		qWarning() << Q_FUNC_INFO << name() << ftdi_get_error_string(&m_handle);
		return false;
	}
	else
	{
		return true;
	}
}

bool QLCFTDI::setLineProperties()
{
	if (ftdi_set_line_property(&m_handle, BITS_8, STOP_BIT_2, NONE) < 0)
	{
		qWarning() << Q_FUNC_INFO << name() << ftdi_get_error_string(&m_handle);
		return false;
	}
	else
	{
		return true;
	}
}

bool QLCFTDI::setBaudRate()
{
	if (ftdi_set_baudrate(&m_handle, 250000) < 0)
	{
		qWarning() << Q_FUNC_INFO << name() << ftdi_get_error_string(&m_handle);
		return false;
	}
	else
	{
		return true;
	}
}

bool QLCFTDI::setFlowControl()
{
	if (ftdi_setflowctrl(&m_handle, SIO_DISABLE_FLOW_CTRL) < 0)
	{
		qWarning() << Q_FUNC_INFO << name() << ftdi_get_error_string(&m_handle);
		return false;
	}
	else
	{
		return true;
	}
}

bool QLCFTDI::clearRts()
{
	if (ftdi_setrts(&m_handle, 0) < 0)
	{
		qWarning() << Q_FUNC_INFO << name() << ftdi_get_error_string(&m_handle);
		return false;
	}
	else
	{
		return true;
	}
}

bool QLCFTDI::purgeBuffers()
{
	if (ftdi_usb_purge_buffers(&m_handle) < 0)
	{
		qWarning() << Q_FUNC_INFO << name() << ftdi_get_error_string(&m_handle);
		return false;
	}
	else
	{
		return true;
	}
}

bool QLCFTDI::setBreak(bool on)
{
	ftdi_break_type type;
	if (on == true)
		type = BREAK_ON;
	else
		type = BREAK_OFF;

	if (ftdi_set_line_property2(&m_handle, BITS_8, STOP_BIT_2, NONE, type) < 0)
	{
		qWarning() << Q_FUNC_INFO << name() << ftdi_get_error_string(&m_handle);
		return false;
	}
	else
	{
		return true;
	}
}

bool QLCFTDI::write(const QByteArray& data)
{
	if (ftdi_write_data(&m_handle, (uchar*) data.data(), data.size()) < 0)
	{
		qWarning() << Q_FUNC_INFO << name() << ftdi_get_error_string(&m_handle);
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

	QByteArray array;
	int read = ftdi_read_data(&m_handle, buffer, size);
	if (userBuffer != NULL)
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
	int read = ftdi_read_data(&m_handle, &byte, 1);
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
