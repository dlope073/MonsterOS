[ORG 0x9999] ; All programs get loaded at 0x9999

_start: ; ver.com's entry point label name can be anything really

	mov ax, ver_str
	mov bh, 0
	mov bl, 0xf
	int 21h ; print 
	
ret ; Return back to control

ver_str: db 'MonsterOS Version 1.0', 0

