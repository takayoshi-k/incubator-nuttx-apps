/****************************************************************************
 * modules/include/synthesizer/fmsound/fmsound.h
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

#ifndef __MODULES_INCLUDE_SYNTHESIZER_FNSOUND_FMSOUND_H__
#define __MODULES_INCLUDE_SYNTHESIZER_FNSOUND_FMSOUND_H__

/********************************************************************************
 * Include Files
 ********************************************************************************/

#include <stdint.h>

#include "synthesizer/fmsound/fmsound_operator.h"

/**
 * @file fmsound.h
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

/**
 * @struct fmsound_s
 * @brief FM Modulator instance
 */
typedef struct fmsound_s
{
  float carrier_level;

  int phase_time;

  int max_carriers;
  struct fmsound_operator_s **carrier;

  int fs;
  float modulation_freq;

  struct fmsound_s *next_sound;
} fmsound_t;

typedef struct fmsound_renderer_s
{
  int fs;
  struct fmsound_s *sounds;
  float output_gain;
} fmsndrender_t;

typedef void (*fmsnd_tickcb_t)(unsigned long arg, FAR fmsndrender_t *fmrndr);

/********************************************************************************
 * Public Function Prototypes
 ********************************************************************************/

int fmsnd_initialize(FAR fmsound_t *fmsnd, int carriernum);
int fmsnd_set_carrier(FAR fmsound_t *fmsnd,
                        FAR struct fmsound_operator_s *op);
void fmsnd_set_soundfreq(FAR fmsound_t *fmsnd, float f);
void fmsnd_set_fs(FAR fmsound_t *fmsnd, int fs);

static inline void fmsnd_reset_phasetime(FAR fmsound_t *fmsnd)
{
  fmsnd->phase_time = 0;
}

static inline void fmsnd_set_carrierlevel(FAR fmsound_t *fmsnd, float cl)
{
  fmsnd->carrier_level = cl;
}

/***************************************/

void fmrenderer_initialize(FAR fmsndrender_t *fmrndr);
void fmrenderer_finalize(FAR fmsndrender_t *fmrndr);
static inline void fmrenderer_outputgain(FAR fmsndrender_t *fmrndr, int gain)
{
  fmrndr->output_gain = (float)gain;
}
void fmrenderer_attach_sound(FAR fmsndrender_t *fmrndr, FAR fmsound_t *fmsnd);
int fmrenderer_rendering_16(FAR fmsndrender_t *fmrndr,
                      FAR int16_t *sample, int sample_num, int chnum,
                      fmsnd_tickcb_t cb, unsigned long cbarg);
void fmrenderer_setfs(FAR fmsndrender_t *fmrndr, int fs);

/** @} fmsound */

#ifdef __cplusplus
}
#endif

#endif  /* __SDK_MODULES_SYNTHESIZER_FNSOUND_FMSOUND_H__ */
