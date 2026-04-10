import serial
import socket
import threading
import time
import sys

PUERTO = '/dev/ttyAMA0'
try:
    puerto_tiva = serial.Serial(PUERTO, 9600, timeout=0.1)
except Exception as e:
    print(f"Error abriendo UART: {e}")
    sys.exit(1)

def leer_tiva(cliente_bt):
    while True:
        try:
            if puerto_tiva.in_waiting > 0:
                msj = puerto_tiva.readline().decode('utf-8', errors='ignore')
                if msj and cliente_bt:
                    cliente_bt.send(msj.encode('utf-8'))
                    print(f"\r[TIVA] {msj.strip()}        ", end="")
        except:
            pass
        time.sleep(0.01)

try:
    server_sock = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
    server_sock.bind((socket.BDADDR_ANY, 1))
    server_sock.listen(1)
except Exception as e:
    print(f"Error al iniciar Bluetooth. Intenta correr con 'sudo': {e}")
    sys.exit(1)

print("=== MODELO C: CONTROL BLUETOOTH DESDE PC ===")
print("La Raspberry Pi está esperando conexión Bluetooth...")

try:
    client_sock, address = server_sock.accept()
    print(f"\n[+] ¡PC Conectada! Dirección: {address}")
    
    client_sock.send(b"\r\n=== CONECTADO AL ROBOT ===\r\n")
    client_sock.send(b"Controles: W, A, S, D | N: Nitro | Espacio: Freno\r\n")

    threading.Thread(target=leer_tiva, args=(client_sock,), daemon=True).start()

    while True:
        data = client_sock.recv(1024)
        if not data:
            break 
        
        comando = data.decode('utf-8', errors='ignore').strip().lower()
        
        for c in comando:
            if c in ['w', 'a', 's', 'd', 'n', ' ']:
                puerto_tiva.write(c.encode('utf-8'))
                print(f"\n[BT] Comando recibido: {c.upper()}")
                
except Exception as e:
    print(f"\n[-] Conexión Bluetooth terminada: {e}")
finally:
    puerto_tiva.write(b' ')
    try:
        client_sock.close()
        server_sock.close()
    except:
        pass
    puerto_tiva.close()
    print("Sistema cerrado.")
