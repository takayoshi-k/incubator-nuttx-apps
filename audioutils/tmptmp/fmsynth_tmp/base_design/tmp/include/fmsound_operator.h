/****************************************************************************
 * modules/include/synthesizer/fmsound_operator.h
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
 * 3. Neither the name of Sony Semiconductor Solutions Corporation nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
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


#ifndef __MODULES_INCLUDE_SYNTHESIZER_FMSOUND_OPERATOR_H__
#define __MODULES_INCLUDE_SYNTHESIZER_FMSOUND_OPERATOR_H__

/********************************************************************************
 * Include Files
 ********************************************************************************/

#include "synthesizer/fmsound/fmsound_envelopegen.h"

/**
 * @file fmsound_operator.h
 */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup fmsound FM Modulation
 * @{
 *
 * FM Modulation Synthesizer
 */

/********************************************************************************
 * Public Data Type
 ********************************************************************************/

typedef struct fmsound_operator_s
{
  fmsnd_eg_t envelope;
  float freq_rate;
  float last_value;
  float *fb_ref;
  float fb_value;
  float fb_rate;
  int maxsubop;
  struct fmsound_operator_s **subops;

  int fs;
  float sound_freq;
  float delta_phase;
  float current_phase;
} fmsnd_op_t;

/********************************************************************************
 * Public Function Prototypes
 ********************************************************************************/

int fmsnd_init_operator(FAR fmsnd_op_t *op,
                    int maxsubops);
int fmsnd_fin_operator(FAR fmsnd_op_t *op);

int fmsnd_set_opfreqrate(FAR fmsnd_op_t *op, float rate);
int fmsnd_set_opfeedbackrate(FAR fmsnd_op_t *op, float rate);

int fmsnd_set_opeglevels(FAR fmsnd_op_t *op,
                          float bl, float al, float dbl, float dl, float sl);

int fmsnd_set_opegperiods(FAR fmsnd_op_t *op,
                          int fs,
                          int at_ms, int dbt_ms, int dp_ms,
                          int sp_ms, int rp_ms);

void fmsnd_set_opfs(FAR fmsnd_op_t *op, int fs);
void fmsnd_set_opsoundfreq(FAR fmsnd_op_t *op, float freq);

int fmsnd_connect_opfeedback(FAR fmsnd_op_t *op, FAR fmsnd_op_t *fbop);
int fmsnd_disconnect_opfeedback(FAR fmsnd_op_t *op);

int fmsnd_bind_opsuboperator(FAR fmsnd_op_t *op,
                             FAR fmsnd_op_t *subop);
int fmsnd_unbind_opsuboperators(FAR fmsnd_op_t *op);

int fmsnd_start_op(FAR fmsnd_op_t *op);

int fmsnd_latch_opfbvalue(FAR fmsnd_op_t *op);

float fmsnd_operate_op(FAR fmsnd_op_t *op, int phase_time, int depth);

/** @} fmsound */

#ifdef __cplusplus
}
#endif

#endif  /* __MODULES_INCLUDE_SYNTHESIZER_FMSOUND_OPERATOR_H__ */
