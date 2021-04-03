#
# makefile for MinGW windows
# How to make
#  use coreutils for windows
#  use 'MinGW-w64 - for 32 and 64 bit Windows'
#  gcc is i686-posix-dwarf version
#

#CC = arm-linux-gnueabi-gcc
CC = g++
#CC2 = gcc
#CXX = g++
#CXXFLAGS = -Lki
#.SUFFIXES: .o .cpp
#.c.obj:
#	$(CC) $(CFLAGS) -c $<
#.cpp.o:
#	$(CC) $(CFLAGS) -c $<



INCDIR = inc
SRCDIR = src
OBJDIR = obj

LUA_DIR=./lua53/src
CY_INC=./cyapi/inc
CY_LIB=./cyapi/lib/x64

#CFLAGS   = -g -O2 -Wall -static -DLUA_COMPAT_5_2 -std=gnu99
#CFLAGS   = -g -O2 -Wall -static -DLUA_COMPAT_5_2
CFLAGS   = -g -O2 -Wall -static


CFLAGS = -O2 -Wall -Werror-implicit-function-declaration -I$(INCDIR)
#LIBS = -lm -lpthread -L=./lua/lib -llua
#LIBS = -lm -lpthread
LIBS = -lm -lpthread -lwsock32 -lws2_32 -llua -lcyapi -lsetupapi
LIBS += -static

CFLAGS  += -I$(LUA_DIR)
CFLAGS  += -L$(LUA_DIR)

CFLAGS  += -I$(CY_INC)
CFLAGS  += -L$(CY_LIB)



EXE1 = main.exe
EXE2 =
EXE3 = 
EXE4 = 
EXE5 = 
EXE6 = 


SRC1 = main.c cusb.c dosmem.cpp dummy.cpp pulbrd.cpp pulgen.cpp strtok_r.cpp \
	if099.c tcp3.c etools.cpp tools.c run_lua.c compu_common.cpp compu_pulser.cpp \
	compu_ad.cpp compu_rf.cpp main_lua.c moddv14.cpp cusb_cyusb3.c

SRCS2 =
SRCS3 =
SRCS4 =
SRCS5 =
SRCS6 =
SRCS7 =

SRC_TMP1 = $(SRC1) $(SRC2) $(SRC3) $(SRC4) $(SRC5) $(SRC6) $(SRC7)
SRC_TMP2:= $(patsubst %.c,   $(SRCDIR)/%.c,  $(SRC_TMP1))
SRCS	:= $(patsubst %.cpp, $(SRCDIR)/%.cpp,$(SRC_TMP2))


#test:
#	@echo ------------SRC_TMP
#	@echo $(SRC_TMP)
#	@echo ------------SRCS
#	@echo $(SRCS)



OBJT1  = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC1))
OBJS1 = $(patsubst %.cpp,$(OBJDIR)/%.o,$(OBJT1))
OBJS2 = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC2))
OBJS3 = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC3))
OBJS4 = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC4))
OBJS5 = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC5))
OBJS6 = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC6))
OBJS7 = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC7))





#test:
#	@echo OBJ = $(OBJS1)
#	@echo OBJ = $(OBJS2)
#	@echo OBJ = $(OBJS3)
#	@echo OBJ = $(OBJS4)
#	@echo OBJ = $(OBJS5)
#	@echo OBJ = $(OBJS6)
#	@echo OBJ = $(OBJS7)

all: $(EXE1) $(EXE2) $(EXE3) $(EXE4) $(EXE5) $(EXE6) $(EXE7)

$(EXE1) : $(OBJS1)
	@echo ------------------------------------------------ for EXE
	@echo $@ :
	$(CC) -o $(EXE1) $(CFLAGS) $(OBJS1) $(LIBS)

$(EXE2) : $(OBJS2)
	@echo ------------------------------------------------ for EXE
	@echo $@ :
	$(CC) -o $(EXE2) $(CFLAGS) $(OBJS2) $(LIBS)
	. ./ftp_up.sh $(FTPHOST) $(EXE2) $(BINPATH)

$(EXE3) : $(OBJS3)
	@echo ------------------------------------------------ for EXE
	@echo $@ :
	$(CC) -o $(EXE3) $(CFLAGS) $(OBJS3) $(LIBS)
	. ./ftp_up.sh $(FTPHOST) $(EXE3) $(CGIPATH)

$(EXE4) : $(OBJS4)
	@echo ------------------------------------------------ for EXE
	@echo $@ :
	$(CC) -o $(EXE4) $(CFLAGS) $(OBJS4) $(LIBS)
	. ./ftp_up.sh $(FTPHOST) $(EXE4) $(BINPATH)

$(EXE5) : $(OBJS5)
	@echo ------------------------------------------------ for EXE
	@echo $@ :
	$(CC) -o $(EXE5) $(CFLAGS) $(OBJS5) $(LIBS)
	. ./ftp_up.sh $(FTPHOST) $(EXE5) $(BINPATH)

$(EXE6) : $(OBJS6)
	@echo ------------------------------------------------ for EXE
	@echo $@ :
	$(CC) -o $(EXE6) $(CFLAGS) $(OBJS6) $(LIBS)

$(EXE7) : $(OBJS7)
	@echo ------------------------------------------------ for EXE
	@echo $@ :
	$(CC) -o $(EXE7) $(CFLAGS) $(OBJS7) $(LIBS)
	. ./ftp_up.sh $(FTPHOST) $(EXE7) $(CGIPATH)

$(OBJDIR)/%.o : $(SRCDIR)/%.c
	@echo ------------------------------------------------ for C
	@echo $@ :
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@echo ------------------------------------------------ for CPP
	@echo $@ :
	$(CC) -c $(CFLAGS) $< -o $@



.PHONY : clean depend

clean:
	rm $(OBJDIR)/*.o

disp:
	@echo $(OBJS1)

depend:
	rm src1.depend
	$(CC) $(CFLAGS) -MM $(SRCS) > src1.depend

-include src1.depend


