from gpiozero import Button, Buzzer
import time

# Configuración de Hardware (usando tus pines originales)
# Pin 17: Botón para encender
# Pin 27: Buzzer
# Pin 22: Nuevo botón para apagar (puedes cambiarlo si prefieres otro)
btn_encender = Button(17)
btn_apagar = Button(22) 
bzr_1 = Buzzer(27)

print("--- Control de Buzzer vía SSH ---")
print("Botón 1 (Pin 17): ENCENDER")
print("Botón 2 (Pin 22): APAGAR")
print("Presiona Ctrl+C para salir.")

try:
    while True:
        # Lógica de "Set/Reset":
        if btn_encender.is_pressed:
            bzr_1.on()
            print("Buzzer activado")
        
        if btn_apagar.is_pressed:
            bzr_1.off()
            print("Buzzer desactivado")
            
        time.sleep(0.1) 

except KeyboardInterrupt:
    print("\nConexión terminada. Limpiando GPIO...")
    bzr_1.off()