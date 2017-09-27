# $Id: print_dnames.S,v 1.1 2008/04/22 03:11:50 hmasci Exp $

#include <mips/regdef.h>
#include <sys/syscall.h>

	.text			# segmento de texto del programa
	
	.abicalls
	.align 2		# alineacion 2^2

	.globl palindrome
	.ent palindrome
palindrome:
	# debugging info: descripcion del stack frame
	.frame	$fp, 40, ra	# $fp: registro usado como frame pointer, 40: tamañodel stack frame, ra: registro que almacena el return address
	
	# bloque para codigo PIC
	.set	noreorder	# apaga reordenamiento de instrucciones
	.cpload t9		# directiva usada para codigo PIC
	.set	reorder		# enciende reordenamiento de instrucciones
	
	# creo stack frame
	subu	sp, sp, 40	# 4 (SRA) + 2 (LTA) + 4 (ABA)

	# directiva para codigo PIC
	.cprestore 24		# inserta aqui "sw gp, 24(sp)", mas "lw gp, 24(sp)" luego de cada jal.
	# salvado de callee-saved regs en SRA	
	sw	$fp, 28(sp)
	sw	ra, 32(sp)

	# de aqui al fin de la funcion uso $fp en lugar de sp.
	move	$fp, sp
	
	# salvo 1er arg (siempre)
	sw	a0, 40($fp)	# n: a0, sp+40
	
	#Inicio funcion palindrome
	
	#	write(1, " ", 1);
	li	a0, 1		# fd=1: a0
	la	a1, sep		# buf=" ": a1	## la, NO lw
	li	a2, 13		# count=1: a2
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

	.end	palindrome
	.size	palindrome,.-palindrome
	
	
	.rdata			# segmento read-only data

	.align 2
name:	.word name_0, name_1, name_2, name_3, name_4, \
	      name_5, name_6, name_7, name_8, name_9
	.size name, 40

	.align 0		# alineacion 2^0

name_0: .asciiz "cero"
name_1: .asciiz "uno"
name_2: .asciiz "dos"
name_3: .asciiz "tres"
name_4: .asciiz "cuatro"
name_5: .asciiz "cinco"
name_6: .asciiz "seis"
name_7: .asciiz "siete"
name_8: .asciiz "ocho"
name_9: .asciiz "nueve"

sep:	.ascii "Hello world!"
