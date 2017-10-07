# $Id: print_dnames.S,v 1.1 2008/04/22 03:11:50 hmasci Exp $

#include <mips/regdef.h>
#include <sys/syscall.h>

	.globl fdinput
	.globl fdoutput
	.globl bufferInput
	.data
	.align  2
	fdinput: .word 8
	fdoutput: .word 8
	bufferInput: .space 256
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
	
	
############################################################################
############ FUNCION READ BYTES AND STORE TO BUFFER ########################
############################################################################
read_bytes:
	lw t1,40($fp)			# sizeALeer = ifbytes
	lw t2, bufferInput		# guardo el addr del buffer
	#while(bytesread = read (fdinput, buffer, ibytes) < sizeALeer);
while_read:					
	lw  a0, fdinput			# a0 es fd del file input
    move  a1, t2			# a1 es el buffer donde almacenar
    move  a2, t1			# a2 es la cantidad de bytes a leer
    li  v0, SYS_read		# v0 es la syscall SYS_read
    syscall
    
    #control de errores
    bnez a0, read_fail		#a0 != 0 then salto a error
    #bytesALeer -= sizeLeido
    subu t1, t1, v0			#bytesALeer -= lectura
    addu t2, t2, v0			#aumento el addr del buffer para escribir correctamente si es que todavia no se escribio todo
    bnez t1, while_read		#si todavia queda algo por leer, vuelvo al loop.
    
    #Test escribir a consola el buffer
	li	a0, 1
	lw	a1, bufferInput
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

