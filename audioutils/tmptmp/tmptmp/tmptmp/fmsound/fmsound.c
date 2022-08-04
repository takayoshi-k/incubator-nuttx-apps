/****************************************************************************
 * modules/synthesizer/fmsound/fmsound.c
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

#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <limits.h>

#include <synthesizer/fmsound/fmsound.h>

#define WRAP_ROUND_TIME_SEC (120)

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * name: update_phase
 ****************************************************************************/

static void update_phase(FAR fmsound_t *fmsnd, int fs)
{
  fmsnd->phase_time++;
  if (fmsnd->phase_time >= fs * WRAP_ROUND_TIME_SEC)
    {
      fmsnd->phase_time = 0;
    }
}

/****************************************************************************
 * name: latch_fbvalues
 ****************************************************************************/

static void latch_fbvalues(FAR fmsound_t *fmsnd)
{
  int i;
  for (i = 0; i < fmsnd->max_carriers; i++)
    {
      if (fmsnd->carrier[i])
        {
          fmsnd_latch_opfbvalue(fmsnd->carrier[i]);
        }
    }
}

/****************************************************************************
 * name: update_snd_fs
 ****************************************************************************/

static void update_snd_fs(FAR fmsound_t *fmsnd)
{
  int i;
  for (i = 0; i < fmsnd->max_carriers; i++)
    {
      if (fmsnd->carrier[i])
        {
          fmsnd_set_opfs(fmsnd->carrier[i], fmsnd->fs);
        }
    }
}

/****************************************************************************
 * name: update_fs
 ****************************************************************************/

static void update_fs(FAR fmsndrender_t *fmrndr)
{
  int i;
  fmsound_t *sound = fmrndr->sounds;
  while (sound)
    {
      for (i = 0; i < sound->max_carriers; i++)
        {
          fmsnd_set_fs(sound, fmrndr->fs);
        }
      sound = sound->next_sound;
    }
}

/****************************************************************************
 * name: restart_carrier
 ****************************************************************************/

static void restart_carrier(FAR fmsound_t *fmsnd)
{
  int i;
  for (i = 0; i < fmsnd->max_carriers; i++)
    {
      if (fmsnd->carrier[i])
        {
          fmsnd_set_opfs(fmsnd->carrier[i], fmsnd->fs);
          fmsnd_set_opsoundfreq(fmsnd->carrier[i], fmsnd->modulation_freq);
          fmsnd_start_op(fmsnd->carrier[i]);
        }
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * name: fmsnd_initialize
 ****************************************************************************/

int fmsnd_initialize(FAR fmsound_t *fmsnd, int carriernum)
{
  if (carriernum <= 0)
    {
      return -EINVAL;
    }

  fmsnd->phase_time = 0;
  fmsnd->carrier_level = 1.f;
  fmsnd->max_carriers = carriernum;
  fmsnd->carrier = NULL;
  fmsnd->modulation_freq = 0.f;
  fmsnd->next_sound = NULL;
  fmsnd->fs = 0;

  if (carriernum)
    {
      fmsnd->carrier = (FAR fmsnd_op_t **)
                  calloc(carriernum, sizeof(fmsnd_op_t *));
      if (!fmsnd->carrier)
        {
          return -ENOMEM;
        }
    }

  return OK;
}

/****************************************************************************
 * name: fmsnd_finalize
 ****************************************************************************/

void fmrenderer_finalize(FAR fmsndrender_t *fmrndr)
{
  int i;

  fmsound_t *sound = fmrndr->sounds;
  while (sound)
    {
      if (sound->carrier)
        {
          for (i = 0; i < sound->max_carriers && sound->carrier[i]; i++)
            {
              fmsnd_fin_operator(sound->carrier[i]);
              sound->carrier[i] = NULL;
            }
          free(sound->carrier);
        }
      sound = sound->next_sound;
    }
}

/****************************************************************************
 * name: fmsnd_set_carrier
 ****************************************************************************/

int fmsnd_set_carrier(FAR fmsound_t *fmsnd,
                        FAR fmsnd_op_t *op)
{
  int i;
  FAR fmsnd_op_t **carrier = fmsnd->carrier;

  for (i = 0; i < fmsnd->max_carriers && *carrier; i++, carrier++)
    {
    }

  if (i < fmsnd->max_carriers)
    {
      *carrier = op;
      restart_carrier(fmsnd);
      return OK;
    }

  return -ENOSPC;
}

/****************************************************************************
 * name: fmsnd_set_soundfreq
 ****************************************************************************/

void fmsnd_set_soundfreq(FAR fmsound_t *fmsnd, float f)
{
  fmsnd->modulation_freq = f;
  restart_carrier(fmsnd);
}

/****************************************************************************
 * name: fmsnd_modulate
 ****************************************************************************/

float fmsnd_modulate(FAR fmsound_t *fmsnd, int fs)
{
  int op;
  float out = 0.f;

  if (fmsnd->carrier == NULL || fmsnd->carrier[0] == NULL)
    {
      return out;
    }

  latch_fbvalues(fmsnd);

  for (op = 0; op < fmsnd->max_carriers; op++)
    {
      if (fmsnd->carrier[op])
        {
          out += fmsnd_operate_op(fmsnd->carrier[op], fmsnd->phase_time, 0);
        }
    }

  /* TODO: Condider if the out should divide max_carriers. */

  update_phase(fmsnd, fs);

  return out * fmsnd->carrier_level;
}

/****************************************************************************
 * name: fmsnd_set_fs
 ****************************************************************************/

void fmsnd_set_fs(FAR fmsound_t *fmsnd, int fs)
{
  fmsnd->fs = fs;
  update_snd_fs(fmsnd);
}

/****************************************************************************
 * name: fmrenderer_setfs
 ****************************************************************************/

void fmrenderer_setfs(FAR fmsndrender_t *fmrndr, int fs)
{
  fmrndr->fs = fs;
  update_fs(fmrndr);
}

/****************************************************************************
 * name: fmrenderer_initialize
 ****************************************************************************/

void fmrenderer_initialize(FAR fmsndrender_t *fmrndr)
{
  fmrndr->fs = 0;
  fmrndr->output_gain = (float)SHRT_MAX;
  fmrndr->sounds = NULL;
}

/****************************************************************************
 * name: fmrenderer_rendering_16
 ****************************************************************************/

int fmrenderer_rendering_16(FAR fmsndrender_t *fmrndr,
                      FAR int16_t *sample, int sample_num, int chnum,
                      fmsnd_tickcb_t cb, unsigned long cbarg)
{
  int i;
  int ch;
  float out;
  FAR fmsound_t *sounds;

  if (sample == NULL || sample_num < 0 || chnum < 0)
    {
      return -EINVAL;
    }

  if (!fmrndr || fmrndr->fs <= 0)
    {
      return -ECANCELED;
    }

  for (i = 0; i < sample_num; i += chnum)
    {
      sounds = fmrndr->sounds;

      out = 0.f;
      while (sounds)
        {
          out += fmsnd_modulate(sounds, fmrndr->fs);
          sounds = sounds->next_sound;
        }

      out *= fmrndr->output_gain;
      for (ch = 0; ch < chnum; ch++)
        {
          *sample++ = (int16_t)out;
        }

      if (cb)
        {
          cb(cbarg, fmrndr);
        }
    }

  if (i > sample_num)
    {
      i -= chnum;
    }

  /* Return total bytes stored in the buffer */

  return i * sizeof(int16_t);
}

/****************************************************************************
 * name: fmrenderer_attach_sound
 ****************************************************************************/

void fmrenderer_attach_sound(FAR fmsndrender_t *fmrndr, FAR fmsound_t *fmsnd)
{
  FAR fmsound_t *snd = fmrndr->sounds;

  fmsnd->next_sound = NULL;

  if (!snd)
    {
      fmrndr->sounds = fmsnd;
    }
  else
    {
      while (snd->next_sound)
        {
          snd = snd->next_sound;
        }
      snd->next_sound = fmsnd;
    }

  update_fs(fmrndr);
}
