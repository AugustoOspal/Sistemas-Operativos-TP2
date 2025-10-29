# Guia para debugear

Por ahora esta funcionando muy basico. Para debugear hay que hacer esto:
- Abir terminal grande (no la integrada al ide)
- Correr ./debug.sh
- Cuando les abra puden hacer varias cosas:
    - Poner break main para poner el brakepoint en main
    - Poner c para que corra hasta llegar a main (el brakepoint de antes)
    - Poner lay src para poder ir viendo el pedazo de codigo arriba
    - Poner n (next) para ir linea por linea
    - Mañana les cuento mas

# Importantisimo!!!
Se tilda todo el tiempo, no se asusten, gdb es asi. Cuando se les glitchee toda la pantalla pueden hacer dos cosas:
- Tocar Ctrl + L 
- Escribir refresh

Cualquiera de esas dos les va a sacar el glitch temporalmente

Cuando salgan de gdb toquen Ctrl + C para que no quede abierto el proceso

# Mañana lo mejoro