CPPFLAGS	= -I. -I../module/include
LDFLAGS		= -lavformat

CFLAGS		= -Wall -Werror -g -O0

TARGET		= bc-reader
OBJS		= bc-reader.o

all: $(TARGET)

$(TARGET): $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS)
