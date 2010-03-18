CPPFLAGS	= -I. -I../module/include
LDFLAGS		= -lavformat

CFLAGS		= -Wall -Werror -g -O2

TARGET		= libbluecherry.a
OBJS		= bc-core.o bc-utils.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcsu $@ $^
clean:
	rm -f $(TARGET) $(OBJS)
