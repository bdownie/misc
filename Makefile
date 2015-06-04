CC=g++
#CFLAGS= -O3 -g3 -Wall -c -fmessage-length=0 -MMD -MP -pg
CFLAGS= -O3 -Wall 
#INCLUDE=-I/home/lakatos/bdownie/inc
#BOOST_INCLUDE=-I/home/lakatos/bdownie/inc/boost
INCLUDE=
BOOST_INCLUDE=
OMP_FLAGS=-fopenmp -lgomp
#OMP_LINK_FLAGS=-lgomp

all:join revcomp trim intersect_vcf

join: joinFasta.cpp
	${CC} ${CFLAGS} ${INCLUDE} -o joinFasta joinFasta.cpp 
#	${CC} -o"adjust_graph" adjust_graph.o

revcomp: reverseComplement.cpp
	${CC} ${CFLAGS} ${INCLUDE} -o reverseComplement reverseComplement.cpp 

#intersect_vcf: intersect_vcf.cpp
#	${CC} ${CFLAGS} ${INCLUDE} -o intersect_vcf intersect_vcf.cpp

trim: TrimLowQualNucleotides.cpp
		${CC} ${CFLAGS} ${INCLUDE} ${OMP_FLAGS} -o TrimLowQualNucleotides TrimLowQualNucleotides.cpp

intersect_vcf: intersect_vcf.cpp
		${CC} ${CFLAGS} ${INCLUDE}  -o intersect_vcf intersect_vcf.cpp

clean: 
	rm -rf *.d *.o 
	rm -rf joinFasta reverseComplement intersect_vcf
