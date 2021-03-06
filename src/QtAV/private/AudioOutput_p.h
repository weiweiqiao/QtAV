/******************************************************************************
	QtAV:  Media play library based on Qt and FFmpeg
    Copyright (C) 2012-2014 Wang Bin <wbsecg1@gmail.com>
    
*   This file is part of QtAV

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/


#ifndef QTAV_AUDIOOUTPUT_P_H
#define QTAV_AUDIOOUTPUT_P_H

#include <QtAV/private/AVOutput_p.h>
#include <QtAV/AudioFrame.h>
#include <QtCore/QQueue>
#include <QtCore/QVector>
#include <limits>

namespace QtAV {

class Q_AV_EXPORT AudioOutputPrivate : public AVOutputPrivate
{
public:
    AudioOutputPrivate():
        mute(false)
      , vol(1)
      , speed(1.0)
      , max_channels(1)
      , nb_buffers(8)
      , buffers_reseted(true)
      , index_enqueue(-1)
      , index_deuqueue(-1)
    {
        frame_infos.resize(nb_buffers);
    }
    virtual ~AudioOutputPrivate(){}

    typedef struct {
        qreal timestamp;
        int data_size;
    } FrameInfo;

    FrameInfo& currentEnqueueInfo() {
        Q_ASSERT(index_enqueue >= 0);
        return frame_infos[index_enqueue%frame_infos.size()];
    }
    FrameInfo& nextEnqueueInfo() { return frame_infos[(index_enqueue + 1)%frame_infos.size()]; }
    const FrameInfo& nextEnqueueInfo() const { return frame_infos[(index_enqueue + 1)%frame_infos.size()]; }
    FrameInfo& currentDequeueInfo() {
        Q_ASSERT(index_deuqueue >= 0);
        return frame_infos[index_deuqueue%frame_infos.size()];
    }
    FrameInfo& nextDequeueInfo() { return frame_infos[(index_deuqueue+1)%frame_infos.size()]; }
    const FrameInfo& nextDequeueInfo() const { return frame_infos[(index_deuqueue+1)%frame_infos.size()]; }
    void resetBuffers() {
        index_enqueue = -1;
        index_deuqueue = -1;
    }
    bool isBufferReseted() const { return index_enqueue == -1 && index_deuqueue == -1; }
    bool canAddBuffer() {
        return isBufferReseted() || (index_enqueue - index_deuqueue  < frame_infos.size()&& index_deuqueue >= 0);
        return isBufferReseted() || (!!((index_enqueue - index_deuqueue + 1) % frame_infos.size()) && index_deuqueue >= 0);
    }
    void bufferAdded() {
        if (index_enqueue < 0 || index_enqueue == std::numeric_limits<int>::max())
            index_enqueue = 0;
        else
            ++index_enqueue;
        return;
        index_enqueue = (index_enqueue + 1) % frame_infos.size();
    }
    bool canRemoveBuffer() {
        return index_enqueue > index_deuqueue;
    }
    void bufferRemoved() {
        if (index_deuqueue == index_enqueue)
            return;
        if (index_deuqueue < 0 || index_deuqueue == std::numeric_limits<int>::max())
            index_deuqueue = 0;
        else
            ++index_deuqueue;
        return;
        index_deuqueue = (index_deuqueue + 1) % frame_infos.size();
    }

    bool mute;
    qreal vol;
    qreal speed;
    int max_channels;
    AudioFormat format;
    QByteArray data;
    AudioFrame audio_frame;
    quint32 nb_buffers;

private:
    bool buffers_reseted;
    // the index of current enqueue/dequeue
    int index_enqueue, index_deuqueue;
    QVector<FrameInfo> frame_infos;
};

} //namespace QtAV
#endif // QTAV_AUDIOOUTPUT_P_H
