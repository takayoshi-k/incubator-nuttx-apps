#include <stdint.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>

#include <synthesizer/fmsound/fmsound.h>
#include <synthesizer/fmsound/fmsound_operator.h>

#include "test_util.h"

#define APP_FS    (48000)
#define APP_BPS   (16)
#define APP_CHNUM (2)

#define APP_SAMPLE_SIZE (4096)

static fmsndout_t g_fmout;
static fmsound_t g_fmsnd;
static fmsnd_op_t g_op[2];

static int16_t sample[APP_SAMPLE_SIZE];

static void dump_buffer(void)
{
  int i;
  for (i=0; i<4096; i+=2)
    {
      printf("%d, %d\n", sample[i], sample[i+1]);
    }
}

static void setup_nonblock(struct termios *save_setting)
{
	struct termios settings;
	char c;

	tcgetattr(0,save_setting);
	settings = *save_setting;

	settings.c_lflag &= ~(ECHO|ICANON);
	settings.c_cc[VTIME] = 0;
	settings.c_cc[VMIN] = 1;
	tcsetattr(0,TCSANOW,&settings);
	fcntl(0,F_SETFL,O_NONBLOCK);	
}

int main(void)
{
  char key;
	struct termios save_setting;

  fmout_initialize_output(&g_fmout);
  fmout_set_fs(&g_fmout, APP_FS);

  fmsnd_initialize(&g_fmsnd, 1);
  fmsnd_init_operator(&g_op[0], 1);
  fmsnd_init_operator(&g_op[1], 0);

  fmsnd_bind_opsuboperator(&g_op[0], &g_op[1]);
  fmsnd_set_carrier(&g_fmsnd, &g_op[0]);
  fmout_attach_sound(&g_fmout, &g_fmsnd);

    fmsnd_set_opfreqrate(&g_op[0], 1.f);
    fmsnd_set_opeglevels(&g_op[0], 0.f, 1.f, 0.3f, 0.5f, 0.5f);
    fmsnd_set_opegperiods(&g_op[0], APP_FS, 0, 200, 50, 0, 10);
    fmsnd_start_op(&g_op[0]);

    fmsnd_set_opfreqrate(&g_op[1], 2.f);
    fmsnd_set_opeglevels(&g_op[1], 0.f, 1.f, 1.f, 1.f, 1.f);
    fmsnd_set_opegperiods(&g_op[1], APP_FS, 0, 50, 0, 0, 10);
    fmsnd_start_op(&g_op[1]);

  fmsnd_set_soundfreq(&g_fmsnd, 261.f);


  if (g_fmout.sounds != &g_fmsnd)
    {
      printf("Error!!!\n");
      return -1;
    }

  setup_nonblock(&save_setting);

  dump_fmout(&g_fmout);

#if 1 
  while (1)
    {
      key = (char)getchar();

      if (key != EOF)
        {
          printf("Key Down:%c\n", key);
          if (key == 'q')
            {
              break;
            }
          fmsnd_set_soundfreq(&g_fmsnd, 261.f);
        }

      // printf("Start modulation --------------------\n");
      fmout_modulate_16(&g_fmout, sample, APP_SAMPLE_SIZE, APP_CHNUM, NULL, 0);
      dump_buffer();
    }
#endif

  tcsetattr(0,TCSANOW,&save_setting);

  return 0;
}
