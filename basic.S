#include <ucode.S>
.text

lqv $v00, 0, 0, $zero

// Load first argument
la $2, arg1
lqv $v01, 0, 0, $2

// Load second argument
la $2, arg2
lqv $v02, 0, 0, $2

// Run instruction once for each broadcast modifier
vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 0
la $2, res_e0
la $3, acc_h_e0
la $4, acc_m_e0
la $5, acc_l_e0
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 1
la $2, res_e1
la $3, acc_h_e1
la $4, acc_m_e1
la $5, acc_l_e1
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 2
la $2, res_e2
la $3, acc_h_e2
la $4, acc_m_e2
la $5, acc_l_e2
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 3
la $2, res_e3
la $3, acc_h_e3
la $4, acc_m_e3
la $5, acc_l_e3
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 4
la $2, res_e4
la $3, acc_h_e4
la $4, acc_m_e4
la $5, acc_l_e4
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 5
la $2, res_e5
la $3, acc_h_e5
la $4, acc_m_e5
la $5, acc_l_e5
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 6
la $2, res_e6
la $3, acc_h_e6
la $4, acc_m_e6
la $5, acc_l_e6
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 7
la $2, res_e7
la $3, acc_h_e7
la $4, acc_m_e7
la $5, acc_l_e7
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 8
la $2, res_e8
la $3, acc_h_e8
la $4, acc_m_e8
la $5, acc_l_e8
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 9
la $2, res_e9
la $3, acc_h_e9
la $4, acc_m_e9
la $5, acc_l_e9
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 10
la $2, res_e10
la $3, acc_h_e10
la $4, acc_m_e10
la $5, acc_l_e10
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 11
la $2, res_e11
la $3, acc_h_e11
la $4, acc_m_e11
la $5, acc_l_e11
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 12
la $2, res_e12
la $3, acc_h_e12
la $4, acc_m_e12
la $5, acc_l_e12
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 13
la $2, res_e13
la $3, acc_h_e13
la $4, acc_m_e13
la $5, acc_l_e13
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 14
la $2, res_e14
la $3, acc_h_e14
la $4, acc_m_e14
la $5, acc_l_e14
jal saveResults
nop

vmudm $v00, $v00, $v00, 0 // Clear accumulator
vadd $v03, $v01, $v02, 15
la $2, res_e15
la $3, acc_h_e15
la $4, acc_m_e15
la $5, acc_l_e15
jal saveResults
nop

break

deadloop:
	j deadloop
	nop

saveResults:
    sqv $v03, 0, 0, $2
    // acc_h
    vsar $v03, $v00, $v00, 0x7
    sqv $v03, 0, 0, $3
    // acc_m
    vsar $v03, $v00, $v00, 0x9
    sqv $v03, 0, 0, $4
    // acc_l
    vsar $v03, $v00, $v00, 0xA
    sqv $v03, 0, 0, $5
    jr $ra
    nop

.data
zero:  .word 0,0,0,0
arg1:  .word 0,0,0,0
arg2:  .word 0,0,0,0

res_e0:   .word 0,0,0,0
acc_h_e0: .word 0,0,0,0
acc_m_e0: .word 0,0,0,0
acc_l_e0: .word 0,0,0,0

res_e1:   .word 0,0,0,0
acc_h_e1: .word 0,0,0,0
acc_m_e1: .word 0,0,0,0
acc_l_e1: .word 0,0,0,0

res_e2:   .word 0,0,0,0
acc_h_e2: .word 0,0,0,0
acc_m_e2: .word 0,0,0,0
acc_l_e2: .word 0,0,0,0

res_e3:   .word 0,0,0,0
acc_h_e3: .word 0,0,0,0
acc_m_e3: .word 0,0,0,0
acc_l_e3: .word 0,0,0,0

res_e4:   .word 0,0,0,0
acc_h_e4: .word 0,0,0,0
acc_m_e4: .word 0,0,0,0
acc_l_e4: .word 0,0,0,0

res_e5:   .word 0,0,0,0
acc_h_e5: .word 0,0,0,0
acc_m_e5: .word 0,0,0,0
acc_l_e5: .word 0,0,0,0

res_e6:   .word 0,0,0,0
acc_h_e6: .word 0,0,0,0
acc_m_e6: .word 0,0,0,0
acc_l_e6: .word 0,0,0,0

res_e7:   .word 0,0,0,0
acc_h_e7: .word 0,0,0,0
acc_m_e7: .word 0,0,0,0
acc_l_e7: .word 0,0,0,0

res_e8:   .word 0,0,0,0
acc_h_e8: .word 0,0,0,0
acc_m_e8: .word 0,0,0,0
acc_l_e8: .word 0,0,0,0

res_e9:   .word 0,0,0,0
acc_h_e9: .word 0,0,0,0
acc_m_e9: .word 0,0,0,0
acc_l_e9: .word 0,0,0,0

res_e10:   .word 0,0,0,0
acc_h_e10: .word 1,0,0,0
acc_m_e10: .word 1,0,0,0
acc_l_e10: .word 1,0,0,0

res_e11:   .word 0,0,0,0
acc_h_e11: .word 1,0,0,0
acc_m_e11: .word 1,0,0,0
acc_l_e11: .word 1,0,0,0

res_e12:   .word 0,0,0,0
acc_h_e12: .word 1,0,0,0
acc_m_e12: .word 1,0,0,0
acc_l_e12: .word 1,0,0,0

res_e13:   .word 0,0,0,0
acc_h_e13: .word 1,0,0,0
acc_m_e13: .word 1,0,0,0
acc_l_e13: .word 1,0,0,0

res_e14:   .word 0,0,0,0
acc_h_e14: .word 1,0,0,0
acc_m_e14: .word 1,0,0,0
acc_l_e14: .word 1,0,0,0

res_e15:   .word 0,0,0,0
acc_h_e15: .word 1,0,0,0
acc_m_e15: .word 1,0,0,0
acc_l_e15: .word 1,0,0,0

vcc_e0: .half 0
vco_e0: .half 0
vce_e0: .half 0
pad_e0: .half 0

vcc_e1: .half 0
vco_e1: .half 0
vce_e1: .half 0
pad_e1: .half 0

vcc_e2: .half 0
vco_e2: .half 0
vce_e2: .half 0
pad_e2: .half 0

vcc_e3: .half 0
vco_e3: .half 0
vce_e3: .half 0
pad_e3: .half 0

vcc_e4: .half 0
vco_e4: .half 0
vce_e4: .half 0
pad_e4: .half 0

vcc_e5: .half 0
vco_e5: .half 0
vce_e5: .half 0
pad_e5: .half 0

vcc_e6: .half 0
vco_e6: .half 0
vce_e6: .half 0
pad_e6: .half 0

vcc_e7: .half 0
vco_e7: .half 0
vce_e7: .half 0
pad_e7: .half 0

vcc_e8: .half 0
vco_e8: .half 0
vce_e8: .half 0
pad_e8: .half 0

vcc_e9: .half 0
vco_e9: .half 0
vce_e9: .half 0
pad_e9: .half 0

vcc_e10: .half 0
vco_e10: .half 0
vce_e10: .half 0
pad_e10: .half 0

vcc_e11: .half 0
vco_e11: .half 0
vce_e11: .half 0
pad_e11: .half 0

vcc_e12: .half 0
vco_e12: .half 0
vce_e12: .half 0
pad_e12: .half 0

vcc_e13: .half 0
vco_e13: .half 0
vce_e13: .half 0
pad_e13: .half 0

vcc_e14: .half 0
vco_e14: .half 0
vce_e14: .half 0
pad_e14: .half 0

vcc_e15: .half 0
vco_e15: .half 0
vce_e15: .half 0
pad_e15: .half 0