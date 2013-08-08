CC=		g++
GCC=		gcc
 
BINDIR=		../bin
LIBDIR=		../lib
INSTALLDIR=	/usr/bin

UTILS=		utils
CRF=		crfs
EVAL=		crfs/evaluation
TRAINER=	crfs/trainer
FEATURE=	crfs/feature
VITERBI=	crfs/viterbi
MATH=		crfs/math
MISC=		crfs/misc
DATAITF=	dataitf

FEASELDIR=		feasel
CHUNKINGFEASELDIR=	$(FEASELDIR)/Chunking
CHUNKINGFEASEL=		chunkingfeasel
POSFEASELDIR=		$(FEASELDIR)/POSTagging
POSFEASEL=		postaggingfeasel
CONVERTCHK=		convertchk
EVALUATECHK=		evaluatechk
VOTING=			majorityvote

OBJS=		$(UTILS)/strtokenizer.o $(TRAINER)/lbfgs.o $(DATAITF)/data.o \
		$(FEATURE)/feature.o $(DATAITF)/dictionary.o $(FEATURE)/featuregen.o \
		$(MISC)/option.o $(TRAINER)/trainer.o $(VITERBI)/viterbi.o \
		$(CRF)/model.o $(MATH)/doublevector.o $(MATH)/doublematrix.o \
		$(MATH)/mathlib.o $(EVAL)/evaluation.o $(EVAL)/convertchunk.o \
		$(EVAL)/evaluatechunk.o

MAIN=		crf
LIB=		flexcrfs

# type "make all" to compile the project
all:	$(OBJS)  $(MAIN).cpp 
	$(CC) -o $(MAIN) $(MAIN).cpp $(OBJS) 
	strip $(MAIN)
	cp $(MAIN) $(BINDIR)
	ar rvu $(LIB).a $(OBJS)
	strip $(LIB).a
	cp $(LIB).a $(LIBDIR)
	rm $(LIB).a
	$(CC) -o $(CHUNKINGFEASELDIR)/$(CHUNKINGFEASEL) $(CHUNKINGFEASELDIR)/$(CHUNKINGFEASEL).cpp $(UTILS)/strtokenizer.o
	strip $(CHUNKINGFEASELDIR)/$(CHUNKINGFEASEL)
	cp $(CHUNKINGFEASELDIR)/$(CHUNKINGFEASEL) $(BINDIR)
	$(CC) -o $(POSFEASELDIR)/$(POSFEASEL) $(POSFEASELDIR)/$(POSFEASEL).cpp $(UTILS)/strtokenizer.o
	strip $(POSFEASELDIR)/$(POSFEASEL)
	cp $(POSFEASELDIR)/$(POSFEASEL) $(BINDIR)
	$(CC) -o $(CONVERTCHK) $(CONVERTCHK).cpp $(EVAL)/convertchunk.o $(UTILS)/strtokenizer.o
	strip $(CONVERTCHK)
	cp $(CONVERTCHK) $(BINDIR)
	$(CC) -o $(EVALUATECHK) $(EVALUATECHK).cpp $(EVAL)/evaluatechunk.o $(UTILS)/strtokenizer.o
	strip $(EVALUATECHK)
	cp $(EVALUATECHK) $(BINDIR)
	$(CC) -o $(VOTING) $(VOTING).cpp $(UTILS)/strtokenizer.cpp
	strip $(VOTING)
	cp $(VOTING) $(BINDIR)

$(UTILS)/strtokenizer.o:	$(UTILS)/strtokenizer.cpp
	$(CC) -c -o $(UTILS)/strtokenizer.o $(UTILS)/strtokenizer.cpp

$(TRAINER)/lbfgs.o:	$(TRAINER)/lbfgs.c
	$(GCC) -DHAVE_CONFIG_H -I. -I. -I. -O3 -Wall -c $(TRAINER)/lbfgs.c -fPIC -DPIC -o $(TRAINER)/lbfgs.o

$(DATAITF)/data.o:	$(DATAITF)/data.cpp
	$(CC) -c -o $(DATAITF)/data.o $(DATAITF)/data.cpp

$(FEATURE)/feature.o:	$(FEATURE)/feature.cpp
	$(CC) -c -o $(FEATURE)/feature.o $(FEATURE)/feature.cpp

$(DATAITF)/dictionary.o:	$(DATAITF)/dictionary.cpp
	$(CC) -c -o $(DATAITF)/dictionary.o $(DATAITF)/dictionary.cpp

$(FEATURE)/featuregen.o:	$(FEATURE)/featuregen.cpp
	$(CC) -c -o $(FEATURE)/featuregen.o $(FEATURE)/featuregen.cpp

$(MISC)/option.o:	$(MISC)/option.cpp
	$(CC) -c -o $(MISC)/option.o $(MISC)/option.cpp

$(TRAINER)/trainer.o:	$(TRAINER)/trainer.cpp
	$(CC) -c -o $(TRAINER)/trainer.o $(TRAINER)/trainer.cpp

$(VITERBI)/viterbi.o:	$(VITERBI)/viterbi.cpp
	$(CC) -c -o $(VITERBI)/viterbi.o $(VITERBI)/viterbi.cpp

$(CRF)/model.o:	$(CRF)/model.cpp
	$(CC) -c -o $(CRF)/model.o $(CRF)/model.cpp

$(MATH)/doublevector.o:	$(MATH)/doublevector.cpp
	$(CC) -c -o $(MATH)/doublevector.o $(MATH)/doublevector.cpp

$(MATH)/doublematrix.o:	$(MATH)/doublematrix.cpp
	$(CC) -c -o $(MATH)/doublematrix.o $(MATH)/doublematrix.cpp

$(MATH)/mathlib.o:	$(MATH)/mathlib.cpp
	$(CC) -c -o $(MATH)/mathlib.o $(MATH)/mathlib.cpp

$(EVAL)/evaluation.o:	$(EVAL)/evaluation.cpp
	$(CC) -c -o $(EVAL)/evaluation.o $(EVAL)/evaluation.cpp

$(EVAL)/convertchunk.o:	$(EVAL)/convertchunk.cpp
	$(CC) -c -o $(EVAL)/convertchunk.o $(EVAL)/convertchunk.cpp

$(EVAL)/evaluatechunk.o:	$(EVAL)/evaluatechunk.cpp
	$(CC) -c -o $(EVAL)/evaluatechunk.o $(EVAL)/evaluatechunk.cpp

# type "make clean" to clean all files generated during the compiling process
clean:
	rm $(OBJS) 
	rm $(LIBDIR)/$(LIB).a
	rm $(MAIN) $(BINDIR)/$(MAIN)	
	rm $(CHUNKINGFEASELDIR)/$(CHUNKINGFEASEL) $(BINDIR)/$(CHUNKINGFEASEL)
	rm $(POSFEASELDIR)/$(POSFEASEL) $(BINDIR)/$(POSFEASEL)
	rm $(CONVERTCHK) $(BINDIR)/$(CONVERTCHK)
	rm $(EVALUATECHK) $(BINDIR)/$(EVALUATECHK)
	rm $(VOTING) $(BINDIR)/$(VOTING)

install:
	cp $(BINDIR)/$(MAIN) $(INSTALLDIR)
	cp $(BINDIR)/$(CHUNKINGFEASEL) $(INSTALLDIR)
	cp $(BINDIR)/$(POSFEASEL) $(INSTALLDIR)
	cp $(BINDIR)/$(CONVERTCHK) $(INSTALLDIR)
	cp $(BINDIR)/$(EVALUATECHK) $(INSTALLDIR)
	cp $(BINDIR)/$(VOTING) $(INSTALLDIR)

uninstall:
	rm $(INSTALLDIR)/$(MAIN)
	rm $(INSTALLDIR)/$(CHUNKINGFEASEL)
	rm $(INSTALLDIR)/$(POSFEASEL)
	rm $(INSTALLDIR)/$(CONVERTCHK)
	rm $(INSTALLDIR)/$(EVALUATECHK)
	rm $(INSTALLDIR)/$(VOTING)
