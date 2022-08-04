#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include <synthesizer/fmsound/fmsound.h>
#include <synthesizer/fmsound/fmsound_operator.h>

void dump_envelope(fmsnd_eg_t *env, const char *indent);
void dump_operator(fmsnd_op_t *op, const char *indent);
void dump_sound(fmsound_t *fmsnd, const char *indent);
void dump_fmout(fmsndout_t *fmout);

#endif  /* __TEST_UTIL_H__ */
