INCLUDE "hardware.inc"

SECTION "Header", ROM0[$100]

	jp EntryPoint

	ds $150 - @, 0 ; Make room for the header

EntryPoint:
	ld a, 10
	ld bc, 300
	ld de, 1024
	ld hl, 10000

	ld [de], a

