INCLUDE "hardware.inc"

SECTION "Header", ROM0[$100]

	jp TEST_16BIT_LOADS

	ds $150 - @, 0 ; Make room for the header

TEST_16BIT_LOADS:
	ld bc, $c000
	ld de, $c001
	ld hl, $c002
	ld sp, $fffe
	
EntryPoint:
	ld a, 1
	ld bc, $c000
	ld de, $c001
	ld [bc], a
	ld a, 255
	ld [de], a
	ld hl, $c002
	ld [hl+], a
	ld [hl-], a
	jp Test

Test:
	ld hl, $c00a
	ld [hl], 128
	ld a, [hl+]
