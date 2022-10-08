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
