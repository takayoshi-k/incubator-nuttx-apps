/****************************************************************************
 * examples/fmsound/utils/operator_algorithm.c
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

#include <synthesizer/fmsound/fmsound.h>
#include <synthesizer/fmsound/fmsound_operator.h>

/****************************************************************************
 * name: fm_algorithm0
 ****************************************************************************/

void fm_algorithm0(FAR fmsound_t *fmsnd, FAR fmsnd_op_t *carrier, int fs)
{
  /* [Simple sin operator]
   *
   *  +--------------+
   *  | OP (carrier) | ----> Audio out
   *  +--------------+
   */

  fmsnd_set_opfreqrate(carrier, 1.f);

  /* Release level     : 0.0
   * Attack level      : 1.0
   * Decay Break level : 0.3
   * Decay level       : 0.1
   * Sustain level     : 0.1
   */

  fmsnd_set_opeglevels(carrier, 0.f, 1.f, 0.3f, 0.1f, 0.1f);

  /* Attack Time     : 0ms
   * Deay Break Time : 200ms
   * Deay Time       : 100ms
   * Sustain Time    : 0ms
   * Release Time    : 0ms
   */

  fmsnd_set_opegperiods(carrier, fs, 0, 200, 100, 0, 0);
  fmsnd_set_carrier(fmsnd, carrier);
}

/****************************************************************************
 * name: fm_algorithm1
 ****************************************************************************/

void fm_algorithm1(FAR fmsound_t *fmsnd, FAR fmsnd_op_t *carrier,
                                              FAR fmsnd_op_t *op, int fs)
{
  /*
   *  +--------------------+
   *  |                    |
   *  |  +--------------+  |    +--------------+
   *  +->| OP (op)      | -+->  | OP (carrier) | ----> Audio out
   *     +--------------+       +--------------+
   */

  fmsnd_set_opfreqrate(carrier, 1.f);
  fmsnd_set_opeglevels(carrier, 0.f, 1.f, 0.3f, 0.1f, 0.1f);
  fmsnd_set_opegperiods(carrier, fs, 0, 200, 100, 0, 0);

  fmsnd_connect_opfeedback(op, op);
  fmsnd_set_opfreqrate(op, 3.7f);
  fmsnd_set_opeglevels(op, 1.f, 1.f, 1.f, 1.f, 1.f);
  fmsnd_set_opegperiods(op, fs, 0, 0, 0, 0, 0);

  fmsnd_bind_opsuboperator(carrier, op);
  fmsnd_set_carrier(fmsnd, carrier);
}

/****************************************************************************
 * name: fm_algorithm2
 ****************************************************************************/

void fm_algorithm2(FAR fmsound_t *fmsnd, FAR fmsnd_op_t *carrier, int fs)
{
  /*
   *  +--------------------+
   *  |                    |
   *  |  +--------------+  |
   *  +->| OP (carrier) | -+--> Audio out
   *     +--------------+
   */

  fmsnd_connect_opfeedback(carrier, carrier);
  fmsnd_set_opfreqrate(carrier, 1.f);
  fmsnd_set_opeglevels(carrier, 0.f, 1.f, 0.3f, 0.1f, 0.1f);
  fmsnd_set_opegperiods(carrier, fs, 0, 200, 100, 0, 0);
  fmsnd_set_carrier(fmsnd, carrier);
}

/****************************************************************************
 * name: fm_algorithm3
 ****************************************************************************/

void fm_algorithm3(FAR fmsound_t *fmsnd, FAR fmsnd_op_t *carrier,
                                              FAR fmsnd_op_t *op, int fs)
{
  /*
   *  +--------------------+  +-------------------+
   *  |                    |  |                   |
   *  |  +--------------+  |  +>+--------------+  |
   *  +->| OP (op)      | -+->  | OP (carrier) | -+--> Audio out
   *     +--------------+       +--------------+
   */

  fmsnd_connect_opfeedback(carrier, carrier);
  fmsnd_set_opfreqrate(carrier, 1.f);
  fmsnd_set_opeglevels(carrier, 0.f, 1.f, 0.3f, 0.1f, 0.1f);
  fmsnd_set_opegperiods(carrier, fs, 0, 200, 100, 150, 150);

  fmsnd_connect_opfeedback(op, op);
  fmsnd_set_opfreqrate(op, 3.7f);
  fmsnd_set_opeglevels(op, 1.f, 1.f, 1.f, 1.f, 1.f);
  fmsnd_set_opegperiods(op, fs, 0, 0, 0, 0, 0);

  fmsnd_bind_opsuboperator(carrier, op);
  fmsnd_set_carrier(fmsnd, carrier);
}

/****************************************************************************
 * name: fm_algorithm4
 ****************************************************************************/

void fm_algorithm4(FAR fmsound_t *fmsnd, FAR fmsnd_op_t *carrier,
                                              FAR fmsnd_op_t *op, int fs)
{
  /*
   *  +--------------------+  +-------------------+
   *  |                    |  |                   |
   *  |  +--------------+  |  +>+--------------+  |
   *  +->| OP (op)      | -+->  | OP (carrier) | -+--> Audio out
   *     +--------------+       +--------------+
   */

  fmsnd_connect_opfeedback(carrier, carrier);
  fmsnd_set_opfreqrate(carrier, 1.f);
  fmsnd_set_opeglevels(carrier, 0.f, 1.f, 0.3f, 0.2f, 0.1f);
  fmsnd_set_opegperiods(carrier, fs, 0, 500, 100, 150, 300);

  fmsnd_connect_opfeedback(op, op);
  fmsnd_set_opfreqrate(op, 3.7f);
  fmsnd_set_opeglevels(op, 1.f, 1.f, 1.f, 1.f, 1.f);
  fmsnd_set_opegperiods(op, fs, 0, 0, 0, 0, 0);

  fmsnd_bind_opsuboperator(carrier, op);
  fmsnd_set_carrier(fmsnd, carrier);
}
