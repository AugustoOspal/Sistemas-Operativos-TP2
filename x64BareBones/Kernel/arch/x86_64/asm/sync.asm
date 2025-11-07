GLOBAL acquire
GLOBAL try_acquire
GLOBAL release

;   cmpxchg [destino], fuente
;   cmpchg realiza lo siguiente de forma atomica:
;   1) Compara el valor en AL/AX/EAX/RAX (dependiendo el tamaño) con el valor en [destino]
;   2) Si son iguales (Flag zero = 1) carga fuente en [destino]
;   3) Si son diferentes (Flag zero = 0) carga [destino] en AL/AX/EAX/RAX

;   La funcion es parecida a la que se describe en la pagina 267
;   del libro de Abraham-Silberschatz Operating systems 10th 2018

;   Segun el libro en la pag 269 el lock es un prefijo que sirve para bloquear
;   el bus mientras se actualiza el valor de [destino]


SECTION .text

;   Me suena medio raro tener que poner rax = 0 en cada iteracion
;   pero no encontre otra forma de hacerlo usando cmpxchg.
;   Si no pongo el mov rax, 0 en cada iteracion, y en la primera iteracion
;   [rdi] era 1, cmpxchg le carga 1 a rax, y en la segunda iteracion siempre
;   va a dar verdadero, por lo que le carga 1 a [rdi] (que ya era 1) y sale.
;   De esa forma creo que se podria usar a forma de available y que acquire llame a
;   available varias veces, pero me parecio mas eficiente asi.

acquire:
    mov rcx, 1
.loop:
    mov rax, 0
    lock cmpxchg [rdi], rcx
    jnz .loop
    ret

try_acquire:
    mov rax, 0
    mov rcx, 1
    lock cmpxchg [rdi], rcx
    ret

release:
    mov qword [rdi], 0
    ret
