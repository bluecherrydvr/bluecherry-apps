CPPFLAGS	= -I. -I../module/include -D_GNU_SOURCE
LDFLAGS		= -lavformat

CFLAGS		= -Wall -Werror -g -O2

TARGET		= libbluecherry.a
OBJS		= bc-core.o bc-utils.o bc-db-core.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcsu $@ $^
clean:
	rm -f $(TARGET) $(OBJS)
