C_FILES = AxmlParser.c main.c
TARGET = AxmlPrinter

CC = gcc
CFLAGS = -Wall
OBJPATH = obj

VERBOSE ?= @

OBJS = $(addprefix $(OBJPATH)/, $(addsuffix .o, $(basename $(C_FILES))))

all: $(TARGET)

$(OBJPATH)/%.o: %.c
	$(VERBOSE) mkdir -p obj
	$(VERBOSE) $(CC) $(CFLAGS) -o $@ -c $<

$(TARGET):$(OBJS)
	$(VERBOSE) $(CC) $(CFLAGS) -o $@ $^

clean:
	$(VERBOSE) rm -rf $(OBJPATH)/*.o
	$(VERBOSE) rm -f $(TARGET)
	$(VERBOSE) rm -f *~
