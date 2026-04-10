# =============================================================================
# Template_RaspberryPi.py
# =============================================================================

# INCLUDES -------------------------------------------------
import RPi.GPIO as GPIO
import time
import serial   # UART

# =============================================================================
# CONFIGURACIÓN GENERAL
# =============================================================================

GPIO.setmode(GPIO.BCM)  # Usar numeración BCM (recomendado)

# =============================================================================
# GPIO ---------------------------
# =============================================================================

# PINES
LED_PIN = 18
BUTTON_PIN = 17

# Configuración
GPIO.setup(LED_PIN, GPIO.OUT)
GPIO.setup(BUTTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)  # Pull-up interno

# =============================================================================
# PWM ---------------------------
# =============================================================================

# Configurar PWM sobre un pin GPIO
PWM_PIN = 18
PWM_FREQ = 1000  # Hz

GPIO.setup(PWM_PIN, GPIO.OUT)
pwm = GPIO.PWM(PWM_PIN, PWM_FREQ)
pwm.start(0)  # Duty inicial 0%

# =============================================================================
# UART ---------------------------
# =============================================================================

# Configurar UART (ej: /dev/serial0)
uart = serial.Serial(
    port='/dev/serial0',
    baudrate=9600,
    timeout=0.1
)

# Variables tipo buffer (como en Tiva)
BUFFER_SIZE = 50
buffer = ""
mensajeCompleto = False

# =============================================================================
# ADC ---------------------------
# =============================================================================

# IMPORTANTE:
# Raspberry Pi NO tiene ADC interno
# necesitas uno externo (ej: MCP3008)

# Ejemplo conceptual (no funcional sin librería):
# import spidev
# spi = spidev.SpiDev()
# spi.open(0, 0)

def read_adc():
    # Simulación (porque depende del ADC externo)
    value = 0
    return value

# =============================================================================
# FUNCIONES
# =============================================================================

def Delay(time_s):
    time.sleep(time_s)

# UART lectura tipo interrupción (simulada)
def UART_read():
    global buffer, mensajeCompleto

    if uart.in_waiting > 0:
        c = uart.read().decode('utf-8')

        if c == '\n' or c == '\r':
            mensajeCompleto = True
        else:
            if len(buffer) < BUFFER_SIZE:
                buffer += c

# =============================================================================
# LOOP PRINCIPAL
# =============================================================================

try:
    while True:

        # -------------------------------------------------
        # GPIO OUTPUT
        # -------------------------------------------------
        # GPIO.output(LED_PIN, GPIO.HIGH)

        # -------------------------------------------------
        # ADC
        # -------------------------------------------------
        adcValue = read_adc()

        # -------------------------------------------------
        # PWM
        # -------------------------------------------------
        # Duty cycle de 0 a 100
        # pwm.ChangeDutyCycle(30)

        # -------------------------------------------------
        # UART ENVÍO
        # -------------------------------------------------
        # uart.write(b"Mensaje\r\n")

        # -------------------------------------------------
        # UART RECEPCIÓN
        # -------------------------------------------------
        UART_read()

        if mensajeCompleto:
            print("Mensaje recibido:", buffer)
            buffer = ""
            mensajeCompleto = False

        # -------------------------------------------------
        # BUTTON (ANTI-BOUNCE)
        # -------------------------------------------------
        if GPIO.input(BUTTON_PIN) == 0:
            Delay(0.02)

            if GPIO.input(BUTTON_PIN) == 0:
                print("Botón presionado")

                while GPIO.input(BUTTON_PIN) == 0:
                    pass

                Delay(0.02)

        # -------------------------------------------------
        # DELAY GENERAL
        # -------------------------------------------------
        Delay(0.01)

# =============================================================================
# CLEANUP
# =============================================================================

except KeyboardInterrupt:
    print("Programa detenido")

finally:
    pwm.stop()
    GPIO.cleanup()
    uart.close()