DEPEND_FILE = depend_file # dependency file name

CC = gcc 

TARGET_SRCS = run.c hdlc.c
TARGET_OBJS = $(TARGET_SRCS:%.c=%.o)
TARGET_NAMES = hdlc_run	

.SUFFIXES : .c .o

all : $(TARGET_NAMES)

$(TARGET_NAMES): $(TARGET_OBJS)     
	$(CC) -o $(TARGET_NAMES) $(TARGET_OBJS)
depend:
	$(CC) -MM $(TARGET_SRCS) > $(DEPEND_FILE)

clean :
	rm -f $(TARGET_NAMES) $(TARGET_OBJS) $(DEPEND_FILE)


