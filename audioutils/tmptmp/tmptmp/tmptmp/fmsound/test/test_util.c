#include <stdio.h>

#include <synthesizer/fmsound/fmsound.h>
#include <synthesizer/fmsound/fmsound_operator.h>

#define PRINT_PARAM(p, tp)  printf("    " #p " = " tp "\n", p)

static const char *state(int s)
{
  switch (s)
    {
      case ENVELOPE_STATE_ATTACK:
        return "Attack";
      case ENVELOPE_STATE_DECAYBREAK:
        return "DecayBreak";
      case ENVELOPE_STATE_DECAY:
        return "Decay";
      case ENVELOPE_STATE_SUSTAIN:
        return "Sustain";
      case ENVELOPE_STATE_RELEASE:
        return "Release";
      case ENVELOPE_STATE_RELEASED:
        return "RELEASED...";
    }
  return "";
}

void dump_envelope(fmsnd_eg_t *env, const char *indent)
{
  int i;
  printf("%s", indent); printf("===== STATE : %s =======\n", state(env->state));
  printf("%s", indent); printf("   Current level = %f\n", env->current_level);
  for (i = 0; i < ENVELOPE_STATE_RELEASED; i++)
    {
      printf("%s", indent); printf("   [%d] %f <--------------> [%d] %f\n", i, env->state_levels[i], i+1, env->state_levels[i+1]);
      printf("%s", indent); printf("              per %d\n", env->state_periods[i]);
      printf("%s", indent); printf("              dlt %f\n", env->state_lvl_delta[i]);
    }
  printf("\n");
}

void dump_operator(fmsnd_op_t *op, const char *indent)
{
  int i;
  char spaces[100];
  printf("%s", indent); printf("========= OP ============\n");
  printf("%s", indent); printf("     freq_rate = %f\n", op->freq_rate);
  printf("%s", indent); printf("     last_value = %f\n", op->last_value);
  printf("%s", indent); printf("     fb_value = %f\n", op->fb_value);
  printf("%s", indent); printf("     fb_rate = %f\n", op->fb_rate);
  printf("%s", indent); printf("     maxsubop = %d\n", op->maxsubop);
  printf("%s", indent); printf("     fs = %d\n", op->fs);
  printf("%s", indent); printf("     sound_freq = %f\n", op->sound_freq);
  printf("%s", indent); printf("     delta_phase = %f\n", op->delta_phase);
  printf("%s", indent); printf("     current_phase = %f\n", op->current_phase);
  printf("%s", indent); printf("     ------- envelope ------\n");
  dump_envelope(&op->envelope, "             ");
  printf("%s", indent); printf("     --------- OP sub ------------\n");
  sprintf(spaces, "%s        ", indent);
  for (i = 0; i < op->maxsubop && op->subops[i]; i++)
    {
      dump_operator(op->subops[i], spaces);
    }
}

#if 0
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
#endif

#if 0
typedef struct fmsound_output_s
{
  int fs;
  struct fmsound_s *sounds;
  float output_gain;
} fmsndout_t;
#endif

void dump_sound(fmsound_t *fmsnd, const char *indent)
{
  int i;

  printf("%s", indent); printf("========= fmsound_t : %p\n", fmsnd);
  printf("%s", indent); printf("---------------------------------------\n");
  printf("%s", indent); PRINT_PARAM(fmsnd->fs, "%d");
  printf("%s", indent); PRINT_PARAM(fmsnd->modulation_freq, "%f");
  printf("%s", indent); PRINT_PARAM(fmsnd->carrier_level, "%f");
  printf("%s", indent); PRINT_PARAM(fmsnd->phase_time, "%d");
  printf("%s", indent); PRINT_PARAM(fmsnd->max_carriers, "%d");

  for (i = 0; i < fmsnd->max_carriers && fmsnd->carrier[i]; i++)
    {
      dump_operator(fmsnd->carrier[i], "        ");
    }
}

void dump_fmout(fmsndout_t *fmout)
{
  fmsound_t *sound = fmout->sounds;

  printf("=======================================\n");
  printf("          fmsndout_t : %p\n", fmout);
  printf("---------------------------------------\n");
  PRINT_PARAM(fmout->fs, "%d");
  PRINT_PARAM(fmout->output_gain, "%f");

  while (sound)
    {
      dump_sound(sound, "    ");
      sound = sound->next_sound;
    }
}
