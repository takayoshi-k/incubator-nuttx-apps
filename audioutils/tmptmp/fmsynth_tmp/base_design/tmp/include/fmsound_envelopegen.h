/****************************************************************************
 * modules/include/synthesizer/fmsound/fmsound_envlopegen.h
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


#ifndef __MODULES_INCLUDE_SYNTHESIZER_FMSOUND_FMSOUND_ENVELOPEGEN_H__
#define __MODULES_INCLUDE_SYNTHESIZER_FMSOUND_FMSOUND_ENVELOPEGEN_H__

/********************************************************************************
 * Include Files
 ********************************************************************************/

#define ENVELOPE_STATE_ATTACK     (0)
#define ENVELOPE_STATE_DECAYBREAK (1)
#define ENVELOPE_STATE_DECAY      (2)
#define ENVELOPE_STATE_SUSTAIN    (3)
#define ENVELOPE_STATE_RELEASE    (4)
#define ENVELOPE_STATE_RELEASED   (5)

/********************************************************************************
 * Public Data Type
 ********************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct fmsound_envelope_s
{
  int state;
  int state_counter;
  float current_level;
  int state_periods[ENVELOPE_STATE_RELEASED];
  float state_levels[ENVELOPE_STATE_RELEASED + 1];
  float state_lvl_delta[ENVELOPE_STATE_RELEASED];
} fmsnd_eg_t;

/********************************************************************************
 * Public Function Prototypes
 ********************************************************************************/

float fmsnd_operate_eg(FAR fmsnd_eg_t *env);

int fmsnd_init_envelope(FAR fmsnd_eg_t *env);

int fmsnd_set_eglevels(FAR fmsnd_eg_t *env,
                        float bl, float al, float dbl, float dl, float sl);

int fmsnd_set_egperiods(FAR fmsnd_eg_t *env,
                         int fs,
                         int at_ms, int dbt_ms, int dp_ms,
                         int sp_ms, int rp_ms);

int fmsnd_start_eg(FAR fmsnd_eg_t *env);
int fmsnd_stop_eg(FAR fmsnd_eg_t *env);

#ifdef __cplusplus
}
#endif

#endif  /* __MODULES_INCLUDE_SYNTHESIZER_FMSOUND_FMSOUND_ENVELOPEGEN_H__ */
