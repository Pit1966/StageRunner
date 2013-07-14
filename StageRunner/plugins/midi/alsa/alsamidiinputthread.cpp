/*
  Q Light Controller
  alsamidiinputthread.cpp

  Copyright (c) Heikki Junnila

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
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <alsa/asoundlib.h>
#include <QDebug>

#include "alsamidiinputdevice.h"
#include "alsamidiinputthread.h"
#include "alsamidiutil.h"
#include "midiprotocol.h"

#define POLL_TIMEOUT_MS 1000

AlsaMidiInputThread::AlsaMidiInputThread(snd_seq_t* alsa,
                                         const snd_seq_addr_t* destinationAddress,
                                         QObject* parent)
    : QThread(parent)
    , m_alsa(alsa)
    , m_destinationAddress(new snd_seq_addr_t)
    , m_running(false)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(alsa != NULL);
    Q_ASSERT(destinationAddress != NULL);
    m_destinationAddress->client = destinationAddress->client;
    m_destinationAddress->port = destinationAddress->port;
}

AlsaMidiInputThread::~AlsaMidiInputThread()
{
    qDebug() << Q_FUNC_INFO;

    m_devices.clear();
    stop();

    delete m_destinationAddress;
    m_destinationAddress = NULL;
}

/****************************************************************************
 * Devices
 ****************************************************************************/

bool AlsaMidiInputThread::addDevice(AlsaMidiInputDevice* device)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(device != NULL);

    m_mutex.lock();

    /* Check, whether the hash table already contains the device */
    uint uid = device->uid().toUInt();
    if (m_devices.contains(uid) == true)
    {
        m_mutex.unlock();
        return false;
    }

    /* Subscribe the device's events */
    subscribeDevice(device);

    /* Insert the device into the hash map for later retrieval */
    m_devices.insert(uid, device);
    m_changed = true;

    /* Start the poller thread in case it's not running */
    if (m_running == false && isRunning() == false)
        start();

    m_mutex.unlock();

    return true;
}

bool AlsaMidiInputThread::removeDevice(AlsaMidiInputDevice* device)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(device != NULL);

    m_mutex.lock();

    uint uid = device->uid().toUInt();
    if (m_devices.remove(uid) > 0)
    {
        unsubscribeDevice(device);
        m_changed = true;
    }

    if (m_devices.size() == 0)
    {
        m_mutex.unlock();
        stop();
    }
    else
    {
        m_mutex.unlock();
    }

    return true;
}

void AlsaMidiInputThread::subscribeDevice(AlsaMidiInputDevice* device)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(device != NULL);

    /* Subscribe events coming from the the device's MIDI port to get
       patched to the plugin's own MIDI port */
    snd_seq_port_subscribe_t* sub = NULL;
    snd_seq_port_subscribe_alloca(&sub);
    snd_seq_port_subscribe_set_sender(sub, device->address());
    snd_seq_port_subscribe_set_dest(sub, m_destinationAddress);
    snd_seq_subscribe_port(m_alsa, sub);
}

void AlsaMidiInputThread::unsubscribeDevice(AlsaMidiInputDevice* device)
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(device != NULL);

    /* Unsubscribe events from the device */
    snd_seq_port_subscribe_t* sub = NULL;
    snd_seq_port_subscribe_alloca(&sub);
    snd_seq_port_subscribe_set_sender(sub, device->address());
    snd_seq_port_subscribe_set_dest(sub, m_destinationAddress);
    snd_seq_unsubscribe_port(m_alsa, sub);
}

/****************************************************************************
 * Poller thread
 ****************************************************************************/

void AlsaMidiInputThread::stop()
{
    qDebug() << Q_FUNC_INFO;

    m_mutex.lock();
    m_running = false;
    m_mutex.unlock();

    wait();
}

void AlsaMidiInputThread::run()
{
    qDebug() << Q_FUNC_INFO << "begin";

    struct pollfd* pfd = 0;
    int npfd = 0;

    m_mutex.lock();
    m_running = true;
    while (m_running == true)
    {
        if (m_changed == true)
        {
            // Poll descriptors must be re-evaluated
            npfd = snd_seq_poll_descriptors_count(m_alsa, POLLIN);
            pfd = (struct pollfd*) alloca(npfd * sizeof(struct pollfd));
            snd_seq_poll_descriptors(m_alsa, pfd, npfd, POLLIN);
            m_changed = false;
        }

        m_mutex.unlock();

        // Poll for MIDI events from the polled descriptors outside of mutex lock
        if (poll(pfd, npfd, POLL_TIMEOUT_MS) > 0)
            readEvent();

        m_mutex.lock();
    }
    m_mutex.unlock();

    qDebug() << Q_FUNC_INFO << "end";
}

void AlsaMidiInputThread::readEvent()
{
    m_mutex.lock();

    /* Wait for input data */
    do
    {
        AlsaMidiInputDevice* device = NULL;
        snd_seq_event_t* ev = NULL;

        /* Receive an event */
        snd_seq_event_input(m_alsa, &ev);

        // Find a device matching the event's address. If one isn't
        // found, skip this event, since we're not interested in it.
        uint uid = AlsaMidiUtil::addressToVariant(&ev->source).toUInt();
        if (m_devices.contains(uid) == true)
            device = m_devices[uid];
        else
            continue;
        Q_ASSERT(device != NULL);

        uchar cmd = 0;
        uchar data1 = 0;
        uchar data2 = 0;

        if (snd_seq_ev_is_control_type(ev))
        {
            cmd = MIDI_CONTROL_CHANGE | ev->data.control.channel;
            data1 = ev->data.control.param;
            data2 = ev->data.control.value;
        }
        else if (snd_seq_ev_is_note_type(ev))
        {
            if (ev->data.note.velocity == 0 && ev->data.note.off_velocity == 0)
                cmd = MIDI_NOTE_OFF | ev->data.note.channel;
            else
                cmd = MIDI_NOTE_ON | ev->data.note.channel;
            data1 = ev->data.note.note;
            data2 = ev->data.note.velocity;
        }

        // ALSA API is a bit controversial on this. snd_seq_event_input() says
        // it ALLOCATES the event but snd_seq_free_event() says this is not
        // needed because the event IS NOT allocated. No crashes observed
        // either way, so I guess freeing nevertheless is a bit safer.
        snd_seq_free_event(ev);

        uint channel = 0;
        uchar value = 0;
        if (QLCMIDIProtocol::midiToInput(cmd, data1, data2, device->midiChannel(),
                                         &channel, &value) == true)
        {
/*
            if (channel == CHANNEL_OFFSET_MBC)
                if (device->incrementMBCCount() == false)
                    continue;
*/
            device->emitValueChanged(channel, value);
        }
    } while (snd_seq_event_input_pending(m_alsa, 0) > 0);

    m_mutex.unlock();
}

