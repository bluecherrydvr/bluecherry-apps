CPPFLAGS	= -I. -I../module/include
LDFLAGS		=-lavformat -lv4l2

CFLAGS		= -Wall -Werror -g -O0 -pthread

TARGET		= bc-reader
OBJS		= bc-reader.o

all: $(TARGET)

$(TARGET): $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS)
