# $Id: print_dnames.S,v 1.1 2008/04/22 03:11:50 hmasci Exp $

#include <mips/regdef.h>
#include <sys/syscall.h>

	.globl fdinput
	.globl fdoutput
	.globl bufferInput
	.globl bufferOutput
	.globl palabra
	.data
	.align  2
	fdinput: .space 8
	fdoutput: .space 8
	bufferInput: .space 256
	bufferOutput: .space 256
	palabra: .space 256
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
	
	sw	a0, fdinput			# almaceno arg fileno de input en global var
	sw	a1, 40($fp)			# almaceno arg cantidad de bytes de buffer input en stack del caller
	sw	a2, fdoutput		# almaceno arg fileno de output en global var
	sw	a3, 44($fp)			# almaceno arg cantidad de bytes de buffer output en stack del caller
	
	la t9, read_bytes
	lw a0, 40($fp)
	jal ra, t9
	sw v0, 20($fp)			# almaceno el resultado del read bytes en LA
	
	li t0, 0				# t0 -> indexBufInput = 0
	sw t0, 16($fp)			# guardo el indice para despues usarlo
while_getch:
	lw t0, 16($fp)			# recupero el indice
	la t9, getch			# guardo el addr de getch
	move a0, t0				# le paso el indice en a0
	jal ra, t9				# llamo a getch
	la t1, palabra			# t1 -> guardo el addr del buffer de palabra en t1.
	lw t0, 16($fp)			# recupero el indice
	addu t1, t1, t0			# avanzo en el word hasta el espacio que corresponde
	sb v0, (t1)				# guardo el byte leido en el word
	addu t0, t0, 1			# index++
	sw t0, 16($fp)			# guardo el indice en el stack
	lw t2, 20($fp)			# recupero bytes leidos
	lw t0, 16($fp)			# recupero el indice
	#beq t2, t0, fillBuffer
	
	bnez v0, while_getch
	
#fillBufer:
#	la t9, read_bytes
#	lw a0, 40($fp)
#	jal ra, t9
#	sw v0, 20($fp)			# almaceno el resultado del read bytes en LA
	
test:	
	li a0, 1				# a0 es fd del file output
	la a1, palabra			# a1 es buffer de donde leer
	lw a2, 20($fp)			# recupero bytes leidos
	li v0, SYS_write		# v0 es la syscall SYS_write
	syscall
	
	b end_palindrome

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

	.end	palindrome
	.size	palindrome,.-palindrome
	

	.ent 	write_bytes
write_bytes:
	
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
	
	la t2, bufferOutput		# guardo el addr del buffer
	
	la a0, bufferOutput		# paso en a0 a mystrlen el buffer del que quiero calcular el len
	la	t9, mystrlen		# mystrlen(buffer)
	jal	ra, t9
	move	t1, v0			# return en v0 -> t1
	
	#while (byteswrite = write(fdoutput, buffer, obytes)
while_write:
	lw a0, fdoutput			# a0 es fd del file output
	move a1, t2				# a1 es buffer de donde leer
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

end_write:
	# restauro callee-saved regs
	lw	gp, 24(sp)
	lw	$fp, 28(sp)
	lw	ra, 32(sp)
	# destruyo stack frame
	addu	sp, sp, 40
	# vuelvo a funcion llamante
	jr	ra
	.end 	write_bytes
	.size	write_bytes,.-write_bytes

	.ent 	read_bytes
read_bytes:
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

	sw a0, 40($fp)			# Guardo el size a leer recibido por parametro
	
	lw t1,40($fp)			# t1 = sizeALeer
	la t2, bufferInput		# guardo el addr del buffer
	move	t0, zero	# i=0: t0, fp+0
	sw	t0, 0($fp)	# i: t0
	#mientras (bytesread = read (fdinput, buffer, ibytes) > 0);
while_read:
	lw  a0, fdinput			# a0 es fd del file input
    move  a1, t2			# a1 es el buffer donde almacenar
    move  a2, t1			# a2 es la cantidad de bytes a leer
    li  v0, SYS_read		# v0 es la syscall SYS_read
    syscall
    lw	t0, 0($fp)	
	addu	t0, t0, v0
	sw	t0, 0($fp)
	#si (bytesread == 0)
    beqz v0, end_while_read	# si lei 0 bytes salto al fin del while porque no hay mas para leer.
    
    #control de errores
    bnez a3, read_fail		# a3 != 0 then salto a error
    #si no bytesALeer -= sizeLeido para comprobar si lei todo lo que queria leer.
    subu t1, t1, v0			# bytesALeer -= bytesLeidos
    addu t2, t2, v0			# aumento el addr del buffer para poder poner en la posicion correcta lo restante (BUFIN += indexRead)
    bnez t1, while_read		# si todavia queda algo por leer, vuelvo al loop.
end_while_read:
	lw	v0, 0($fp)		# guardo en v0 la cantidad de bytes leidos para devolverlo
	b end_read				# si llegue hasta aca, salto al final.
read_fail:
	subu v0, zero, a3		
	b end_read

end_read:
	# return bytesRead;
	# restauro callee-saved regs
	lw	gp, 24(sp)
	lw	$fp, 28(sp)
	lw	ra, 32(sp)
	# destruyo stack frame
	addu	sp, sp, 40
	# vuelvo a funcion llamante
	jr	ra

	.end 	read_bytes
	.size	read_bytes,.-read_bytes

.ent 	getch
getch:
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
	
	sw a0, 40($fp)			# guardo el indice en el stack
	la t2, bufferInput		# t2 -> addr bufferInput
	addu t4, t2, a0			# t4 += bufferInput + indexInput
	lb v0, 0(t4)			# v0 = bufferInput[index]
	
end_getch:
	# restauro callee-saved regs
	lw	gp, 24(sp)
	lw	$fp, 28(sp)
	lw	ra, 32(sp)
	# destruyo stack frame
	addu	sp, sp, 40
	# vuelvo a funcion llamante
	jr	ra
	.end 	getch
	.size	getch,.-getch

	.ent	mystrlen
mystrlen:
	.frame	$fp, 16, ra
	.set	noreorder
	.cpload t9
	.set	reorder

	# creo stack frame
	subu	sp, sp, 16	# 2 (SRA) + 2 (LTA)
	.cprestore 8		# sw gp, 8(sp)
	sw	$fp, 12(sp)
	move	$fp, sp

	# salvo 1er arg (siempre)
	sw	a0, 16($fp)			## redundante

	# for (i=0; s[i] != 0; i++)
	move	t0, zero	# i=0: t0, fp+0
	sw	t0, 0($fp)	# i: t0
_for_loop:
	# condicion de corte: s[i] != 0
	lw	a0, 16($fp)			## redundante
	lw	t0, 0($fp)			## redundante
	addu	t1, a0, t0	# s[i]: t1
	lb	t1, 0(t1)			## lb, NO lw!
	beq	t1, zero, _end_for
	
	lw	t0, 0($fp)	# i++		## redundante
	addu	t0, t0, 1
	sw	t0, 0($fp)			## redundante
	j	_for_loop

_end_for:
	lw	v0, 0($fp)			## podria ser un move v0, t0
	lw	gp, 8(sp)
	lw	$fp, 12(sp)
	addu	sp, sp, 16
	jr	ra

	.end	mystrlen
	.size	mystrlen,.-mystrlen	
	
	.rdata			# segmento read-only data

