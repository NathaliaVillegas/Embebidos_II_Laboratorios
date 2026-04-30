#******************************************************************************
#
# Makefile - Rules for building the example.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=TM4C1294NCPDT

#
# The base directory for TivaWare.
#
ROOT=/home/nath/Documentos/SDKs/Tiva/TivaWare

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find header files that do not live in the source directory.
#
IPATH=/home/nath/Documentos/SDKs/Tiva/TivaWare

#
all: ${COMPILER}
all: ${COMPILER}/Ejercicio7.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}


#
${COMPILER}/Ejercicio7.axf: ${COMPILER}/Ejercicio7.o
${COMPILER}/Ejercicio7.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/Ejercicio7.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/Ejercicio7.axf: Ejercicio7.ld
SCATTERgcc_Ejercicio7=Ejercicio7.ld
ENTRY_Ejercicio7=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C129_RA1

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
