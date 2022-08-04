/****************************************************************************
 * apps/examples/fmsynth/musickeyboard/keyboard_main.c
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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <limits.h>
#include <math.h>

#include <nuttx/audio/audio.h>

#include "fmsynth_nxaudio.h"

/****************************************************************************
 * Pre-processor
 ****************************************************************************/

#define APP_FS    (48000)
#define APP_BPS   (16)
#define APP_CHNUM (2)
#define NXAUDIO_FREQ1 (400.f)
#define NXAUDIO_FREQ2 (900.f)

#define T() {printf("%s(%d)\n", __func__, __LINE__); sleep(1);}

static int phase_counter = 0;

static float deltas[2] = 
{
  2 * M_PI * NXAUDIO_FREQ1 / APP_FS,
  2 * M_PI * NXAUDIO_FREQ2 / APP_FS,
};

static float current_delta;
static int didx = 0;

/****************************************************************************
 * Private Data Type
 ****************************************************************************/

struct app_options
{
  int volume;
  int mode;
};

struct kbd_s
{
  struct nxaudio_s nxaudio;

  volatile int request_scale;
} g_kbd;

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static void app_dequeue_cb(unsigned long arg,
                           FAR struct ap_buffer_s *apb);
static void app_complete_cb(unsigned long arg);
static void app_user_cb(unsigned long arg,
                        FAR struct audio_msg_s *msg, FAR bool *running);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct nxaudio_callbacks_s cbs =
{
  app_dequeue_cb,
  app_complete_cb,
  app_user_cb
};

/****************************************************************************
 * Private functions
 ****************************************************************************/

/****************************************************************************
 * name: app_dequeue_cb
 ****************************************************************************/

static void app_dequeue_cb(unsigned long arg,
                           FAR struct ap_buffer_s *apb)
{
  FAR struct kbd_s *kbd = (struct kbd_s *)arg;
  int scale = kbd->request_scale;
  int16_t *sample = (int16_t *)apb->samp;
  int i;

  T();

  apb->curbyte = 0;
  apb->flags = 0;

  T();
  if (scale != -1)
    {
      didx ^= 1;
      current_delta = deltas[didx];
      kbd->request_scale = -1;
      phase_counter = 0;
    }

  T();
  for (i = 0; i < apb->nmaxbytes / sizeof(int16_t); i++)
    {
      if (sample == NULL)
        {
          printf("Sample is NULL\n");
          sleep(1);
        }
      sample[i*2] = (int16_t)(SHRT_MAX * sinf(current_delta * phase_counter++) * 0.5f);
      sample[i*2 + 1] = (int16_t)(SHRT_MAX * sinf(current_delta * phase_counter++) * 0.5f);
    }
  T();
  apb->nbytes = apb->nmaxbytes;

  T();
  nxaudio_enqbuffer(&kbd->nxaudio, apb);
  T();
}

/****************************************************************************
 * name: app_complete_cb
 ****************************************************************************/

static void app_complete_cb(unsigned long arg)
{
  /* Do nothing.. */
  printf("Audio loop is Done\n");
}

/****************************************************************************
 * name: app_user_cb
 ****************************************************************************/

static void app_user_cb(unsigned long arg,
                        FAR struct audio_msg_s *msg, FAR bool *running)
{
  /* Do nothing.. */
}

/****************************************************************************
 * name: audio_loop_thread
 ****************************************************************************/

static void *audio_loop_thread(pthread_addr_t arg)
{
  struct kbd_s *kbd = (struct kbd_s *)arg;

  nxaudio_start(&kbd->nxaudio);
  nxaudio_msgloop(&kbd->nxaudio, &cbs, (unsigned long)kbd);

  return NULL;
}

/****************************************************************************
 * name: create_audio_thread
 ****************************************************************************/

static pthread_t create_audio_thread(FAR struct kbd_s *kbd)
{
  pthread_t pid;
  pthread_attr_t tattr;
  struct sched_param sparam;

  pthread_attr_init(&tattr);
  sparam.sched_priority = sched_get_priority_max(SCHED_FIFO) - 9;
  pthread_attr_setschedparam(&tattr, &sparam);
  pthread_attr_setstacksize(&tattr, 4096);

  pthread_create(&pid, &tattr, audio_loop_thread,
                              (pthread_addr_t)kbd);
  pthread_setname_np(pid, "musickeyboard_thread");

  return pid;
}

/****************************************************************************
 * name: print_help
 ****************************************************************************/

static void print_help(FAR char *name)
{
  printf("nsh> %s ([-v (volume)]) ([-m (mode)])\n", name);
}

/****************************************************************************
 * name: configure_option
 ****************************************************************************/

static int configure_option(FAR struct app_options *option,
                              int argc, char **argv)
{
  int opt;

  option->volume = 1000;
  option->mode = 0;
  while ((opt = getopt(argc, argv, "v:m:")) != ERROR)
    {
      switch(opt)
        {
          case 'v':
            option->volume = atoi(optarg) * 10;
            if (option->volume < 0 || option->volume > 1000)
              {
                option->volume = 400;
              }
            break;
 
          case 'm':
            option->mode = atoi(optarg);
            if (option->mode < 0 || option->mode > 4)
              {
                option->mode = 0;
              }
            break;

          case 'h':
          case '?':
            return ERROR;
            break;

          default:
            return ERROR;
            break;
        }
    }

  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * name: main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int i;
  int ret;
  char key;
  bool running = true;
  pthread_t pid;
  struct app_options appopt;
  int key_idx;

  appopt.volume = 1000;
  appopt.mode = 0;

  printf("Start %s\n", argv[0]);

  if (configure_option(&appopt, argc, argv) != OK)
    {
      print_help(argv[0]);
      return -1;
    }

  ret = init_nxaudio(&g_kbd.nxaudio, APP_FS, APP_BPS, APP_CHNUM);
  if (ret < 0)
    {
      printf("init_nxaoud() returned with error!!\n");
      return -1;
    }
  g_kbd.request_scale = -1;

  T();
  nxaudio_setvolume(&g_kbd.nxaudio, appopt.volume);
  T();

  current_delta = deltas[0];

  /* Render audio samples in audio buffers */
  T();

  printf("abufnum = %d\n", g_kbd.nxaudio.abufnum);
  for (i = 0; i < g_kbd.nxaudio.abufnum; i++)
    {
      app_dequeue_cb((unsigned long)&g_kbd,
                           g_kbd.nxaudio.abufs[i]);
    }

  T();
  pid = create_audio_thread(&g_kbd);

  T();
  while (running)
    {
      key = (char)getchar();
      if (key != EOF)
        {
          switch (key)
            {
              case 'q':
                running = false;
                break;

              case 'c':
                g_kbd.request_scale = 1;
                break;
            }
        }
    }

  T();
  nxaudio_stop(&g_kbd.nxaudio);
  pthread_join(pid, NULL);

  T();
  fin_nxaudio(&g_kbd.nxaudio);

  T();
  return ret;
}
