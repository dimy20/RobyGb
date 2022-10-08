INCLUDE "hardware.inc"

SECTION "Header", ROM0[$100]

	jp TEST

	ds $150 - @, 0

TEST:
	ld a, 10
	ld bc, $c000
	ld [bc], a
	ld a, 0
	ld a, [bc]

	ld a, 100
	ld de, $c001
	ld [de], a
	ld a, 0
	ld a, [de]

	ld hl, $c002
	ld [hl], 128
	ld a, [hl+]

	ld hl, $c003
	ld [hl], 234
	ld a, [hl-]

	ld a, 255
	ld [$c005], a

	ld a, [$c002]
	
	jp TEST2

TEST2:
	ld a, 10
	ld [$ff00 + 0], a
	ld [$ff00 + 10], a
	ld a, 0
	ld a, [$ff00 + 10]

	ld a, 1
	ld c, 2
	ld [$ff00 + c], a

	ld a, 0
	ld a, [$ff00 + c]
