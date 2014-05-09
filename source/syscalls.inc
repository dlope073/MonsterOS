; Copyright (c) 2014, Daniel Lopez
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



;Add another interrupt entry

mov dx, int_lbachs
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

mov dx, int_list_files_at_current_dir
mov [es:32h*4], dx
mov ax, 0
mov [es:32h*4 + 2], ax

; Add another interrupt entry

mov dx, int_shutdown
mov [es:33h*4], dx
mov ax, 0
mov [es:33h*4 + 2], ax


; Add another interrupt entry

mov dx, int_date
mov [es:34h*4], dx
mov ax, 0
mov [es:34h*4 + 2], ax

popa

ret

;-----------------------------------------------------------------
; AX=Sector Of Starting Directory (Root always 5)

int_list_files_at_current_dir:

pusha

	int 29h 

	mov ch, byte[c] ; ch <- cylinder
	mov cl, byte[s] ; cl <- sector
	xor ax, ax
	mov es, ax    ; ES <- 0
	mov dh, byte[h]
	mov dl, 80h ; DH = 0 (head), drive = 80h (0th hard disk)
	mov bx, 0x9e00 ; segment offset of the buffer
	mov ax, 0201h ; AH = 02 (disk read), AL = 01 (number of sectors to read)
	int 13h
	
	mov cx, 0x9e01
	mov dx, 0
_loop___:
	cmp dx, 46
	je _exit____ ; Exit loop because there are not more FILE structure entries
	mov si, cx
	mov al, byte[si]
	cmp al, 0
	je _exit____ ; Exit if null string is found
	mov ax, cx
	int 21h ; Print filename
	int 23h ; Print new line
	add cx, 11
	inc dx
	jmp _loop___

_exit____:	

popa 

iret



;-----------------------------------------------------------------
; Loads Program into memory. Using the filename of the program.
; BX=Contains starting address of null-terminated string 
int_load_program_by_filename:

pusha


	mov ah, 0
	mov al, byte[current_sector]
	int 29h ; lbachs

	
	push bx
	mov ch, byte[c] ; ch <- cylinder
	mov cl, byte[s] ; cl <- sector
	xor ax, ax
	mov es, ax    ; ES <- 0
	mov dh, byte[h]
	mov dl, 80h ; DH = 0 (head), drive = 80h (0th hard disk)
	mov bx, 0x9e00 ; segment offset of the buffer
	mov ax, 0201h ; AH = 02 (disk read), AL = 01 (number of sectors to read)
	int 13h
	pop bx
	
	mov cx, 0x9e01
	mov dx, 0
_loop1___:
	cmp dx, 46
	je  no_program_found; Exit loop because there are no more FILE structure entries
	mov si, cx
	mov al, byte[si]
	cmp al, 0
	je  no_program_found; Exit if null string is found
	mov ax, cx
	


	; Copy a filename to temp as it will be used later
	push cx
	mov di, temp
	mov si, ax
	mov cx, 9
	rep movsb
	pop cx
	
	int 30h
	or dx, dx
	jz found_program
	add cx, 11
	inc dx
	

	
	jmp _loop1___
	
no_program_found:

	mov ax, invalid_cmd
	int 21h
	int 23h
	jmp _exit1____
	

found_program:

	  mov bx, cx
	  cmp byte[bx-1], 0xff

	  
	  je its_a_directory
	  
	  mov bx, cx
	  
	  add bx, 9
	  	  
	  mov ah, 0
	  mov al, byte[bx]
	  int 29h ; lbachs	
	  
	
	  mov ch, byte[c] ; ch <- cylinder
	  mov cl, byte[s] ; cl <- sector
	  xor ax, ax
	  mov es, ax    ; ES <- 0
	  mov dh, byte[h] 
	  mov dl, 80h ; DH = pointed by h (head), drive = 80h (0th hard disk)
	  mov ah, 02h
	  mov al, byte[bx-10]         ; AH = 02 (disk read), AL = At address bx-10 (number of sectors to read)
	  mov bx, 0x9999 ; segment offset of the buffer
     
	 
	  int 13h

	 
	  call 0x9999
	  
	  jmp _exit1____
	  
its_a_directory:

	push bx
	mov bx, current_sector
	xor ax, ax
	mov al, byte[bx]
	mov bx, previous_sector
	mov byte[bx], al ; Swap previous_sector and current_sector
	pop bx

	mov bx, cx
	mov dl, byte[bx+9]
	mov bh, 00
	mov bl, dl
	mov byte[current_sector], bl ; Update current sector 

	call zero_buffer
	mov ax, dir_found_msg
	int 21h 
	mov bx, buffer
    int 22h ; Read From Keyboard  And Store String In Buffer
	cmp byte[bx], 'n'
	je _exit1____
	cmp byte[bx], 'y'	
	jne _exit1____
	
change_cwd:

	 mov si, pwd
	 mov di, previous_pwd
 	 mov cx, 64 
	 rep movsb  ; transfer  pwd to previous pwd 

	 mov bx, pwd ; bx <- pwd
	 mov byte[bx], '\' ; Add path sep char
	 inc bx
	 mov di, bx ; pwd destination
	 mov si, temp  ; temp source
	 mov cx, 9
	 rep movsb ; transfer temp to pwd
	 

	
_exit1____:
	
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
je doit ; dx == cx

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
; Clears the screen -- it requires no arguments 


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

; ISR to Shutdown machine

int_shutdown:

pusha ; Save regs

;Connect to APM API
mov     ax,5301
xor     bx,bx
int     15

;Try to set APM version (to 1.2)
mov     ax, 530eh
xor     bx,bx
mov     cx,0102h
int     15h

;Turn off the system
mov     ax,5307h
mov     bx,0001h
mov     cx,0003h
int     15h

popa ; Restore regs

iret

;------------------------------------------------------------------------------------------------------
; No parameters -- Reads RTC to get current date 

int_date:

pusha

xor ax, ax
mov ah, 04h
int 1ah

jc done

mov ah, 00
mov al, dh

int 26h ; Convert month to string

mov ax, bx
mov bx, 01h

int 21h ; Print month

mov ah, 0Eh
mov al, '/'
int 0x10 ; Print Date Separator

mov ah, 00 
mov al, dl

int 26h ; Convert day to string

mov ax, bx
mov bx, 01h

int 21h ; Print day

mov ah, 0Eh
mov al, '/'
int 0x10 ; Print Date Separator

mov ax, cx

int 26h ; Convert year to string

mov ax, bx
mov bx, 01h

int 21h ; Print year


popa

done:

iret

;--------------------------------------------------------------------------------------------------------------------------------------------------------
; Converts LBA to CHS and stores the CHS result in memory



int_lbachs:

pusha

 push dx			; save the value in dx
 xor dx,dx		; zero dx
 mov bx, [sectorsPerTrack]	; move into place stp (lba all ready in place)
 div bx			; make the divide (ax/bx -> ax,dx)
 inc dx			; add one to the remainder (sector value)
 push dx			; save the sector value on the stack

 xor dx,dx		; zero dx
 mov bx, [heads]	; move numheads into place (numtracks all ready in place)
 div bx			; make the divide (ax/bx -> ax,dx)

 mov cx,ax		; move ax to cx (cylinder)
 mov bx,dx		; move dx to bx (head)
 pop ax			; take the last value entered on the stack off.
			; it doesn't need to go into the same register.
			; (sector)
 pop dx			; restore dx, just in case something important was
			; originally in there before running this.
mov word[c], cx
mov word[h], bx
mov word[s], ax			
	
popa 

 
 iret


