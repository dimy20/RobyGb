INCLUDE "hardware.inc"

SECTION "Header", ROM0[$100]

	jp EntryPoint

	ds $150 - @, 0 ; Make room for the header

EntryPoint:
	; Shut down audio circuitry
	ld b, 1
	ld d, 23
	ld h, 4
	ld c, 192
	ld e, 55
	ld l, 32
	ld a, 3

	ld b, b
	ld b, c
	ld b, d
	ld b, e
	ld b, h
	ld b, l
	ld b, a

	ld c, b
	ld c, c
	ld c, d
	ld c, e
	ld c, h
	ld c, l
	ld c, a

	ld d, b
	ld d, c
	ld d, d
	ld d, e
	ld d, h
	ld d, l
	ld d, a

	ld e, b
	ld e, c
	ld e, d
	ld e, e
	ld e, h
	ld e, l
	ld e, a

	ld h, b
	ld h, c
	ld h, d
	ld h, e
	ld h, h
	ld h, l
	ld h, a

	ld l, b
	ld l, c
	ld l, d
	ld l, e
	ld l, h
	ld l, l
	ld l, a

	ld a, b
	ld a, c
	ld a, d
	ld a, e
	ld a, h
	ld a, l
	ld a, a

	ld a, 1
	ld bc, $c000
	ld de, $c001
	ld [bc], a
	ld a, 255
	ld [de], a
	ld hl, $c002
	ld [hl+], a
	ld [hl-], a
	ld hl, $c005
	ld [hl], 10
