/****************************************************************************
 * apps/examples/fmsynth/utils/fmsynth_nxaudio.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __EXAMPLES_FMSYNTH_MMLPLAYER_FMSYNTH_NXAUDIO_H
#define __EXAMPLES_FMSYNTH_MMLPLAYER_FMSYNTH_NXAUDIO_H

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

int init_mmlplayer_nxaudio(FAR struct nxaudio_s *nxaudio,
                           int fs, int bps, int chnum);
void fin_mmlplayer_nxaudio(FAR struct nxaudio_s *nxaudio);
int mmlplayer_nxaudio_enqbuffer(FAR struct nxaudio_s *nxaudio,
                                FAR struct ap_buffer_s *apb);
int mmlplayer_nxaudio_setvolume(FAR struct nxaudio_s *nxaudio, uint16_t vol);
int mmlplayer_nxaudio_start(FAR struct nxaudio_s *nxaudio);
int mmlplayer_nxaudio_msgloop(FAR struct nxaudio_s *nxaudio,
                              FAR struct nxaudio_callbacks_s *cbs,
                              unsigned long arg);
int mmlplayer_nxaudio_stop(FAR struct nxaudio_s *nxaudio);

#endif  /* __EXAMPLES_FMSYNTH_MMLPLAYER_FMSYNTH_NXAUDIO_H */
