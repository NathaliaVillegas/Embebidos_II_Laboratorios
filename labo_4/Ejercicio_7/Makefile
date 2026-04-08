#******************************************************************************
# Makefile - Reglas para el ejercicio 6 de Tiva (Timers)
#******************************************************************************

# Definición del microcontrolador
PART=TM4C1294NCPDT

# Directorio base de TivaWare (sube 4 niveles desde boards/ek-tm4c1294xl/ejer6_tiva)
ROOT=/home/nath/Documentos/SDKs/Tiva/TivaWare

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find header files that do not live in the source directory.
#
IPATH=/home/nath/Documentos/SDKs/Tiva/TivaWare

# Regla por defecto: construye el archivo .axf
all: ${COMPILER}
all: ${COMPILER}/ejer6.axf

# Limpiar archivos de compilación previa
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

# Crear el directorio de salida (gcc)
${COMPILER}:
	@mkdir -p ${COMPILER}

# Reglas de compilación para ejer6
${COMPILER}/ejer6.axf: ${COMPILER}/ejer6.o
${COMPILER}/ejer6.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/ejer6.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
${COMPILER}/ejer6.axf: ejer6.ld

# Variables específicas para el linker
SCATTERgcc_ejer6=ejer6.ld
ENTRY_ejer6=ResetISR
CFLAGSgcc=-DTARGET_IS_TM4C129_RA1

# Incluir archivos de dependencia generados automáticamente
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
