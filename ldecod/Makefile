###
###     Makefile for H.264/AVC decoder
###
###             generated for UNIX/LINUX environments
###             by H. Schwarz, Limin Wang
###



NAME=   ldecod

### include debug information: 1=yes, 0=no
DBG?= 0
### Generate 32 bit executable : 1=yes, 0=no
M32?= 0
### include O level optimization : 0-3
OPT?= 3
### Static Compilation
STC?= 0
### OPENMP support : 1=yes, 0=no
OPENMP?= 0


DEPEND= dependencies

BINDIR= ../bin
INCDIR= inc
SRCDIR= src
OBJDIR= obj

ADDSRCDIR= ../lcommon/src
ADDINCDIR= ../lcommon/inc

ifeq ($(STC),1)
ifeq ($(DBG),1)  ### Do not use static compilation for Debug mode
STC=0
STATIC=
else
STATIC= -static
endif
else
STATIC= 
endif

# -----
_OS_TYPE_RAW := Unknown
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    _OS_TYPE_RAW := Linux
else ifeq ($(UNAME_S),Darwin) # macOS
    _OS_TYPE_RAW := Darwin
else ifneq (,$(findstring CYGWIN,$(UNAME_S)))
    _OS_TYPE_RAW := Windows
else ifneq (,$(findstring MINGW,$(UNAME_S)))
    _OS_TYPE_RAW := Windows
else ifneq (,$(findstring MSYS,$(UNAME_S)))
    _OS_TYPE_RAW := Windows
else ifeq ($(OS),Windows_NT) # Fallback for environment variable
    _OS_TYPE_RAW := Windows
endif
OS_TYPE := $(strip $(_OS_TYPE_RAW))


ifeq ($(OS_TYPE),Windows)
    WINLIBS = -lws2_32
else
    WINLIBS =
endif
LIBS = -lm $(WINLIBS) $(STATIC)

CFLAGS+=  -std=gnu99 -pedantic -ffloat-store -fno-strict-aliasing -fsigned-char $(STATIC)
FLAGS=  $(CFLAGS) -Wall -I$(INCDIR) -I$(ADDINCDIR) -D__USE_LARGEFILE64 -D_FILE_OFFSET_BITS=64

ifeq ($(M32),1)
FLAGS+=-m32
endif

ifeq ($(OPENMP),1)
  FLAGS+=-fopenmp
endif

OPT_FLAG = -O$(OPT)
ifeq ($(DBG),1)
SUFFIX= .dbg
FLAGS+= -g
else
SUFFIX=
FLAGS+= $(OPT_FLAG)
endif

OBJSUF= .o$(SUFFIX)

SRC=    $(wildcard $(SRCDIR)/*.c) 
ADDSRC= $(wildcard $(ADDSRCDIR)/*.c)
OBJ=    $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o$(SUFFIX)) $(ADDSRC:$(ADDSRCDIR)/%.c=$(OBJDIR)/%.o$(SUFFIX)) 
# Adjust binary name based on OS
ifeq ($(OS_TYPE),Windows)
    BIN_EXT = .exe
else
    BIN_EXT =
endif
BIN=    $(BINDIR)/$(NAME)$(SUFFIX)$(BIN_EXT)


.PHONY: default distclean clean tags depend

default: messages objdir_mk depend bin 

messages:
	@echo 'OS Type: $(OS_TYPE)'
	@echo 'os = $(OS)'
	@echo 'Compiling with libraries: $(LIBS)'
ifeq ($(M32),1)
	@echo 'Compiling with M32 support...'
endif
ifeq ($(DBG),1)
	@echo 'Compiling with Debug support...'
	@echo 'Note static compilation not supported in this mode.'
endif
ifeq ($(STC),1)
	@echo 'Compiling with -static support...'
endif
ifeq ($(OPENMP),1)
	@echo 'Compiling with -fopenmp support...'
endif

clean:
	@echo remove all objects
	@rm -rf $(OBJDIR)

distclean: clean
	@rm -f $(DEPEND) tags
	@rm -f $(BIN)

tags:
	@echo update tag table
	@ctags inc/*.h src/*.c

bin:    $(OBJ)
	@echo
	@echo 'creating binary "$(BIN)"'
	$(CC) $(FLAGS) -o $(BIN) $(OBJ) $(LIBS)
	@echo '... done'
	@echo

depend:
	@echo
	@echo 'checking dependencies'
	@$(SHELL) -ec '$(CC) $(FLAGS) -MM $(CFLAGS) -I$(INCDIR) -I$(ADDINCDIR) $(SRC) $(ADDSRC)                   \
         | sed '\''s@\(.*\)\.o[ :]@$(OBJDIR)/\1.o$(SUFFIX):@g'\''               \
         >$(DEPEND)'
	@echo

$(OBJDIR)/%.o$(SUFFIX): $(SRCDIR)/%.c
	@echo 'compiling object file "$@" ...'
	@$(CC) -c -o $@ $(FLAGS) $<

$(OBJDIR)/%.o$(SUFFIX): $(ADDSRCDIR)/%.c
	@echo 'compiling object file "$@" ...'
	@$(CC) -c -o $@ $(FLAGS) $<

objdir_mk:
	@echo 'Creating $(OBJDIR) ...'
	@mkdir -p $(OBJDIR)

-include $(DEPEND)
