-- DOESNT ADD A ZERO FOR 11111
=== LIST 0 ===

000010: (swap 1 '0000'..(slice 0))..'0000'
000011: (swap 2 '0000'..(slice 0))..'0000'

=== LIST 1 ===

-- take 0000 11 11 11 11 11 00 to 11
-- and  0000 11 11 10 11 11 00 to 10

-- End case
000000: '11'
00000: '10'
-- Continuing
000011: '0000'
000010: '000'
00010: '000'
00011: '000'

=== LIST 2 ===

-- take 0000 11 11 11 11 11 00 to 10
-- and  0000 11 11 10 11 11 00 to 11

-- End case
000000: '10'
00000: '11'
-- Continuing
000011: '0000'
000010: '000'
00010: '000'
00011: '000'
