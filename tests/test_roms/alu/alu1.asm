INCLUDE "hardware.inc"

SECTION "Header", ROM0[$100]

	jp TEST

	ds $150 - @, 0

TEST:
	ld a, 0
	ld b, 1
	ld c, 2
	ld d, 3
	ld e, 4
	ld h, 5
	ld l, 6

	add a, b
	add a, c
	add a, d
	add a, e
	add a, h
	add a, l

	ld hl, $c000
	ld [hl], 7
	add a, [hl]
	add a, a
