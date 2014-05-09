; Copyright (c) 2014, Daniel Lopez
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

call get_drive_geometry ; Get the drive geometry 

; Clear Screen

int 27h


; Print Startup Message

mov ax, welcome_msg ; Store pointer to string AX register
mov bx, current_color ; Store current color in BX
int 21h ; Call print ISR

mov bx, pwd
mov byte[bx], '\'


hang:
	

	call zero_buffer
	
	; Print Cli message plus current working path plus drive letter

	mov ax, cli_msg
	mov bx, current_color

	int 21h ; print ax=msg bl=blue
	
	mov ax, drive
	mov bx, current_sector
	
	int 21h ; print
	
	mov ax, pwd
	mov bx, current_sector
	
	int 21h ; print
	
	mov ax, arrow
	mov bx, current_color
	
	int 21h ; print

	; Get Input
	mov bx, buffer
	int 22h ; Read From Keyboard  And Store String In Buffer
	
	cmp byte[bx], 0 ; Empty command are ignored therefore
	
	je hang
	
	mov bx, buffer
	mov ax, internal_command_A
	
	int 30h ; string compare buffer and commandA
	
	cmp dx, 0 
	
	je help_command ;if (buffer == command) goto help_command
	
	mov bx, buffer
	mov ax, internal_command_B
	
	int 30h ; string compare buffer and commandB
	
	cmp dx, 0 
	
	je clear_command
	
	mov bx, buffer
	mov ax, internal_command_C
	
	int 30h ; string compare buffer and commandC
	
	cmp dx, 0 
	
	je list_command
	
	
	mov bx, buffer
	mov ax, internal_command_E
	
	int 30h ; string compare buffer and commandE
	
	cmp dx, 0 
	
	je date_command
	
	
	mov bx, buffer
	mov ax, internal_command_F
	
	int 30h ; string compare buffer and commandE
	
	cmp dx, 0 
	
	je pwd_command
	
	mov bx, buffer
	mov ax, internal_command_G
	
	int 30h ; string compare buffer and commandE
	
	cmp dx, 0 
	
	je previous_dir_command
	
	
	mov bx, buffer
	mov ax, internal_command_D
	
	int 30h
	
	cmp dx, 0
	
	je shutdown_command
	
	jmp external_command
	
	
list_command:

		int 23h ; Print newline
		mov bx, current_sector
		mov ah, 00
		mov al, byte[bx]
		int 32h ; List files from file table
		
		
		int 23h
		
		jmp hang
	
external_command:
		
		int 23h ; new line

		mov bx, buffer
		int 31h ; Check if buffer contains a valid filename and load and execute external command.
		int 23h ; new line
		
		jmp hang
		

clear_command:

		; Clear Screen

		int 27h
		
		jmp hang
	
shutdown_command:

	int 33h
	int 23h
	mov ax, poweroff_failed
	mov bx, current_color
	int 21h
    int 23h
	
	jmp hang
	
	
date_command:

	int 23h
	
	
	
	int 34h ; Call date printing ISR
	
	
	int 23h


	jmp hang
	
help_command:

		int 23h ; Print newline

		mov ax, help_msg
		mov bx, current_color
		int 21h ; Print help string

		int 23h ; Print newline
		
		jmp hang
		
previous_dir_command:

mov al, byte[current_sector]
cmp al, 7

je root_dir_error

mov si, previous_pwd
mov di, pwd
mov cx, 64 
rep movsb  ; transfer previous pwd to pwd 

mov bx, previous_sector
xor ax, ax
mov al, byte[bx]
mov bx, current_sector
mov byte[bx], al ; Swap current_sector and previous sector



jmp hang


root_dir_error:

	int 23h 
	mov ax, at_root
	mov bx, current_color
	int 21h
	int 23h
	
	jmp hang

		
pwd_command:

	int 23h ; new line 
	
	mov ax, drive
	int 21h ; print drive letter

	mov ax, pwd 
	int 21h ; print current working directory 
	int 23h ; new line 
	
	jmp hang
		
		
zero_buffer:

	mov ax, buffer
	mov bx, 12
	int 24h	

ret 


get_drive_geometry:

xor ax, ax
mov ah, 8h
int 13h
and cl, 0x3f
mov byte[sectorsPerTrack], cl ; Store sectors per track
sub dh, 1
mov byte[heads], dh ; store number of heads
 
ret 

sectorsPerTrack: db 00
heads: db 00
c dw 00
h dw 00
s dw 00
drive: db 'A:', 0 
arrow: db '>', 0
root: db '\', 0
at_root: db 'Directory does not have a parent directory! Already at root directory!', 0 
dir_found_msg: db 'This is a directory!', 13, 10, 'Would you like to change your current working directory? [y/n]: ', 0
welcome_msg: db 'MonsterOS Version 1.1', 13, 10, 'Copyright (C) 2014 Daniel Lopez. Licensed Under The Simplified BSD License', 13, 10, 0
cli_msg: db 13, 10, 'MonsterOS ', 0
help_msg: db 'Welcome to the MonsterOS shell prompt.', 13, 10, 13, 10, 'MonsterOS shell prompt contains only two internal commands: help, clear, and list.', 13, 10, 'This means that MonsterOS treats everything else that is not the help/clear command as an external command.', 13, 10, 'An external command is basically a filename of a file stored on the disk.', 10, 13, 'If the file is located on the disk it is executed if it is a program; else it is treated a text file and its contents are displayed.', 13, 10, 'If no file is found then a error message is displayed.', 0
internal_command_A: db 'help', 0
internal_command_B: db 'clear', 0
internal_command_C: db 'list', 0
internal_command_D: db 'poweroff', 0
internal_command_E: db 'date', 00
internal_command_F: db 'pwd', 00
internal_command_G: db '..', 00
invalid_cmd: db 'Invalid Internal / External Command!',0
poweroff_failed: db 'Machine failed to shutdown!!!', 13, 10, 'Error: APM May Not Be Supported', 0
current_color: db 001h
current_sector: db 7
previous_sector: db 0
temp: times 9 db 0
previous_pwd: times 64 db 0
pwd: times 64 db 0
buffer: times 64 db 0

times 2560-($-$$) db 0

