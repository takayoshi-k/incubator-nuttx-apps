/****************************************************************************
 * examples/fmsound/mmlplayer/mmlplayer_score.h
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

#ifndef __EXAMPLES_FMMUSIC_FMMUSIC_SCORE_H__
#define __EXAMPLES_FMMUSIC_FMMUSIC_SCORE_H__

static const char *floh_walzer_right = 
  "T120 L4 O4"
  "R8 {D#C#}8"
  "R8{[<A#>F#][<A#>F#]}{D#C#}8 R8{[<A#>F#][<A#>F#]}{D#C#}8 R8[<A#>F#]8R8[<A#>F#]8 R8{[<B>F][<B>F]}     {D#C#}8"
  "R8{[<B>F][<B>F]}{D#C#}8     R8{[<B>F][<B>F]}{D#C#}8     R8[<B>F]8R8[<B>F]8     R8{[<A#>F#][<A#>F#]} {D#C#}8"
  "R8{[<A#>F#][<A#>F#]}{D#C#}8 R8{[<A#>F#][<A#>F#]}{D#C#}8 R8[<A#>F#]8R8[<A#>F#]8 R8{[<B>F][<B>F]}     {D#C#}8"
  "R8{[<B>F][<B>F]}{D#C#}8     R8{[<B>F][<B>F]}{D#C#}8     R8[<B>F]8R8[<B>F]8     R8{[<A#>F#][<A#>F#]} {D#C#}8"
  "R8[<A#>F#]8R8[<A#>F#]8      R8[<A#>F#]8R8[<A#>F#]8      R2                     R8{[<B>F][<B>F]}     {D#C#}8"
  "R8[<B>F]8R8[<B>F]8          R8[<B>F]8R8[<B>F]8          R2                     R8{[<A#>F#][<A#>F#]} {D#C#}8"
  "R8{[<A#>F#][<A#>F#]}{D#C#}8 R8{[<A#>F#][<A#>F#]}{D#C#}8 R8[<A#>F#]8R8[<A#>F#]8 R8{[<B>F][<B>F]}     {D#C#}8"
  "R8{[<B>F][<B>F]}{D#C#}8     R8{[<B>F][<B>F]}{D#C#}8     R8[<B>F]8R8[<B>F]8     R8{[<A#>F#][<A#>F#]} {D#C#}8"
  "[<A#>F#]8{CC}8{C#C}         R8[<B>F]8[<A#>F#]8R8";

static const char *floh_walzer_left = 
  "T120 L4 O3"
  "R4"
  "F#.R8  F#.R8 F#D#       C#.R8"
  "C#.R8  C#.R8 C#D#       F#.R8"
  ">A#.R8 A#.R8 A#>C#      D#.R8"
  "D#.R8  D#.R8 D#C#       <A#.R8"
  "<F#C#  F#C#  {F#F}{F#G} G#R"
  "G#C#   G#C#  {G#G}{G#A} A#R"
  "F#.R8  F#.R8 F#D#       C#.R8"
  "C#.R8  C#.R8 C#D#       F#.R8"
  "C#8R8R R8C#8<F#8R8";

#endif  /* __EXAMPLES_FMMUSIC_FMMUSIC_SCORE_H__ */
