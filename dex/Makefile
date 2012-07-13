C_FILES = DexParser.c main.c utils.c
TARGET = DexPrinter

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
