; file ps.as
.entry LENGTH
.extern W
.extern L3
MAIN: mov @r3 ,LENGTH
LOOP: jmp L1
prn -5
bfne W
sub @r1, @r4
mcro acab
bne L3
endmcro
acab

acab
L1: inc K
.entry LOOP
jmp W
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22