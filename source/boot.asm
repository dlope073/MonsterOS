; Copyright (c) 2013, Daniel Lopez
;All rights reserved.
;
;Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
;
;Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
;Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
;THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

; Boot Loader

[ORG 0x7c00] ; BIOS loads at 0x7c00 in memory


xor ax, ax ; make it zero
mov ds, ax ; Data segment is zero

;Set desired video mode (Graphics Mode)
mov ah, 0
mov al, 3h
int 10h

;Load OS from DISK at 0x7e00

xor ax, ax
mov es, ax    ; ES <- 0
mov cx, 2     ; cylinder 0, sector 2
mov dx, 0080h ; DH = 0 (head), drive = 80h (0th hard disk)
mov bx, 7e00h ; segment offset of the buffer
mov ax, 0204h ; AH = 02 (disk read), AL = 04 (number of sectors to read)
int 13h


;Load list of FILE structures off of disk starting at sector 4

xor ax, ax
mov es, ax    ; ES <- 0
mov cx, 6     ; cylinder 0, sector 6
mov dx, 0080h ; DH = 0 (head), drive = 80h (0th hard disk)
mov bx, 0x9e00 ; segment offset of the buffer
mov ax, 020Ah ; AH = 02 (disk read), AL = 10 (number of sectors to read)
int 13h


jmp 0h:0x7e00 ; Jump To OS


times 510-($-$$) db 0
   db 0x55
   db 0xAA