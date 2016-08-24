DEPEND_FILE = depend_file

CC = gcc
AR = ar
RANLIB = ranlib

LIB_NAME = lib_HDLC
LIB_FULL_NAME = lib$(LIB_NAME).a

LIB_SRCS = hdlc.c
LIB_OBJS = $(LIB_SRCS:.c=.o)

LIBS = -l$(LIB_NAME)
LIB_DIR = -L./

TARGET_SRCS = run.c
TARGET_OBJS = $(TARGET_SRCS:.c=.o)
TARGET_NAMES = test_SHDLC

.SUFFIXES : .c .o

all : $(LIB_FULL_NAME) $(TARGET_NAMES)

$(LIB_FULL_NAME) : $(LIB_OBJS)
	$(AR) rcv $@ $(LIB_OBJS)
	$(RANLIB) $@

$(TARGET_NAMES): $(TARGET_OBJS)
	$(CC) -o $(TARGET_NAMES) $(TARGET_SRCS) $(LIB_DIR) $(LIBS)

depend:
	$(CC) -MM $(LIB_SRCS) $(TARGET_SRCS) > $(DEPEND_FILE)

clean :
	rm -f $(LIB_FULL_NAME) $(LIB_OBJS) $(TARGET_NAMES) $(TARGET_OBJS) $(DEPEND_FILE)


