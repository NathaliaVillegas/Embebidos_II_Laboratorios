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

all: ${COMPILER}
all: ${COMPILER}/Ejer_tiva1.axf

clean:
	@rm -rf ${COMPILER} ${wildcard *~}

${COMPILER}:
	@mkdir -p ${COMPILER}

${COMPILER}/Ejer_tiva1.axf: ${COMPILER}/Ejer_tiva1.o
${COMPILER}/Ejer_tiva1.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/Ejer_tiva1.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/Ejer_tiva1.axf: Ejer_tiva1.ld
	$(LD) -T Ejer_tiva1.ld --entry ResetISR $(LDFLAGSgcc) --gc-sections -o $@ $(filter %.o %.a,$^) '$(LIBM)' '$(LIBC)' '$(LIBGCC)'
	@$(OBJCOPY) -O binary $@ ${@:.axf=.bin}

ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
