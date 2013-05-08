;Copyright (c) 2013, Daniel Lopez All rights reserved.

;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

;Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



[ORG 0x9999] ; All programs get loaded at 0x9999


_start:

	push es ; save regs in order to dump them on the console
	push ds
	push cs
	push dx
	push cx
	push bx
	push ax
	
	mov ax, regsAX
	int 21h
	
	pop ax
	mov bx, reg_buffer
	int 26h
	
	mov ax, reg_buffer ; print ax 
	int 21h
	
	mov ah, 00eh
	mov al, 9
	int 10h ; print tab
	
	mov ax, regsBX
	int 21h
	
	pop ax
	mov bx, reg_buffer
	int 26h
	
	mov ax, reg_buffer ; print bx
	int 21h
	
	mov ah, 00eh
	mov al, 9
	int 10h ; print tab
	
	mov ax, regsCX
	int 21h
	
	pop ax
	mov bx, reg_buffer
	int 26h
	
	mov ax, reg_buffer ; print cx
	int 21h
	
	mov ah, 00eh
	mov al, 9
	int 10h ; print tab
	
	mov ax, regsDX
	int 21h
	
	pop ax
	mov bx, reg_buffer 
	int 26h
	
	mov ax, reg_buffer ; print dx
	int 21h
	
	mov ah, 00eh
	mov al, 9
	int 10h ; print tab
	
	mov ax, regsCS
	int 21h
	
	pop ax
	mov bx, reg_buffer
	int 26h
	
	mov ax, reg_buffer ; print cs
	int 21h
	
	mov ah, 00eh
	mov al, 9
	int 10h ; print tab
	
	mov ax, regsDS
	int 21h
	
	pop ax
	mov bx, reg_buffer
	int 26h
	
	mov ax, reg_buffer ; print ds
	int 21h
	
	mov ah, 00eh
	mov al, 9
	int 10h ; print tab
	
	mov ax, regsES
	int 21h
	
	pop ax
	mov bx, reg_buffer
	int 26h
	
	mov ax, reg_buffer ; print es
	int 21h
	
	mov ah, 00eh
	mov al, 9
	int 10h ; print tab
	
		
ret 

regsAX: db 'AX: ', 0
regsBX: db 'BX: ', 0
regsCX: db 'CX: ', 0
regsDX: db 'DX: ', 0
regsCS: db 'CS: ', 0
regsDS: db 'DS: ', 0
regsES: db 'ES: ', 0

reg_buffer: times 6 db 0