# Makefile for JustForFun Files

# A few variables
include ../Make.defines

LIBS+=-lncurses

SRC_DIR=.
EXE_DIR=./demo

EXES = \
    ${EXE_DIR}/hello_world \
	${EXE_DIR}/init_func_example \
	${EXE_DIR}/printw_example \
	${EXE_DIR}/scanw_example \
	${EXE_DIR}/simple_attr \
	${EXE_DIR}/with_chgat \
	${EXE_DIR}/win_border \
	${EXE_DIR}/other_border \
	${EXE_DIR}/simple_color \
	${EXE_DIR}/simple_key \
#	${EXE_DIR}/key_code \
	${EXE_DIR}/mouse_menu \
	${EXE_DIR}/temp_leave \

${EXE_DIR}/%: %.o
	${CC} -o $@ $< ${LIBS}

%.o: ${SRC_DIR}/%.c
	${CC} -o $@ -c $< $(CFLAGS) $(F_DEBUG) 

all: ${EXES}

clean:
	$(RM) ${EXES}
