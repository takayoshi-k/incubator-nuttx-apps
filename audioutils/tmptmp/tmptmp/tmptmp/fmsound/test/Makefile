BASE_DIR = ..
SRCS = $(BASE_DIR)/fmsound.c $(BASE_DIR)/fmsound_envelopegen.c $(BASE_DIR)/fmsound_operator.c test_util.c

IPATH =  -Idummy_include -I../../../include -I$(BASE_DIR)
DEF =  -DFAR= -DOK=0 -DERROR=-1

CFLAGS = $(IPATH) $(DEF)

all:
	gcc $(CFLAGS) -o envelope_test $(SRCS) envelope_test.c -lm
	gcc $(CFLAGS) -o operator_test $(SRCS) operator_test.c -lm
	gcc $(CFLAGS) -o fmsound_test  $(SRCS) fmsound_test.c -lm

clean:
	rm -f envelope_test operator_test fmsound_test
