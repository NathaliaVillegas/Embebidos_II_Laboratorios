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
all: ${COMPILER}/UART_mensaje_de_la_Tiva.axf

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
${COMPILER}/UART_mensaje_de_la_Tiva.axf: ${COMPILER}/UART_mensaje_de_la_Tiva.o
${COMPILER}/UART_mensaje_de_la_Tiva.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/UART_mensaje_de_la_Tiva.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/UART_mensaje_de_la_Tiva.axf: UART_mensaje_de_la_Tiva.ld
SCATTERgcc_UART_mensaje_de_la_Tiva=UART_mensaje_de_la_Tiva.ld
ENTRY_UART_mensaje_de_la_Tiva=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C129_RA1

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
