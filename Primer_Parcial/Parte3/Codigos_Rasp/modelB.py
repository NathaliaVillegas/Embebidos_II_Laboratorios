import serial
import sys
import tty
import termios
import time

try:
    puerto = serial.Serial('/dev/ttyAMA0', 9600, timeout=1)
except Exception as e:
    sys.exit(1)

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

print("--- CONTROL REMOTO ACTIVADO ---")
print("W: Adelante | S: Atras | A: Izquierda | D: Derecha")
print("N: Nitro (70% <-> 100%) | Espacio: Parar | X: Salir")

try:
    while True:
        tecla = leer_tecla().lower()
        if tecla in ['w', 'a', 's', 'd', 'n', ' ']:
            puerto.write(tecla.encode('utf-8'))
            print(f"\rComando: {tecla.upper()}        ")
            time.sleep(0.05)
        elif tecla == 'x' or ord(tecla) == 3:
            break
except Exception:
    pass
finally:
    puerto.write(b' ')
    puerto.close()
    print("\r\nFin.")
