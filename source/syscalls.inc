; Copyright (c) 2013, Daniel Lopez
;All rights reserved.
;
;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
;
;Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
;Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



; Initialize Interrupts
init_int:

pusha

xor ax, ax
mov es, ax ; es = 0

; Add the interrupt entry 

mov dx, int_print 
mov [es:21h*4], dx
mov ax, 0
mov [es:21h*4 + 2], ax

; Add another interrupt entry

mov dx, int_block_input_echo 
mov [es:22h*4], dx
mov ax, 0
mov [es:22h*4 + 2], ax

; Add another interrupt entry

mov dx, int_print_newline 
mov [es:23h*4], dx
mov ax, 0
mov [es:23h*4 + 2], ax

; Add another interrupt entry

mov dx, int_zero_buffer 
mov [es:24h*4], dx
mov ax, 0
mov [es:24h*4 + 2], ax

; Add another interrupt entry

mov dx, int_string_length 
mov [es:25h*4], dx
mov ax, 0
mov [es:25h*4 + 2], ax

; Add another interrupt entry

mov dx, int_integer_to_string
mov [es:26h*4], dx
mov ax, 0
mov [es:26h*4 + 2], ax

; Add another interrupt entry

mov dx, int_clear_screen
mov [es:27h*4], dx
mov ax, 0
mov [es:27h*4 + 2], ax

; Add another interrupt entry

mov dx, int_check_file_struct
mov [es:28h*4], dx
mov ax, 0
mov [es:28h*4 + 2], ax

; Add another interrupt entry

mov dx, int_write_file_struct
mov [es:29h*4], dx
mov ax, 0
mov [es:29h*4 + 2], ax

; Add another interrupt entry

mov dx, int_string_compare
mov [es:30h*4], dx
mov ax, 0
mov [es:30h*4 + 2], ax

; Add another interrupt entry

mov dx, int_load_program_by_filename
mov [es:31h*4], dx
mov ax, 0
mov [es:31h*4 + 2], ax

; Add another interrupt entry

mov dx, int_list_files_from_file_table
mov [es:32h*4], dx
mov ax, 0
mov [es:32h*4 + 2], ax

popa

ret

;-----------------------------------------------------------------
;Prints each file name from the file table located at 0x9e00

int_list_files_from_file_table:

pusha
	
	mov cx, 0x9e01
	mov dx, 0
_loop___:
	cmp dx, 5
	je _exit____ ; Exit loop because there are not more FILE structure entries
	mov si, cx
	mov al, byte[si]
	cmp al, 0
	je _exit____ ; Exit if null string is found
	mov ax, cx
	int 21h ; Print filename
	int 23h ; Print new line
	add cx, 0x200
	inc dx
	jmp _loop___

_exit____:	

popa 

iret

;-----------------------------------------------------------------
; Loads Program into memory. Using the filename of the program.
; AX=Contains starting address of null-terminated string
;
int_load_program_by_filename:

pusha

	mov cx, 0x9e01
	mov dx, 0
_loop__:
	cmp dx, 10
	je no_program_found ; Exit loop because there are no more FILE structure entries in the file table
	mov si, cx
	cmp byte[si], 0
	je no_program_found ; Exit if null string is found
	mov bx, cx ; bx <- cx
	int 30h
	or dx, dx
	jz found_program ; If strings match then jump to found_program and start executing the program from memory
	add cx, 0x200
	inc dx
	jmp _loop__
	
no_program_found:

	mov ax, invalid_cmd
	mov bx, 000fh
	int 21h ; Print invalid command message

jmp _exit___
	
found_program:
	add cx, 13
	mov si, cx
	cmp byte[si], 0
	je exec ; if al == 0 then file is program needs to be executed
	
	inc cx
	mov ax, cx
	int 21h
	
	jmp _exit___
	
exec:
	copy_prog_in_mem: ; Copies program into 0x9999 because that is where all programs are loaded and executed
			inc cx
			mov bx, cx ; bx <- cx because cx cannot be used for this operation
			mov di, 0x9999 ; Destination index is 0x9999 because that is where the program will be loaded
			mov dx, 0 ; our counter
			looop:
			cmp dx, 497 ; 497 because 497 is the maximum number of bytes a program can be. Remember each file table entry occupies 512 bytes
			je _call ; if dx = 497 then the program has been fully copied to the region in memory where it will be executed
			mov  si, [bx] ; copy contents at address bx into source index
			mov [di], si ; Move contents at source index and store it into the memory address at destination index
			inc dx ; increment dx
			inc di ; increment destination index
			inc bx ; increment bx
			jmp looop
	_call:
			call 0x9999 
_exit___:
	
popa


iret

;-----------------------------------------------------------------
; Compares two strings
; AX=Address of String1 BX=Address of String2 
; Returns DX=0 equal any other value means not equal
int_string_compare:

push cx ; save cx

int 25h ; Get String Length of String1
mov dx, cx
push ax ; save ax
mov ax, bx
int 25h
pop ax
cmp dx, cx
jg doit ; dx > cx
mov cx, dx

doit:
	mov si, ax
	mov di, bx
	rep cmpsb
	je identical

	mov dx, 1
	jmp finish_it
identical:
	mov dx, 0
finish_it:

pop cx ; restore cx

iret

;-----------------------------------------------------------------
; Checks To See If An Existing FILE structure exists on a specific sector on the disk
; Params: AX = Sector To Check If FILE 
; Returns The Status In AX. If AX is zero there is an existing FILE structure currently on the disk.
; Any other value would mean it is safe to write a FILE structure at that sector
int_check_file_struct:

push cx
push es
push dx
push bx

mov cx, ax    ; Sector ax
xor ax, ax
mov es, ax    ; ES <- 0
mov dx, 0080h ; DH = 0 (head), drive = 80h (0th hard disk)
mov bx, 2000h ; segment offset of the buffer
mov ax, 0201h ; AH = 02 (disk read), AL = 01 (number of sectors to read)
int 13h

mov cx, [bx]
cmp cx, 0x69
je file_exists

mov ax, 1

jmp __exit ; File doesnt exist

file_exists:

xor ax, ax

__exit:


pop bx
pop dx
pop es
pop cx

iret

;-----------------------------------------------------------------
; Writes A FILE structure on the disk. 
; Params: BX=Starting Address of FILE structure CX=Sector To Write
; Returns AX= -1 if failed to write 
int_write_file_struct:

pusha

	mov ax, bx
	int 28h
	or ax, ax
	jz _error_
	
	xor ax, ax
	mov es, ax    ; ES <- 0
	mov dx, 0080h ; DH = 0 (head), drive = 80h (0th hard disk)
	mov ax, 0301h ; AH = 03 (disk write), AL = 01 (number of sectors to write)
	int 13h
	
	jmp _____exit
	
_error_:

	mov ax, -1

_____exit:
	
popa

iret 

;-----------------------------------------------------------------

int_clear_screen:

pusha
		mov ah, 0
		mov al, 3h
		int 10h

popa

iret

;-----------------------------------------------------------------

;Converts an integer into a string
;Params: ax has the integer value, bx has the pointer to the string buffer
;Returns nothing
;This is an ISR

int_integer_to_string:

   push dx
   push cx
   push bx
   push ax
   
   mov di, bx
   
   mov bx, 10               ; base of the decimal system
   xor cx, cx               ; number of digits generated
os_int_to_string_next_divide:
   xor dx, dx               ; RAX extended to (RDX,RAX)
   div bx                  ; divide by the number-base
   push dx               ; save remainder on the stack
   inc cx                  ; and count this remainder
   cmp ax, 0               ; was the quotient zero?
   jne os_int_to_string_next_divide      ; no, do another division

os_int_to_string_next_digit:
   pop ax                  ; else pop recent remainder
   add al, '0'               ; and convert to a numeral
   stosb                  ; store to memory-buffer
   loop os_int_to_string_next_digit      ; again for other remainders
   xor al, al
   stosb                  ; Store the null terminator at the end of the string

   pop ax
   pop bx
   pop cx
   pop dx
   
   iret
   
   
   
; -----------------------------------------------------------------------------

;Computes String (Null Terminated) Length
;Params: Starting Address Stored In AX
;Returns Length  At CX
;This is an ISR
int_string_length:

push ax ; Save
	mov si, ax
	mov cx, 0
	loopy:
	
			lodsb
			or al, al
			jz exity
			inc cx
			jmp loopy
	exity:
pop ax ; Restore

iret

;----------------------------------------------------------------------------------

; Fills the buffer with zeroes.
; Params: Starting Address of Buffer Stored In AX, BX Contains String Length
; This is a ISR no direct invoking

int_zero_buffer:

pusha

	mov di, ax
    mov cx, 0
	
	l:
	mov al, 0
	stosb
	cmp cx, bx
	je e ; if cx == bx goto e
	inc cx ; cx++
	jmp l ; Go back to the beginning of the loop
	
	e:

popa

iret

;---------------------------------------------------------------------------------------------------------

; Reads Each Character Until Enter Has Been Pressed. This ISR Can ECHO each Character
; Params: AX Starting Address Of String
; This is a ISR no direct invoking
int_block_input_echo:

	pusha
	
	mov di, bx
	mov cx, 0
	
	_again_:

	cmp cx, 12 ; Prompt cannot accept more than 12 characters
	je _max_reached ; We jump to end if cx equals to 12
	
	mov ah, 0
	int 0x16
	cmp al, 0x8
	jne print
	cmp cx, 0
	je _again_ ; Ignore print backspace if cursor is already at the beginning
print:
	mov ah, 0Eh ; ah = 0x0e
	int 0x10 ; print character
	cmp al, 0x8 ; compare al with backspace
	je _ignore ; If backspace we ignore it and avoid adding into the buffer.
	cmp al, 0xD ; compare with line feed
	je _end_ ; if al='\r' break from loop
	stosb ; Store Character Byte At AL Into [ES:DI] Then INCREMENTS DI
	inc cx ; cx++
	jmp _again_ ; else go back to the beginning of the loop
	_ignore: 
	dec cx ; cx-- because we pressed backspace
	cmp di, bx
	je _again_ ; if (di == buffer) we do not want to decrement di 
	dec di
	jmp _again_ ; else go back to the beginning of the loop
	

	
	_max_reached:
	
	mov ah, 0Eh ; ah = 0x0e
	mov al, 0xD
	int 0x10 ; print character
	
	_end_:	
	
	mov al, 0
	stosb
	
    mov ah, 0Eh
	mov al, 10
	int 0x10 ; Print New Line
	 
	popa

iret 

;--------------------------------------------------------------------------------------------------------------------------

;Prints a single new line character
; This is a ISR
int_print_newline:

pusha

		mov ah, 0Eh
		mov al, 13
		int 0x10 ; Print New Line

		mov ah, 0Eh
		mov al, 10
		int 0x10 ; Print New Line
		
popa

iret

; Prints a null terminated string on the screen
; This is ISR (Interrupt Service Routine)
int_print:
	pusha ; Store all working registers on the stack!
	mov si, ax

	.loop:
			lodsb
			or al, al
			jz .endloop ; If character is a null character aka 0 then end the loop			
			mov ah, 0Eh ; ah = 0x0e
			int 0x10 ; print character
			jmp .loop ; Go back to the beginning of the loop
	.endloop:
	popa ; Pop all the saved registers on the stack and restore them!
iret ; Return from interrupt

;------------------------------------------------------------------------------------------------------