CC = gcc
CFLAGS = -O3 -Wall -Wextra -I./include
SRCDIR = src
OBJDIR = obj
BINDIR = bin

ifdef OS
    # Windows
    RM = del /Q
    RMDIR = rmdir /Q /S
    EXE = .exe
    FIXPATH = $(subst /,\,$1)
else
    # Unix-like
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
    EXE =
    FIXPATH = $1
endif

SOURCES = $(wildcard $(SRCDIR)/*.c) main.c
OBJECTS = $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(SOURCES)))
EXECUTABLE = $(BINDIR)/inmemorydb_test$(EXE)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
ifdef OS
	@if not exist $(call FIXPATH,$(BINDIR)) mkdir $(call FIXPATH,$(BINDIR))
else
	@$(MKDIR) $(BINDIR)
endif
	$(CC) $(OBJECTS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
ifdef OS
	@if not exist $(call FIXPATH,$(OBJDIR)) mkdir $(call FIXPATH,$(OBJDIR))
else
	@$(MKDIR) $(OBJDIR)
endif
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/main.o: main.c
ifdef OS
	@if not exist $(call FIXPATH,$(OBJDIR)) mkdir $(call FIXPATH,$(OBJDIR))
else
	@$(MKDIR) $(OBJDIR)
endif
	$(CC) $(CFLAGS) -c $< -o $@

clean:
ifdef OS
	@if exist $(call FIXPATH,$(OBJDIR)) $(RMDIR) $(call FIXPATH,$(OBJDIR))
	@if exist $(call FIXPATH,$(BINDIR)) $(RMDIR) $(call FIXPATH,$(BINDIR))
else
	@$(RMDIR) $(OBJDIR) $(BINDIR)
endif

.PHONY: all clean