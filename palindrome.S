# $Id: print_dnames.S,v 1.1 2008/04/22 03:11:50 hmasci Exp $

#include <mips/regdef.h>
#include <sys/syscall.h>

	.globl fdinput
	.globl fdoutput
	.globl bufferInput
	.data
	.align  2
	fdinput: .space 8
	fdoutput: .space 8
	bufferInput: .space 256
	bufferOutput: .space 256
	.text			# segmento de texto del programa
	
	.abicalls
	.align 2		# alineacion 2^2

	.globl palindrome
	.ent palindrome
palindrome:
	.frame	$fp, 40, ra		# $fp: registro usado como frame pointer, 40: tamañodel stack frame, ra: registro que almacena el return address
	
	# bloque para codigo PIC
	.set	noreorder		# apaga reordenamiento de instrucciones
	.cpload t9				# directiva usada para codigo PIC
	.set	reorder			# enciende reordenamiento de instrucciones
	
	subu	sp, sp, 40		#Se crea el stack frame

							# directiva para codigo PIC
	.cprestore 24			# inserta aqui "sw gp, 24(sp)", mas "lw gp, 24(sp)" luego de cada jal.
	
	sw	$fp, 28(sp)			# guardo fp en el stack
	sw	ra, 32(sp)			# guardo ra en el stack

	move	$fp, sp 		# de aqui al fin de la funcion uso $fp en lugar de sp.
	
	#Inicio funcion palindrome
	
	sw	a0, fdinput			# almaceno arg fileno de input e:n global var
	sw	a1, 40($fp)			# almaceno arg cantidad de bytes de buffer input
	sw	a2, fdoutput		# almaceno arg fileno de output en global var
	sw	a3, 44($fp)			# almaceno arg cantidad de bytes de buffer output
	
read_bytes:
	lw t1,40($fp)			# sizeALeer = ibytes
	la t2, bufferInput		# guardo el addr del buffer
	#while(bytesread = read (fdinput, buffer, ibytes) < sizeALeer);
while_read:					
	lw  a0, fdinput			# a0 es fd del file input
    move  a1, t2			# a1 es el buffer donde almacenar
    move  a2, t1			# a2 es la cantidad de bytes a leer
    li  v0, SYS_read		# v0 es la syscall SYS_read
    syscall
    
    #control de errores
    bnez a3, read_fail		# a3 != 0 then salto a error
	#si (bytesread == 0)
    beqz v0, write_bytes	# si lei 0 bytes salto al fin porque no hay mas para leer.
    #si no bytesALeer -= sizeLeido para comprobar si lei todo lo que queria leer.
    subu t1, t1, v0			# bytesALeer -= lectura
    addu t2, t2, v0			# aumento el addr del buffer para poder poner en la posicion correcta lo restante
    bnez t1, while_read		# si todavia queda algo por leer, vuelvo al loop.

write_bytes:
	lw t1,44($fp)			# sizeAEscribir = obytes
	la t2, bufferInput		# guardo el addr del buffer
	#while (byteswrite = write(fdoutput, buffer, obytes)
while_write:
	lw a0, fdoutput			# a0 es fd del file output
	move a1, t2				# a1 es buffer donde almacenar
	move a2, t1				# a2 es la cantidad de bytes a escribir
	li v0, SYS_write		# v0 es la syscall SYS_write
	syscall
	
	#control de errores
    bnez a3, read_fail		# a3 != 0 then salto a error
    #si (byteswrite == 0)
    beqz v0, end_palindrome	# si escribi 0 bytes salto al fin porque no hay nada mas para escribir
	#si no sizeAEscribir -= sizeEscrito
	subu t1, t1, v0			# sizeAEscribir -= sizeEscrito
	addu t2, t2, v0			# aumento el addr del buffer para poder poner en la posicion correcta lo restante
	bnez t1, while_write	# si todavia queda algo por escribir, vuelvo al loop.

test:    
   #Test escribir a consola el buffer
	li	a0, 1
	la	a1, bufferInput
	lw	a2, 40($fp)
	li	v0, SYS_write
	syscall
    	
	#Fin funcion palindrome
end_palindrome:
	# return;
	# restauro callee-saved regs
	lw	gp, 24(sp)
	lw	$fp, 28(sp)
	lw	ra, 32(sp)
	# destruyo stack frame
	addu	sp, sp, 40
	# vuelvo a funcion llamante
	jr	ra
	
read_fail:
	subu v0, zero, a3		
	b end_palindrome
	
	.end	palindrome
	.size	palindrome,.-palindrome
	
	
	.rdata			# segmento read-only data

