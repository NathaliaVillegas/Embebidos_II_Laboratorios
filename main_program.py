#!/usr/bin/env python3
"""
=== ROBOT EMBEBIDOS - MAIN PROGRAM ===
Raspberry Pi Control Program
Models: A (Manual) and B (Autonomous)
"""

import serial
import RPi.GPIO as GPIO
import time
import threading
import sys
from enum import Enum

# ===== CONFIGURACION =====
BOTON_PIN = 25
TIVA_PORT = '/dev/ttyAMA0'
BAUD_RATE = 9600
TIMEOUT = 0.1

# ===== ENUMERACIONES =====
class ModeloOperacion(Enum):
    MODELO_A = 'A'  # Manual
    MODELO_B = 'B'  # Autonomo

class DireccionGiro(Enum):
    IZQUIERDA = 'L'
    DERECHA = 'R'

# ===== CLASE CONTROL ROBOT =====
class RobotControl:
    def __init__(self):
        self.puerto = None
        self.modelo_actual = ModeloOperacion.MODELO_A
        self.lock = threading.Lock()
        self.velocidad_m1 = 0
        self.velocidad_m2 = 0
        self.ejecutando = True
        self.distancia_ultima = 0
        self.girando = False
        
        # Configurar GPIO
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(BOTON_PIN, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        
        # Conectar a UART
        try:
            self.puerto = serial.Serial(TIVA_PORT, BAUD_RATE, timeout=TIMEOUT)
            print("[INFO] Puerto UART conectado correctamente")
        except Exception as e:
            print(f"[ERROR] No se pudo conectar al puerto UART: {e}")
            sys.exit(1)
    
    def enviar_comando(self, comando):
        """Enviar comando al Tiva"""
        try:
            if self.puerto and self.puerto.is_open:
                self.puerto.write(f"{comando}\n".encode())
        except Exception as e:
            print(f"[ERROR] Error al enviar comando: {e}")
    
    def girar(self, grados, direccion):
        """
        Ejecutar giro no-bloqueante
        grados: angulo de giro (0-360)
        direccion: DireccionGiro.IZQUIERDA o DireccionGiro.DERECHA
        """
        if grados <= 0 or grados > 360:
            print("[WARN] Grados inválidos (1-360)")
            return
        
        if self.girando:
            print("[WARN] Ya hay un giro en progreso")
            return
        
        self.girando = True
        comando = f"{direccion.value}{grados}"
        print(f"[GIRO] Iniciando giro: {grados}° hacia {'IZQUIERDA' if direccion == DireccionGiro.IZQUIERDA else 'DERECHA'}")
        self.enviar_comando(comando)
    
    def detener(self):
        """Detener motores"""
        self.enviar_comando("S0")
        self.velocidad_m1 = 0
        self.velocidad_m2 = 0
        print("[MOTOR] Sistema detenido")
    
    def adelante_velocidad(self, velocidad=50):
        """Movimiento adelante con velocidad especificada (0-100)"""
        velocidad = max(0, min(100, velocidad))
        self.enviar_comando(f"A{velocidad}")
        self.enviar_comando(f"B{velocidad}")
        self.velocidad_m1 = velocidad
        self.velocidad_m2 = velocidad
        print(f"[MOTOR] Adelante a {velocidad}%")
    
    def reversa_velocidad(self, velocidad=50):
        """Movimiento reversa (requerirá implementación en Tiva)"""
        velocidad = max(0, min(100, velocidad))
        print(f"[MOTOR] Reversa a {velocidad}% (no completamente implementado)")
    
    def sonar(self):
        """Activar buzzer"""
        self.enviar_comando("buzzer")
        print("[AUDIO] Buzzer activado")
    
    def leer_uart(self):
        """Thread: Leer mensajes del Tiva"""
        while self.ejecutando:
            try:
                if self.puerto and self.puerto.in_waiting > 0:
                    mensaje = self.puerto.readline().decode('utf-8').rstrip()
                    if mensaje:
                        self._procesar_mensaje_tiva(mensaje)
            except Exception as e:
                print(f"[ERROR] Error al leer UART: {e}")
                time.sleep(0.1)
    
    def _procesar_mensaje_tiva(self, mensaje):
        """Procesar mensajes recibidos del Tiva"""
        print(f"[TIVA] {mensaje}")
        
        # Parsear distancia
        if "Distancia:" in mensaje:
            try:
                distancia_str = mensaje.split(":")[1].strip().split()[0]
                self.distancia_ultima = int(distancia_str)
            except:
                pass
        
        # Detectar finalizacion de giro
        if "completado" in mensaje.lower() or "giro180" in mensaje.lower():
            self.girando = False
            if self.modelo_actual == ModeloOperacion.MODELO_B:
                # Reanudar movimiento en modo autonomo
                time.sleep(0.5)
                self.adelante_velocidad(75)
    
    def leer_boton(self):
        """Thread: Leer entrada del boton fisico"""
        estado_anterior = GPIO.HIGH
        contador_pulsaciones = 0
        
        while self.ejecutando:
            try:
                estado_actual = GPIO.input(BOTON_PIN)
                
                if estado_actual == GPIO.LOW and estado_anterior == GPIO.HIGH:
                    contador_pulsaciones += 1
                    print(f"[BOTON] Pulsacion #{contador_pulsaciones}")
                    
                    # Logica de boton
                    if contador_pulsaciones == 1:
                        self.adelante_velocidad(75)
                    elif contador_pulsaciones == 2:
                        self.girar(90, DireccionGiro.IZQUIERDA)
                    elif contador_pulsaciones == 3:
                        self.girar(90, DireccionGiro.DERECHA)
                    elif contador_pulsaciones >= 4:
                        self.detener()
                        contador_pulsaciones = 0
                    
                    time.sleep(0.05)
                
                estado_anterior = estado_actual
                time.sleep(0.01)
            except Exception as e:
                print(f"[ERROR] Error al leer boton: {e}")
    
    def modelo_manual(self):
        """Modo Manual (Modelo A) - Control por consola y boton"""
        print("\n" + "="*60)
        print("MODELO A - MODO MANUAL")
        print("="*60)
        print("Comandos disponibles:")
        print("  'adelante [velocidad]' - Avanzar (0-100%)")
        print("  'reversa [velocidad]'  - Retroceder (0-100%)")
        print("  'giza [grados]'        - Girar izquierda (1-360)")
        print("  'girde [grados]'       - Girar derecha (1-360)")
        print("  'detener'              - Detener motores")
        print("  'buzzer'               - Activar buzzer")
        print("  'modelo'               - Cambiar a modelo autonomo")
        print("  'salir'                - Terminar programa")
        print("="*60 + "\n")
        
        while self.ejecutando and self.modelo_actual == ModeloOperacion.MODELO_A:
            try:
                comando = input("Robot> ").strip().lower()
                
                if not comando:
                    continue
                
                partes = comando.split()
                accion = partes[0]
                
                if accion == "adelante":
                    velocidad = int(partes[1]) if len(partes) > 1 else 50
                    self.adelante_velocidad(velocidad)
                
                elif accion == "reversa":
                    velocidad = int(partes[1]) if len(partes) > 1 else 50
                    self.reversa_velocidad(velocidad)
                
                elif accion == "giza":
                    grados = int(partes[1]) if len(partes) > 1 else 90
                    self.girar(grados, DireccionGiro.IZQUIERDA)
                
                elif accion == "girde":
                    grados = int(partes[1]) if len(partes) > 1 else 90
                    self.girar(grados, DireccionGiro.DERECHA)
                
                elif accion == "detener":
                    self.detener()
                
                elif accion == "buzzer":
                    self.sonar()
                
                elif accion == "modelo":
                    self.modelo_actual = ModeloOperacion.MODELO_B
                    self.detener()
                
                elif accion == "salir":
                    self.ejecutando = False
                    break
                
                else:
                    print("[WARN] Comando no reconocido")
            
            except ValueError:
                print("[ERROR] Parámetro inválido")
            except KeyboardInterrupt:
                self.ejecutando = False
                break
            except Exception as e:
                print(f"[ERROR] {e}")
    
    def modelo_autonomo(self):
        """Modo Autonomo (Modelo B) - Navegacion automatica"""
        print("\n" + "="*60)
        print("MODELO B - MODO AUTONOMO")
        print("="*60)
        print("El robot busca obstáculos y gira 180° automáticamente")
        print("Presione Ctrl+C para volver a Modelo Manual")
        print("="*60 + "\n")
        
        # Iniciar modo autonomo en la Tiva
        self.enviar_comando("auto")
        time.sleep(1)
        
        # Iniciar movimiento
        self.adelante_velocidad(75)
        
        while self.ejecutando and self.modelo_actual == ModeloOperacion.MODELO_B:
            try:
                time.sleep(0.5)
                
                # El control es completamente automatico en la Tiva
                # Aqui solo monitoreamos distancia y estado
                if self.distancia_ultima < 5 and not self.girando:
                    print(f"[AUTO] Distancia crítica: {self.distancia_ultima}cm - Girando")
                    self.girando = True
                
            except KeyboardInterrupt:
                print("\n[INFO] Volviendo a Modelo Manual...")
                self.modelo_actual = ModeloOperacion.MODELO_A
                self.detener()
                break
            except Exception as e:
                print(f"[ERROR] {e}")
    
    def ejecutar(self):
        """Ejecutar programa principal"""
        # Iniciar threads de lectura
        hilo_uart = threading.Thread(target=self.leer_uart, daemon=True)
        hilo_uart.start()
        
        hilo_boton = threading.Thread(target=self.leer_boton, daemon=True)
        hilo_boton.start()
        
        print("\n" + "="*60)
        print("ROBOT EMBEBIDOS - PROGRAMA PRINCIPAL")
        print("="*60)
        print("Iniciando en MODELO A (Manual)")
        print("="*60 + "\n")
        
        try:
            while self.ejecutando:
                if self.modelo_actual == ModeloOperacion.MODELO_A:
                    self.modelo_manual()
                elif self.modelo_actual == ModeloOperacion.MODELO_B:
                    self.modelo_autonomo()
        except KeyboardInterrupt:
            print("\n[INFO] Interrupción detectada")
        finally:
            self.limpiar()
    
    def limpiar(self):
        """Limpiar recursos"""
        print("\n[INFO] Limpiando recursos...")
        self.ejecutando = False
        self.detener()
        
        if self.puerto and self.puerto.is_open:
            self.puerto.close()
        
        GPIO.cleanup()
        print("[INFO] Programa terminado")

# ===== PUNTO DE ENTRADA =====
if __name__ == "__main__":
    robot = RobotControl()
    robot.ejecutar()
