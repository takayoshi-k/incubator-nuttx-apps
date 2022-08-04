#include <stdio.h>

#include <synthesizer/fmsound/fmsound_envelopegen.h>

#define FS 48000

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

static void dump_envelope(fmsnd_eg_t *env)
{
  int i;
  printf("===== STATE : %s =======\n", state(env->state));
  printf("   Current level = %f\n", env->current_level);
  for (i = 0; i < ENVELOPE_STATE_RELEASED; i++)
    {
      printf("   [%d] %f <--------------> [%d] %f\n", i, env->state_levels[i], i+1, env->state_levels[i+1]);
      printf("              per %d\n", env->state_periods[i]);
      printf("              dlt %f\n", env->state_lvl_delta[i]);
    }
  printf("\n");
}

int main(void)
{
  struct fmsound_envelope_s envelope;

  fmsnd_init_envelope(&envelope);
  fmsnd_set_eglevels(&envelope,
                      0.f, 1.f, 0.3f, 0.5f, 0.5f);
  fmsnd_set_egperiods(&envelope, FS,
                      // 10, 5, 3, 50, 10);
                      10, 5, 3, 0, 10);

  dump_envelope(&envelope);

  fmsnd_start_eg(&envelope);
  dump_envelope(&envelope);
  while (envelope.state != ENVELOPE_STATE_RELEASED)
    {
      // dump_envelope(&envelope);
      printf("state:%s(%d) : %f\n", state(envelope.state), envelope.state_counter, fmsnd_operate_eg(&envelope));
      // printf("%f\n", envelope_operate(&fmsnd, &envelope));
    }

#if 0
  printf("Again======\n");

  start_envelope(&envelope);
  while (envelope.state != ENVELOPE_STATE_RELEASED)
    {
      printf("state:%s(%d) : %f\n", state(envelope.state), envelope.state_counter, envelope_operate(&fmsnd, &envelope));
    }
#endif

  return 0;
}
