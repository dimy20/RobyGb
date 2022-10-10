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
	ld a, 0
	jp TEST2

TEST2:
	ld b, 255
	adc a, b
	ld b, 1
	adc a, b
	ld b, 1
	adc a, b

	ld a, 0
	ld c, 255
	adc a, c
	ld c, 1
	adc a, c
	ld c, 1
	adc a, c
	jp TEST3

TEST3:
	ld b, 10
	ld a, 20
	sub a, b

	ld c, 10
	sub a, c
