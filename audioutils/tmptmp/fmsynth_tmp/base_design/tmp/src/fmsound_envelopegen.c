/****************************************************************************
 * modules/synthesizer/fmsound/fmsound_envelopegen.c
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

#include <sys/types.h>

#include <synthesizer/fmsound/fmsound.h>
#include <synthesizer/fmsound/fmsound_envelopegen.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * name: calc_delta
 ****************************************************************************/

static float calc_delta(float start_lv, float end_lv, int period)
{
  float ret = 0.f;
  if (period > 0)
    {
      ret = (end_lv - start_lv) / (float)period;
    }
  return ret;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * name: fmsnd_init_envelope
 ****************************************************************************/

int fmsnd_init_envelope(FAR fmsnd_eg_t *env)
{
  int i;

  env->state = ENVELOPE_STATE_RELEASED;
  env->state_counter = 0;

  for (i = 0; i < ENVELOPE_STATE_RELEASED; i++)
    {
      env->state_levels[i] = 1.f;
      env->state_periods[i] = 0;
    }
  env->state_levels[ENVELOPE_STATE_RELEASED] = 1.f;

  return OK;
}

/****************************************************************************
 * name: fmsnd_set_eglevels
 ****************************************************************************/

int fmsnd_set_eglevels(FAR fmsnd_eg_t *env,
                        float bl, float al, float dbl, float dl, float sl)
{
  env->state_levels[ENVELOPE_STATE_ATTACK]
      = env->state_levels[ENVELOPE_STATE_RELEASED] = bl;
  env->state_levels[ENVELOPE_STATE_DECAYBREAK] = al;
  env->state_levels[ENVELOPE_STATE_DECAY] = dbl;
  env->state_levels[ENVELOPE_STATE_SUSTAIN]
      = env->state_levels[ENVELOPE_STATE_RELEASE] = sl;

  return OK;
}

/****************************************************************************
 * name: fmsnd_set_egperiods
 ****************************************************************************/

int fmsnd_set_egperiods(FAR fmsnd_eg_t *env,
                        int fs,
                        int ap_ms, int dbp_ms, int dp_ms,
                        int sp_ms, int rp_ms)
{
  int i;

  env->state_periods[ENVELOPE_STATE_ATTACK] = fs * ap_ms / 1000;
  env->state_periods[ENVELOPE_STATE_DECAYBREAK] = fs * dbp_ms / 1000;
  env->state_periods[ENVELOPE_STATE_DECAY] = fs * dp_ms / 1000;
  env->state_periods[ENVELOPE_STATE_SUSTAIN] = fs * sp_ms / 1000;
  env->state_periods[ENVELOPE_STATE_RELEASE] = fs * rp_ms / 1000;

  for (i = 0; i < ENVELOPE_STATE_RELEASED; i++)
    {
      env->state_lvl_delta[i] = calc_delta(env->state_levels[i],
                env->state_levels[i+1], env->state_periods[i] );
    }

  return OK;
}

/****************************************************************************
 * name: fmsnd_start_eg
 ****************************************************************************/

int fmsnd_start_eg(FAR fmsnd_eg_t *env)
{
  env->state = ENVELOPE_STATE_ATTACK;
  env->state_counter = 0;
  env->current_level = env->state_levels[ENVELOPE_STATE_ATTACK];
  return OK;
}

/****************************************************************************
 * name: fmsnd_stop_eg
 ****************************************************************************/

int fmsnd_stop_eg(FAR fmsnd_eg_t *env)
{
  env->state = ENVELOPE_STATE_RELEASED;
  env->state_counter = 0;
  return OK;
}

/****************************************************************************
 * name: fmsnd_operate_eg
 ****************************************************************************/

float fmsnd_operate_eg(FAR fmsnd_eg_t *env)
{
  if (env->state != ENVELOPE_STATE_RELEASED)
    {
      if (env->state_counter >= env->state_periods[env->state])
        {
          env->state_counter = 0;
          do
            {
              env->state++;
            }
          while (env->state < ENVELOPE_STATE_RELEASED
               && env->state_periods[env->state] == 0);

          env->current_level = env->state_levels[env->state];
        }
      else
        {
          env->current_level += env->state_lvl_delta[env->state];
          env->state_counter++;
        }
    }

  return env->current_level;
}
