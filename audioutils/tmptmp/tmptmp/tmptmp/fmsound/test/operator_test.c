#include <stdio.h>
#include <synthesizer/fmsound/fmsound.h>
#include <synthesizer/fmsound/fmsound_operator.h>

#include "test_util.h"

#define FS 48000

int main(void)
{
  float val;
  int phase_time;
  struct fmsound_operator_s envop;
  struct fmsound_operator_s fbop;
  struct fmsound_operator_s mainop;
  struct fmsound_operator_s subop;

  struct fmsound_operator_s convine1;
  struct fmsound_operator_s convine2;

  phase_time = 0;

  /* envop */

  fmsnd_init_operator(&envop, 0);
  fmsnd_set_opeglevels(&envop, 0.f, 1.f, 0.3f, 0.5f, 0.5f);
  fmsnd_set_opegperiods(&envop, FS, 0, 30, 10, 100, 30);
  fmsnd_set_fs_op(&envop, FS);
  fmsnd_set_soundfreq_op(&envop, 261.f);
  fmsnd_start_op(&envop);

  /* fbop */

  fmsnd_init_operator(&fbop, 0);
  fmsnd_connect_opfeedback(&fbop, &fbop);  /* self feedback */
  fmsnd_set_fs_op(&fbop, FS);
  fmsnd_set_soundfreq_op(&fbop, 261.f);
  fmsnd_start_op(&fbop);

  /* mainop / subop */

  fmsnd_init_operator(&mainop, 1);
  fmsnd_init_operator(&subop, 0);
  if (fmsnd_bind_opsuboperator(&mainop, &subop) != OK)
    {
      printf("ERROR\n");
      return -1;
    }
  fmsnd_set_opfreqrate(&subop, 2.f);
  fmsnd_set_fs_op(&mainop, FS);
  fmsnd_set_soundfreq_op(&mainop, 261.f);
  fmsnd_start_op(&mainop);

  /* convine1 */

  fmsnd_init_operator(&convine1, 0);
  fmsnd_set_fs_op(&convine1, FS);
  fmsnd_set_soundfreq_op(&convine1, 261.f);
  dump_operator(&convine1, "");
  fmsnd_start_op(&convine1);

  /* convine2 */

  fmsnd_init_operator(&convine2, 0);
  fmsnd_set_opfreqrate(&convine2, 2.f);
  fmsnd_set_fs_op(&convine2, FS);
  fmsnd_set_soundfreq_op(&convine2, 261.f);
  dump_operator(&convine2, "");

  /* Start */

  while (phase_time < FS * (30+10+100+30+1) / 1000)
    {
      fmsnd_update_opfbvalue(&envop);
      val = fmsnd_operate_op(&envop, phase_time, 0);
      printf("%f, ", val);

      fmsnd_update_opfbvalue(&fbop);
      val = fmsnd_operate_op(&fbop, phase_time, 0);
      printf("%f, ", val);

      fmsnd_update_opfbvalue(&mainop);
      val = fmsnd_operate_op(&mainop, phase_time, 0);
      printf("%f, ", val);

      fmsnd_update_opfbvalue(&convine1);
      fmsnd_update_opfbvalue(&convine2);
printf("convine ----- ");
      val = fmsnd_operate_op(&convine1, phase_time, 0);
#if 0
      val = fmsnd_operate_op(&convine1, phase_time, 0)
          + fmsnd_operate_op(&convine2, phase_time, 0);
#endif
dump_envelope(&convine1.envelope, "");
      printf("%f\n", val);

      phase_time++;
    }

  return 0;
}
