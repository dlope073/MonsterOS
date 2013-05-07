; Copyright (c) 2013, Daniel Lopez
;All rights reserved.
;
;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
;
;Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
;Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

	; Print Desired Message

	mov ax, cli_msg
	mov bh, 0
	mov bl, 0xf

	int 21h ; print ax=msg bl=blue

	; Get Input
	mov bx, buffer
	int 22h ; Read From Keyboard  And Store String In Buffer
	
	mov bx, buffer
	mov ax, only_internal_command
	
	int 30h ; string compare buffer and command
	
	cmp dx, 0 
	
	jne invalid ;if (buffer != command) goto invalid
	
	int 23h ; Print newline
	
	mov ax, help_msg
	mov bx, 000fh
	int 21h ; Print help string

	int 23h ; Print newline

	mov cx, 8
	
	mov ax, buffer
	int 24h ;ZERO(buffer)
	
	jmp hang
	
invalid:
	
	int 23h ; Print newline

	
	mov ax, invalid_cmd
	mov bx, 000fh
	int 21h ; Print invalid command message

	int 23h ; Print newline

	mov cx, 8 ; buffer length 8
	
	mov ax, buffer
	int 24h ;ZERO(buffer)
	

	
	jmp hang
	
cli_msg: db 13, 10, 'MonsterOS> ', 0
help_msg: db 'Welcome to the MonsterOS shell prompt. MonsterOS shell prompt contains only one internal command which is the command help. This means that MonsterOS treats everything else that is not the help command as an external command. An external command is basically a filename of a file on the hard disk. If the file is located on the disk it executed if it is a program else it is treated a text file and its contents are displayed. If no file is found then a error message is displayed. A list of external programs available on the disk is shown below:', 0
only_internal_command: db 'help', 0
invalid_cmd: db 'Invalid Internal / External Command!',0
buffer: times 8 db 0

