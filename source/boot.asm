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
mov ax, 0205h ; AH = 02 (disk read), AL = 5 (number of sectors to read)
int 13h


jmp 0h:0x7e00 ; Jump To OS


times 510-($-$$) db 0
   db 0x55
   db 0xAA