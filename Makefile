CPPFLAGS	= -I. -I../module/include
LDFLAGS		= 

CFLAGS		= -Wall -Werror -g -O0 -pthread

TARGET		= bc-reader
OBJS		= bc-reader.o

all: $(TARGET)

$(TARGET): $(OBJS)

clean:
	rm -f $(TARGET) $(OBJS)
