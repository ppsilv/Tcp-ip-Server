#################################################################
#                                                               #
#       MAKEFILE : generic                                      #
#                                                               #
#                                                               #
#       AUTHOR   : Paulo da Silva(Proprietário da Open)         #
#       CREATED  : 23/05/2017                       		    #
#                                                               #
#################################################################
COMPI_DATE= $(shell date +"%Y%m%d")
INSTALL_DIR=$(HOME)/bin
SRCENV=$(SRC_EASY)
APPS=apps

# flags para compilacao
# ---------------------

#INICIO CSU includes and libs ******************************************
EXTERN_LIBS= -L/usr/local/lib
EXTERN_INCS=
APP_INC =-I$(HOME)/$(SRCENV)/include  -I../
APP_INC+=-I$(HOME)/$(SRCENV)/include/core
APP_INC+=-I$(HOME)/$(SRCENV)/$(APPS)/include
#APP_INC+=-DUNIX -DOS_UNIX -DOS_SOLARIS -DNDEBUG -DPKCS11_2_0
APP_INC+=-DOS_SOLARIS -DNDEBUG -DPKCS11_2_0
APP_LIB =-L$(HOME)/lib
APP_LIB_FILES=-lsocket -lnsl  -lsqlite -lshim -ldl -lc
#FIM CSU includes and libs ******************************************

CC=gcc

#DEBUG= -ggdb
INCLUDES=$(APP_INC)
CFLAGS=-Wall $(DEBUG) $(INCDIR) -DCOMPI_DATE=$(COMPI_DATE) $(APP_INC) $(EXTERN_INCS)
LDFLAGS= -L$(LIBHOME) $(APP_LIB) $(EXTERN_LIBS) $(APP_LIB_FILES)
LDNLIBS= $(PROLDLIBS) $(LDLIBS)
PROC= $(ORACLE_HOME)/bin/proc
FLAGS=define=UNIX define=SPARC define=OS_UNIX define=OS_SOLARIS define=NDEBUG 

# Definicao dos sufixos
# ---------------------

.SUFFIXES: .o .c .cpp

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

PROJECT=    makefile
BASEDIR=    ./
INCDIR=     $(INCLUDES)

c =  $(shell ls *.c)
C = 
OUT = sc_svr
OBJ= ${c:.c=.o}

$(OUT):  $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDNLIBS)
	@echo '*********   File '$(OUT)' successfully created.   *********'
	@make clean
	@cp $(OUT) $(INSTALL_DIR)/.
	
all: $(OUT)

cleanall:
	@rm -f ./*.o ./*.lis ./*.bak
	@rm -f ${OUT}

clean:
	@rm -f ./*.o ./*.lis ./*.bak
	
purge:
	@\rm -f ./*.o ./*.lis ./*.bak
	@\rm -f ${OUT}
	@make all
	cp ${OUT} ${INSTALL_DIR}
	@echo "Compiled successful"
	
install:
	@echo "Copying executable to: $(OUT) $(INSTALL_DIR)"
	@cp $(OUT) $(INSTALL_DIR)/.
