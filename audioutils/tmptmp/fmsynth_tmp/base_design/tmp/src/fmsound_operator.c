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

#include <stdlib.h>
#include <sys/types.h>
#include <synthesizer/fmsound/fmsound_operator.h>
#include <math_lib.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * name: update_parameters
 ****************************************************************************/

static void update_parameters(FAR fmsnd_op_t *op)
{
  if (op->fs != 0)
    {
      op->delta_phase = 2 * M_PI * op->sound_freq * op->freq_rate
                        / (float)op->fs;
    }
  else
    {
      op->delta_phase = 0.f;
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * name: fmsnd_init_operator
 ****************************************************************************/

int fmsnd_init_operator(FAR fmsnd_op_t *op,
                    int maxsubops)
{
  int ret;

  op->last_value = 0.f;
  op->freq_rate = 1.f;
  op->fb_ref = NULL;
  op->fb_value = 0.f;
  op->fb_rate = 1.f;
  op->subops = NULL;
  op->maxsubop = maxsubops;
  ret = fmsnd_init_envelope(&op->envelope);

  op->fs = 0;
  op->sound_freq = 0.f;
  op->delta_phase = 0.f;
  op->current_phase = 0.f;

  if (maxsubops > 0)
    {
      op->subops = (fmsnd_op_t **)
                  calloc(maxsubops, sizeof(fmsnd_op_t *));
      if (!op->subops)
        {
          ret = ERROR;
        }
    }

  return ret;
}

/****************************************************************************
 * name: fmsnd_fin_operator
 ****************************************************************************/

int fmsnd_fin_operator(FAR fmsnd_op_t *op)
{
  int i;
  if (op->subops)
    {
      for (i = 0; i < op->maxsubop && op->subops[i]; i++)
        {
          fmsnd_fin_operator(op->subops[i]);
          op->subops[i] = NULL;
        }
      free(op->subops);
    }
  return OK;
}

/****************************************************************************
 * name: fmsnd_set_opfreqrate
 ****************************************************************************/

int fmsnd_set_opfreqrate(FAR fmsnd_op_t *op, float rate)
{
  int i;
  op->freq_rate = rate;
  update_parameters(op);

  for (i = 0; i < op->maxsubop && op->subops[i]; i++)
    {
      fmsnd_set_opfreqrate(op->subops[i], rate);
    }

  return OK;
}

/****************************************************************************
 * name: fmsnd_set_opfeedbackrate
 ****************************************************************************/

int fmsnd_set_opfeedbackrate(FAR fmsnd_op_t *op, float rate)
{
  op->fb_rate = rate;
  return OK;
}

/****************************************************************************
 * name: fmsnd_set_opeglevels
 ****************************************************************************/

int fmsnd_set_opeglevels(FAR fmsnd_op_t *op,
                          float bl, float al, float dbl, float dl, float sl)
{
  return fmsnd_set_eglevels(&op->envelope, bl, al, dbl, dl, sl);
}

/****************************************************************************
 * name: fmsnd_set_opegperiods
 ****************************************************************************/

int fmsnd_set_opegperiods(FAR fmsnd_op_t *op,
                           int fs,
                           int at_ms, int dbt_ms, int dp_ms,
                           int sp_ms, int rp_ms)
{
  return fmsnd_set_egperiods(&op->envelope, fs,
                              at_ms, dbt_ms, dp_ms, sp_ms, rp_ms);
}

/****************************************************************************
 * name: fmsnd_connect_opfeedback
 ****************************************************************************/

int fmsnd_connect_opfeedback(FAR fmsnd_op_t *op,
                             FAR fmsnd_op_t *fbop)
{
  int ret = ERROR;

  if (fbop)
    {
      op->fb_ref = &fbop->last_value;
      ret = OK;
    }

  return ret;
}

/****************************************************************************
 * name: fmsnd_disconnect_opfeedback
 ****************************************************************************/

int fmsnd_disconnect_opfeedback(FAR fmsnd_op_t *op)
{
  op->fb_ref = NULL;
  op->fb_value = 0.f;
  return OK;
}

/****************************************************************************
 * name: fmsnd_bind_opsuboperator
 ****************************************************************************/

int fmsnd_bind_opsuboperator(FAR fmsnd_op_t *op,
                             FAR fmsnd_op_t *subop)
{
  int i;
  int ret = ERROR;

  for (i = 0; i < op->maxsubop; i++)
    {
      if (op->subops[i] == NULL)
        {
          op->subops[i] = subop;
          ret = OK;
          break;
        }
    }

  return ret;
}

/****************************************************************************
 * name: fmsnd_unbind_opsuboperators
 ****************************************************************************/

int fmsnd_unbind_opsuboperators(FAR fmsnd_op_t *op)
{
  int i;

  for (i = 0; i < op->maxsubop; i++)
    {
      op->subops[i] = NULL;
    }

  return OK;
}

/****************************************************************************
 * name: fmsnd_start_op
 ****************************************************************************/

int fmsnd_start_op(FAR fmsnd_op_t *op)
{
  int i;
  for (i = 0; i < op->maxsubop && op->subops[i]; i++)
    {
      fmsnd_start_op(op->subops[i]);
    }

  return fmsnd_start_eg(&op->envelope);
}

/****************************************************************************
 * name: fmsnd_latch_opfbvalue
 ****************************************************************************/

int fmsnd_latch_opfbvalue(FAR fmsnd_op_t *op)
{
  int i;
  for (i = 0; i < op->maxsubop && op->subops[i]; i++)
    {
      fmsnd_latch_opfbvalue(op->subops[i]);
    }

  if (op->fb_ref)
    {
      op->fb_value = *op->fb_ref;
    }
  return OK;
}

/****************************************************************************
 * name: fmsnd_set_opfs
 ****************************************************************************/

void fmsnd_set_opfs(FAR fmsnd_op_t *op, int fs)
{
  int i;
  op->fs = fs;
  update_parameters(op);

  for (i = 0; i < op->maxsubop && op->subops[i]; i++)
    {
      fmsnd_set_opfs(op->subops[i], fs);
    }
}

/****************************************************************************
 * name: fmsnd_set_opsoundfreq
 ****************************************************************************/

void fmsnd_set_opsoundfreq(FAR fmsnd_op_t *op, float freq)
{
  int i;
  op->sound_freq = freq;
  update_parameters(op);

  for (i = 0; i < op->maxsubop && op->subops[i]; i++)
    {
      fmsnd_set_opsoundfreq(op->subops[i], freq);
    }
}

/****************************************************************************
 * name: fmsnd_operate_op
 ****************************************************************************/

float fmsnd_operate_op(FAR fmsnd_op_t *op, int phase_time, int depth)
{
  int i;
  float phase;

  if (phase_time == 0)
    {
      op->current_phase = 0.f;
    }
  else
    {
      /* delta_phase  = 2 * M_PI * (sound_freq) * op->freq_rate / (float)fs */

      op->current_phase += op->delta_phase;
    }
  phase = op->current_phase + op->fb_value * op->fb_rate;

  for (i = 0; i < op->maxsubop && op->subops[i]; i++)
    {
      phase += fmsnd_operate_op(op->subops[i], phase_time, depth + 1);
    }

  op->last_value = fmsnd_operate_eg(&op->envelope);
  op->last_value *= SIN_FUNC(phase);

  return op->last_value;
}
