import serial
import time
import sys
import tty
import termios
import threading

try:
    puerto = serial.Serial('/dev/ttyAMA0', 9600, timeout=0.01)
except:
    puerto = serial.Serial('/dev/serial0', 9600, timeout=0.01)

ESTADO_ACTUAL = "A"

def cambiar_modelo():
    global ESTADO_ACTUAL
    if ESTADO_ACTUAL == "A":
        ESTADO_ACTUAL = "B"
        puerto.write(b'2') 
        puerto.write(b' ')
        sys.stdout.write("       ESTÁS EN MODELO B (Manual)       \r\n")
        sys.stdout.write("Controles: W, A, S, D, N, Espacio.\r\n")
    else:
        ESTADO_ACTUAL = "A"
        puerto.write(b'1') 
        sys.stdout.write("      ESTÁS EN MODELO A (Autónomo)      \r\n")
        sys.stdout.write("Escribe el comando y presiona Enter.\r\n")
        sys.stdout.write("Ejemplo: V50, L45, R90, STOP\r\n\nComando: ")
    sys.stdout.flush()

def escuchar_tiva():
    while True:
        try:
            if puerto.in_waiting > 0:
                msj = puerto.readline().decode('utf-8', errors='ignore').strip()
                if msj:
                    if msj == "CMD:CAMBIO":
                        cambiar_modelo()
                    else:
                        sys.stdout.write(f"\r[TIVA] {msj}                        \r\n")
                        if ESTADO_ACTUAL == "A" and "D:" not in msj:
                            sys.stdout.write("Comando: ")
                        sys.stdout.flush()
        except:
            pass
        time.sleep(0.01)

threading.Thread(target=escuchar_tiva, daemon=True).start()

print("    MAIN PROGRAM INICIADO    ")
print("El robot inicia en MODELO A (Autonomo).")
print("-> Presiona el BOTÓN SW1 en la TIVA para cambiar al Modelo B.")
print("Escribe 'X' para cerrar programa.\n")
sys.stdout.write("Comando: ")
sys.stdout.flush()

buffer_modelo_a = ""
fd = sys.stdin.fileno()
old_settings = termios.tcgetattr(fd)

try:
    tty.setcbreak(fd)
    while True:
        ch = sys.stdin.read(1)
        
        if ch.lower() == 'x' or ord(ch) == 3:
            break
            
        if ESTADO_ACTUAL == "A":
            if ch in ['\n', '\r']:
                cmd = buffer_modelo_a.upper()
                if cmd == "STOP": 
                    puerto.write(b"V0\n")
                elif cmd.startswith(('V','L','R')): 
                    puerto.write(f"{cmd}\n".encode())
                
                sys.stdout.write(f"\r\n-> [ENVIADO AL ROBOT]: {cmd}\r\n")
                buffer_modelo_a = ""
                sys.stdout.write("Comando: ")
                sys.stdout.flush()
                
            elif ch == '\x7f' or ch == '\b':
                if len(buffer_modelo_a) > 0:
                    buffer_modelo_a = buffer_modelo_a[:-1]
                    sys.stdout.write("\b \b") 
                    sys.stdout.flush()
            
            elif ch.isalnum():
                buffer_modelo_a += ch.upper()
                sys.stdout.write(ch.upper())
                sys.stdout.flush()
                
        elif ESTADO_ACTUAL == "B":
            if ch.lower() in ['w', 'a', 's', 'd', 'n', ' ']:
                puerto.write(ch.lower().encode('utf-8'))

except Exception:
    pass
finally:
    termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    puerto.write(b' ')
    puerto.close()
    print("\r\nSistema apagado.")
