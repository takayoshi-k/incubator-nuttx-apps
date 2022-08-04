/****************************************************************************
 * examples/fmsound/mmlplayer/mmlplayer_main.h
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <limits.h>

#include <nuttx/audio/audio.h>
#include <synthesizer/fmsound/fmsound.h>
#include <synthesizer/fmsound/fmsound_operator.h>
#include <utils/mml_parser/mml_parser.h>

#include "fmsound_nxaudio.h"
#include "operator_algorithm.h"
#include "music_scale.h"

#include "mmlplayer_score.h"

/****************************************************************************
 * Pre-processor
 ****************************************************************************/

#define APP_FS    (48000)
#define APP_BPS   (16)
#define APP_CHNUM (2)
#define CARRIER_LEVEL (1.f / 3.f)

/****************************************************************************
 * Private Data Type
 ****************************************************************************/

struct app_options
{
  int volume;
  int mode;
};

struct mmlplayer_s
{
  struct nxaudio_s nxaudio;

  /* Rendarer */

  fmsndrender_t fmrndr;

  /* Right hand sound */

  fmsound_t  rsound[2];
  fmsnd_op_t rop[2][2];
  int rtick;
  char *rscore;
  struct music_macro_lang_s rmml;

  /* Left hand sound */

  fmsound_t  lsound[1];
  fmsnd_op_t lop[1][2];
  int ltick;
  char *lscore;
  struct music_macro_lang_s lmml;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

extern int board_external_amp_mute_control(bool en);
static void app_dequeue_cb(unsigned long arg,
                           FAR struct ap_buffer_s *apb);
static void app_complete_cb(unsigned long arg);
static void app_user_cb(unsigned long arg,
                        FAR struct audio_msg_s *msg, FAR bool *running);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct mmlplayer_s g_mmlplayer;

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
 * name: print_note
 ****************************************************************************/

static void print_note(bool LR, int index, int length)
{
  printf("%c: O%d%c : %d\n", LR ? 'R' : 'L',
                index / 12, "CcDdEFfGgAaB"[index % 12], length);
}

/****************************************************************************
 * name: print_chord
 ****************************************************************************/

static void print_chord(bool LR, int index1, int index2, int length)
{
  printf("%c: [O%d%c, O%d%c] : %d\n", LR ? 'R' : 'L',
                index1 / 12, "CcDdEFfGgAaB"[index1 % 12],
                index2 / 12, "CcDdEFfGgAaB"[index2 % 12],
                length);
}

/****************************************************************************
 * name: update_righthand_note
 ****************************************************************************/

static void update_righthand_note(FAR struct mmlplayer_s *fmmsc)
{
  int mml_ret;
  struct mml_result_s mml_result;

  fmmsc->rtick = 0;
  do
    {
      mml_ret = parse_mml(&fmmsc->rmml, &fmmsc->rscore, &mml_result);
      switch (mml_ret)
        {
          case MML_TYPE_NOTE:
            fmmsc->rtick = mml_result.length;
            fmsnd_set_soundfreq(&fmmsc->rsound[0],
                                    musical_scale[mml_result.note_idx[0]]);
            fmsnd_set_carrierlevel(&fmmsc->rsound[0], CARRIER_LEVEL);
            fmsnd_set_carrierlevel(&fmmsc->rsound[1], 0.f);
            print_note(1, mml_result.note_idx[0], mml_result.length);
            break;

          case MML_TYPE_CHORD:
            fmmsc->rtick = mml_result.length;
            fmsnd_set_soundfreq(&fmmsc->rsound[0],
                                    musical_scale[mml_result.note_idx[0]]);
            fmsnd_set_soundfreq(&fmmsc->rsound[1],
                                    musical_scale[mml_result.note_idx[1]]);
            fmsnd_set_carrierlevel(&fmmsc->rsound[0], CARRIER_LEVEL);
            fmsnd_set_carrierlevel(&fmmsc->rsound[1], CARRIER_LEVEL);
            print_chord(1, mml_result.note_idx[0], mml_result.note_idx[1],
                                                        mml_result.length);
            break;

          case MML_TYPE_REST:
            fmmsc->rtick = mml_result.length;
            fmsnd_set_carrierlevel(&fmmsc->rsound[0], 0.f);
            fmsnd_set_carrierlevel(&fmmsc->rsound[1], 0.f);
            printf("R: Rest : %d\n", mml_result.length);
            break;
        }
    }
  while (!fmmsc->rtick && mml_ret != MML_TYPE_EOF);
}

/****************************************************************************
 * name: update_lefthand_note
 ****************************************************************************/

static void update_lefthand_note(FAR struct mmlplayer_s *fmmsc)
{
  int mml_ret;
  struct mml_result_s mml_result;

  fmmsc->ltick = 0;
  do
    {
      mml_ret = parse_mml(&fmmsc->lmml, &fmmsc->lscore, &mml_result);
      switch (mml_ret)
        {
          case MML_TYPE_NOTE:
            fmmsc->ltick = mml_result.length;
            fmsnd_set_soundfreq(&fmmsc->lsound[0],
                                    musical_scale[mml_result.note_idx[0]]);
            fmsnd_set_carrierlevel(&fmmsc->lsound[0], CARRIER_LEVEL);
            print_note(0, mml_result.note_idx[0], mml_result.length);
            break;

          case MML_TYPE_REST:
            fmmsc->ltick = mml_result.length;
            fmsnd_set_carrierlevel(&fmmsc->lsound[0], 0.f);
            printf("L: Rest : %d\n", mml_result.length);
            break;
        }
    }
  while (!fmmsc->ltick && mml_ret != MML_TYPE_EOF);
}

/****************************************************************************
 * name: tick_callback
 ****************************************************************************/

static void tick_callback(unsigned long arg, FAR fmsndrender_t *fmrndr)
{
  FAR struct mmlplayer_s *fmmsc = (FAR struct mmlplayer_s *)arg;

  fmmsc->rtick--;
  fmmsc->ltick--;

  if (fmmsc->rtick <= 0)
    {
      update_righthand_note(fmmsc);
    }

  if (fmmsc->ltick <= 0)
    {
      update_lefthand_note(fmmsc);
    }
}

/****************************************************************************
 * name: app_dequeue_cb
 ****************************************************************************/

static void app_dequeue_cb(unsigned long arg,
                           FAR struct ap_buffer_s *apb)
{
  FAR struct mmlplayer_s *mmlplayer = (struct mmlplayer_s *)arg;

  apb->curbyte = 0;
  apb->flags = 0;
  apb->nbytes = fmrenderer_rendering_16(&mmlplayer->fmrndr, (int16_t *)apb->samp, 
                           apb->nmaxbytes / sizeof(int16_t),
                           mmlplayer->nxaudio.chnum,
                           tick_callback, (unsigned long)mmlplayer);
  nxaudio_enqbuffer(&mmlplayer->nxaudio, apb);
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
 * name: autio_loop_thread
 ****************************************************************************/

static void *audio_loop_thread(pthread_addr_t arg)
{
  struct mmlplayer_s *mmlplayer = (struct mmlplayer_s *)arg;

  nxaudio_start(&mmlplayer->nxaudio);
  nxaudio_msgloop(&mmlplayer->nxaudio, &cbs, (unsigned long)mmlplayer);

  return NULL;
}

/****************************************************************************
 * name: create_audio_thread
 ****************************************************************************/

static pthread_t create_audio_thread(FAR struct mmlplayer_s *mmlplayer)
{
  pthread_t pid;
  pthread_attr_t tattr;
  struct sched_param sparam;

  pthread_attr_init(&tattr);
  sparam.sched_priority = sched_get_priority_max(SCHED_FIFO) - 9;
  pthread_attr_setschedparam(&tattr, &sparam);
  pthread_attr_setstacksize(&tattr, 4096);

  pthread_create(&pid, &tattr, audio_loop_thread,
                              (pthread_addr_t)mmlplayer);
  pthread_setname_np(pid, "mmlplayer_thread");

  return pid;
}

/****************************************************************************
 * name: make_sound
 ****************************************************************************/

static void make_sound(int mode, int fs, FAR fmsound_t *fmsnd,
                                                         fmsnd_op_t op[2])
{
  switch (mode)
    {
      case 0:
        fm_algorithm0(fmsnd, &op[0], fs);
        break;
      case 1:
        fm_algorithm1(fmsnd, &op[0], &op[1], fs);
        break;
      case 2:
        fm_algorithm2(fmsnd, &op[0], fs);
        break;
      case 3:
        fm_algorithm3(fmsnd, &op[0], &op[1], fs);
        break;
      case 4:
        fm_algorithm4(fmsnd, &op[0], &op[1], fs);
        break;
    }
}

/****************************************************************************
 * name: init_fmmusi_soundsc
 ****************************************************************************/

static void init_mmlplayer_sound(FAR struct mmlplayer_s *mmlplayer, int fs,
                                                               int mode)
{
  fmrenderer_initialize(&mmlplayer->fmrndr);
  fmrenderer_setfs(&mmlplayer->fmrndr, fs);

  fmsnd_initialize(&mmlplayer->rsound[0], 1);
  fmsnd_init_operator(&mmlplayer->rop[0][0], 1);
  fmsnd_init_operator(&mmlplayer->rop[0][1], 0);
  make_sound(mode, fs, &mmlplayer->rsound[0], mmlplayer->rop[0]);

  fmsnd_initialize(&mmlplayer->rsound[1], 1);
  fmsnd_init_operator(&mmlplayer->rop[1][0], 1);
  fmsnd_init_operator(&mmlplayer->rop[1][1], 0);
  make_sound(mode, fs, &mmlplayer->rsound[1], mmlplayer->rop[1]);

  fmsnd_initialize(&mmlplayer->lsound[0], 1);
  fmsnd_init_operator(&mmlplayer->lop[0][0], 1);
  fmsnd_init_operator(&mmlplayer->lop[0][1], 0);
  make_sound(mode, fs, &mmlplayer->lsound[0], mmlplayer->lop[0]);

  fmrenderer_attach_sound(&mmlplayer->fmrndr, &mmlplayer->rsound[0]);
  fmrenderer_attach_sound(&mmlplayer->fmrndr, &mmlplayer->rsound[1]);
  fmrenderer_attach_sound(&mmlplayer->fmrndr, &mmlplayer->lsound[0]);

  mmlplayer->rtick = 0;
  mmlplayer->ltick = 0;

  init_mml(&mmlplayer->rmml, fs, 120, 4, 4);
  init_mml(&mmlplayer->lmml, fs, 120, 4, 3);

  mmlplayer->rscore = (char *)floh_walzer_right;
  mmlplayer->lscore  = (char *)floh_walzer_left;
}

/****************************************************************************
 * name: fin_mmlplayer
 ****************************************************************************/

static void fin_mmlplayer(FAR struct mmlplayer_s *mmlplayer)
{
  fmrenderer_finalize(&mmlplayer->fmrndr);
  fin_nxaudio(&mmlplayer->nxaudio);
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

  appopt.volume = 1000;
  appopt.mode = 0;

  printf("Start %s\n", argv[0]);

  if (configure_option(&appopt, argc, argv) != OK)
    {
      print_help(argv[0]);
      return -1;
    }

  ret = init_nxaudio(&g_mmlplayer.nxaudio, APP_FS, APP_BPS, APP_CHNUM);
  if (ret < 0)
    {
      printf("init_nxaoud() returned with error!!\n");
      return -1;
    }
  ret = nxaudio_setvolume(&g_mmlplayer.nxaudio, appopt.volume);

  init_mmlplayer_sound(&g_mmlplayer, APP_FS, appopt.mode);

  for (i = 0; i < g_mmlplayer.nxaudio.abufnum; i++)
    {
      app_dequeue_cb((unsigned long)&g_mmlplayer,
                           g_mmlplayer.nxaudio.abufs[i]);
    }

  pid = create_audio_thread(&g_mmlplayer);

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
            }
        }
    }

  board_external_amp_mute_control(true);
  nxaudio_stop(&g_mmlplayer.nxaudio);
  pthread_join(pid, NULL);

  fin_mmlplayer(&g_mmlplayer);

  return ret;
}
