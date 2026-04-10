import serial
import sys
import tty
import termios
import time
import threading

PUERTO = '/dev/ttyAMA0'

try:
    puerto = serial.Serial(PUERTO, 9600, timeout=0.1)
except Exception as e:
    print(f"Error abriendo {PUERTO}: {e}")
    sys.exit(1)

def leer_uart():
    while True:
        try:
            if puerto.in_waiting > 0:
                mensaje = puerto.readline().decode('utf-8', errors='ignore').strip()
                if mensaje:
                    print(f"\r[TIVA] {mensaje}                        ")
        except:
            pass


def leer_tecla():
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    ch = ''
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    return ch
hilo_uart = threading.Thread(target=leer_uart)
hilo_uart.daemon = True
hilo_uart.start()

print("=== SISTEMA DE CONTROL Y TELEMETRIA ===")
print("W: Adelante | S: Atras | A: Izquierda | D: Derecha")
print("N: Nitro (70/100%) | Espacio: Freno | X: Salir\n")

try:
    while True:
        tecla = leer_tecla().lower()
        
        if tecla in ['w', 'a', 's', 'd', 'n', ' ']:
            puerto.write(tecla.encode('utf-8'))
            time.sleep(0.05)
            
        elif tecla == 'x' or ord(tecla) == 3:
            print("\r\nApagando sistema...")
            break
except Exception:
    pass
finally:
    puerto.write(b' ')
    time.sleep(0.1)
    puerto.close()
    print("\rPuerto cerrado.")
