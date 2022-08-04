/****************************************************************************
 * examples/fmsound/utils/fmsound_nxaudio.h
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

#ifndef __EXAMPLES_FMSOUND_UTILS_FMSOUND_NXAUDIO_H__
#define __EXAMPLES_FMSOUND_UTILS_FMSOUND_NXAUDIO_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include <mqueue.h>
#include <nuttx/audio/audio.h>

/****************************************************************************
 * Public Data Types
 ****************************************************************************/

struct nxaudio_s
{
  int fd;

  int abufnum;
  struct ap_buffer_s **abufs;
  mqd_t mq;

  int chnum;
};

struct nxaudio_callbacks_s
{
  void (*dequeue)(unsigned long arg, FAR struct ap_buffer_s *apb);
  void (*complete)(unsigned long arg);
  void (*user)(unsigned long arg, FAR struct audio_msg_s *msg,
                                              FAR bool *running);
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

int init_nxaudio(FAR struct nxaudio_s *nxaudio,
                         int fs, int bps, int chnum);
void fin_nxaudio(FAR struct nxaudio_s *nxaudio);
int nxaudio_enqbuffer(FAR struct nxaudio_s *nxaudio,
                              FAR struct ap_buffer_s *apb);
int nxaudio_setvolume(FAR struct nxaudio_s *nxaudio, uint16_t vol);
int nxaudio_start(FAR struct nxaudio_s *nxaudio);
int nxaudio_msgloop(FAR struct nxaudio_s *nxaudio,
                            FAR struct nxaudio_callbacks_s *cbs,
                            unsigned long arg);
int nxaudio_stop(FAR struct nxaudio_s *nxaudio);

#endif  /* __EXAMPLES_FMSOUND_UTILS_FMSOUND_NXAUDIO_H__ */
