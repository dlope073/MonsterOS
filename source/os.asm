; Copyright (c) 2013, Daniel Lopez
;All rights reserved.
;
;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
;
;Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
;Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

;Kernel

[ORG 0x7e00]

jmp main

%include "syscalls.inc"



main:

call init_int

xor ax, ax ; make it zero
mov ds, ax ; Data segment is zero
mov es, ax



; Clear Screen

int 27h

;Set desired background color (Green)
mov ah, 0x0b 
mov bh, 0
mov bl, 2
int 0x10	




hang:
	
	;mov ax, buffer
	;mov bx, 8 ; Length
	;int 24h ;ZERO(buffer)
	

	; Print Desired Message

	mov ax, cli_msg
	mov bh, 0
	mov bl, 0xf

	int 21h ; print ax=msg bl=blue

	; Get Input
	mov bx, buffer
	int 22h ; Read From Keyboard  And Store String In Buffer
	
	mov bx, buffer
	mov ax, internal_command_A
	
	int 30h ; string compare buffer and commandA
	
	cmp dx, 0 
	
	je help_command ;if (buffer == command) goto help_command
	
	mov bx, buffer
	mov ax, internal_command_B
	
	int 30h ; string compare buffer and commandB
	
	cmp dx, 0 
	
	jne invalid

clear_commmand:

		; Clear Screen

		int 27h
		
		jmp hang
	
help_command:

		int 23h ; Print newline

		mov ax, help_msg
		mov bx, 000fh
		int 21h ; Print help string

		int 23h ; Print newline
		
		jmp hang
	
invalid:
	
	int 23h ; Print newline

	
	mov ax, invalid_cmd
	mov bx, 000fh
	int 21h ; Print invalid command message

	int 23h ; Print newline

	
	jmp hang
	
cli_msg: db 13, 10, 'MonsterOS> ', 0
help_msg: db 'Welcome to the MonsterOS shell prompt.', 13, 10, 13, 10, 'MonsterOS shell prompt contains only two internal commands: help and clear.', 13, 10, 'This means that MonsterOS treats everything else that is not the help/clear command as an external command.', 13, 10, 'An external command is basically a filename of a file stored on the disk.', 10, 13, 'If the file is located on the disk it is executed if it is a program; else it is treated a text file and its contents are displayed.', 13, 10, 'If no file is found then a error message is displayed.', 0
internal_command_A: db 'help', 0
internal_command_B: db 'clear', 0
invalid_cmd: db 'Invalid Internal / External Command!',0
buffer: times 8 db 0

