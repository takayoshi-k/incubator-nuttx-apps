/****************************************************************************
 * examples/fmsound/utils/fmsound_nxaudio.c
 *
 *   Copyright 2022 Sony Semiconductor Solutions Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/ioctl.h>
#include <nuttx/audio/audio.h>

#include <errno.h>

#include "fmsound_nxaudio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define AUDIO_DEVICE_PATH "/dev/audio/pcm1"
#define FMMUSIC_NXAUDIO_MQ "/tmp/fmaudio_mq"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * name: configure_audio
 ****************************************************************************/

static int configure_audio(int fd, int ch, int fs, int bps, int chmap)
{
  struct audio_caps_desc_s cap;

  cap.caps.ac_len = sizeof(struct audio_caps_s);
  cap.caps.ac_type = AUDIO_TYPE_OUTPUT;
  cap.caps.ac_channels = ch;
  cap.caps.ac_chmap = chmap;
  cap.caps.ac_controls.hw[0] = fs;
  cap.caps.ac_controls.b[2] = bps;
  cap.caps.ac_controls.b[3] = 0;  /* Just set 0 */

  return ioctl(fd, AUDIOIOC_CONFIGURE, (unsigned long)&cap);
}

/****************************************************************************
 * name: create_audiomq
 ****************************************************************************/

static mqd_t create_audiomq(int fd, int buf_num)
{
  mqd_t ret;
  struct mq_attr attr;

  attr.mq_maxmsg = buf_num;
  attr.mq_msgsize = sizeof(struct audio_msg_s);
  attr.mq_curmsgs = 0;
  attr.mq_flags = 0;

  ret = mq_open(FMMUSIC_NXAUDIO_MQ, O_RDWR | O_CREAT, 0644, &attr);
  printf("mq_open(%s) = %d\n", FMMUSIC_NXAUDIO_MQ, ret);
  if (ret >= (mqd_t) 0)
    {
      int rr;
      if ((rr = ioctl(fd, AUDIOIOC_REGISTERMQ, (unsigned long)ret)) < 0)
        {
          printf("mq register failed: %d, %d\n", rr, errno);
        }
    }

  return ret;
}

/****************************************************************************
 * name: create_audio_buffers
 ****************************************************************************/

static struct ap_buffer_s **create_audio_buffers(int fd, int num, int sz)
{
  int i;
  struct audio_buf_desc_s desc;
  struct ap_buffer_s **ret;

  ret = (struct ap_buffer_s **)calloc(num, sizeof(void *));

  for (i = 0; i < num; i++)
    {
      desc.numbytes = sz;
      desc.u.pbuffer = &ret[i];

      ioctl(fd, AUDIOIOC_ALLOCBUFFER, (unsigned long)&desc);
    }

  return ret;
}

/****************************************************************************
 * name: free_audio_buffers
 ****************************************************************************/

static void free_audio_buffers(FAR struct nxaudio_s *nxaudio)
{
  int x;
  struct audio_buf_desc_s desc;

  for (x = 0; x < nxaudio->abufnum; x++)
    {
      if (nxaudio->abufs[x] != NULL)
        {
          desc.u.buffer = nxaudio->abufs[x];
          ioctl(nxaudio->fd, AUDIOIOC_FREEBUFFER,
                (unsigned long)&desc);
        }
    }

  free(nxaudio->abufs);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * name: fin_nxaudio
 ****************************************************************************/

void fin_nxaudio(FAR struct nxaudio_s *nxaudio)
{
  free_audio_buffers(nxaudio);
  ioctl(nxaudio->fd, AUDIOIOC_STOP, 0);
  ioctl(nxaudio->fd, AUDIOIOC_UNREGISTERMQ, (unsigned long)nxaudio->mq);
  ioctl(nxaudio->fd, AUDIOIOC_RELEASE, 0);
  ioctl(nxaudio->fd, AUDIOIOC_SHUTDOWN, 0);
  mq_close(nxaudio->mq);
  mq_unlink(FMMUSIC_NXAUDIO_MQ);
  close(nxaudio->fd);
}

/****************************************************************************
 * name: init_nxaudio
 ****************************************************************************/

int init_nxaudio(FAR struct nxaudio_s *nxaudio,
                         int fs, int bps, int chnum)
{
  struct ap_buffer_info_s buf_info;

  nxaudio->fd = open(AUDIO_DEVICE_PATH, O_RDWR | O_CLOEXEC);
  if (nxaudio->fd >= 0)
    {
      if (ioctl(nxaudio->fd, AUDIOIOC_RESERVE, 0) < 0)
        {
          close(nxaudio->fd);
          return -1;
        }

      /* Audio configuration: 2 channels, 48 kFs, 16 bps */

      configure_audio(nxaudio->fd, chnum, fs, bps, 0);
      nxaudio->chnum = chnum;

      ioctl(nxaudio->fd, AUDIOIOC_GETBUFFERINFO, (unsigned long)&buf_info);

      /* Create message queue to communicate with audio driver */

      nxaudio->mq = create_audiomq(nxaudio->fd, buf_info.nbuffers + 8);

      /* Create message queue to communicate with audio driver */

      nxaudio->abufs = create_audio_buffers(nxaudio->fd,
                               buf_info.nbuffers, buf_info.buffer_size);
      nxaudio->abufnum = buf_info.nbuffers;

      return 0;
    }
  else
    {
      return -1;
    }
}

/****************************************************************************
 * name: nxaudio_enqbuffer
 ****************************************************************************/

int nxaudio_enqbuffer(FAR struct nxaudio_s *nxaudio,
                              FAR struct ap_buffer_s *apb)
{
  struct audio_buf_desc_s desc;

  desc.numbytes  = apb->nbytes;
  desc.u.buffer = apb;

  return ioctl(nxaudio->fd, AUDIOIOC_ENQUEUEBUFFER, (unsigned long)&desc);
}

/****************************************************************************
 * name: nxaudio_setvolume
 ****************************************************************************/

int nxaudio_setvolume(FAR struct nxaudio_s *nxaudio, uint16_t vol)
{
  struct audio_caps_desc_s  cap_desc;

  cap_desc.caps.ac_len            = sizeof(struct audio_caps_s);
  cap_desc.caps.ac_type           = AUDIO_TYPE_FEATURE;
  cap_desc.caps.ac_format.hw      = AUDIO_FU_VOLUME;
  cap_desc.caps.ac_controls.hw[0] = vol;

  return ioctl(nxaudio->fd, AUDIOIOC_CONFIGURE, (unsigned long)&cap_desc);
}

/****************************************************************************
 * name: nxaudio_start
 ****************************************************************************/

int nxaudio_start(FAR struct nxaudio_s *nxaudio)
{
  return ioctl(nxaudio->fd, AUDIOIOC_START, 0);
}

/****************************************************************************
 * name: nxaudio_start
 ****************************************************************************/

int nxaudio_stop(FAR struct nxaudio_s *nxaudio)
{
  struct audio_msg_s  term_msg;

  term_msg.msg_id = AUDIO_MSG_STOP;
  term_msg.u.data = 0;
  mq_send(nxaudio->mq, (FAR const char *)&term_msg, sizeof(term_msg), 0);

  return OK;
}

/****************************************************************************
 * name: nxaudio_msgloop
 ****************************************************************************/

int nxaudio_msgloop(FAR struct nxaudio_s *nxaudio,
                            FAR struct nxaudio_callbacks_s *cbs,
                            unsigned long arg)
{
  bool running = true;
  struct audio_msg_s msg;
  unsigned int prio;
  ssize_t size;

  if (!cbs)
    {
      return -1;
    }

  while (running)
    {
      size = mq_receive(nxaudio->mq, (FAR char *)&msg, sizeof(msg), &prio);
      if (size != sizeof(msg))
        {
          continue;
        }

      switch (msg.msg_id)
        {
          case AUDIO_MSG_DEQUEUE:
            if (cbs->dequeue)
              {
                cbs->dequeue(arg, msg.u.ptr);
              }
            break;
          case AUDIO_MSG_COMPLETE:
            if (cbs->complete)
              {
                cbs->complete(arg);
              }
            break;

          case AUDIO_MSG_STOP:
            running = false;
            break;

          case AUDIO_MSG_USER:
            if (cbs->user)
              {
                cbs->user(arg, &msg, &running);
              }
            break;
          default:
            break;
        }
    }

  return 0;
}
