INCLUDE "hardware.inc"

SECTION "Header", ROM0[$100]

	jp TEST

	ds $150 - @, 0

TEST:
	ld bc, 1024
	push bc
	ld bc, 0
	pop bc

	ld de, 1024
	push de
	ld de, 0
	pop de

	ld hl, 1024
	push hl
	ld hl, 0
	pop hl

	ld a, 20
	push af
	pop af
