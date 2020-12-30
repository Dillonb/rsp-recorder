#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>

#include <libdragon.h>
#include <rsp.h>

extern const void __basic_ucode_data_start;
extern const void __basic_ucode_start;
extern const void __basic_ucode_end;

static resolution_t res = RESOLUTION_320x240;
static bitdepth_t bit = DEPTH_32_BPP;

static volatile bool broke = false;

static void sp_handler() {
    broke = true;
}

#define PACKED __attribute__((__packed__))

typedef uint32_t word;
typedef uint16_t half;
typedef uint8_t  byte;

typedef union v128 {
    word words[4];
    half elements[8];
    byte bytes[16];
} v128_t;
_Static_assert(sizeof(v128_t) == sizeof(word) * 4, "v128_t needs to be 128 bits");

typedef struct v_result {
    v128_t res;
    v128_t acch;
    v128_t accm;
    v128_t accl;
} v_result_t;

typedef struct flag_result {
    half vcc;
    half vco;
    byte vce;
} flag_result_t;

struct {
    v128_t zero;

    v128_t arg1;
    v128_t arg2;

    v_result_t result_elements[16];
    flag_result_t flag_elements[16];
} testcase;

void print_vecr(v128_t* r) {
    printf("%04X%04X%04X%04X%04X%04X%04X%04X",
           r->elements[0], r->elements[1], r->elements[2], r->elements[3],
           r->elements[4], r->elements[5], r->elements[6], r->elements[7]);
}

void print_vecr_ln(v128_t* r) {
    print_vecr(r);
    printf("\n");
}

int main(void) {
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init( res, bit, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    console_init();
    console_set_render_mode(RENDER_MANUAL);
    rsp_init();

    /* Attach SP handler and enable interrupt */
    register_SP_handler(&sp_handler);
    set_SP_interrupt(1);

    // Size must be multiple of 8 and start & end must be aligned to 8 bytes
    unsigned long data_size = (unsigned long) (&__basic_ucode_start - &__basic_ucode_data_start);
    unsigned long ucode_size = (unsigned long) (&__basic_ucode_end - &__basic_ucode_start);
    load_data((void*)&__basic_ucode_data_start, data_size);
    load_ucode((void*)&__basic_ucode_start, ucode_size);

    console_clear();

    memset(&testcase, 0, sizeof(testcase));

    testcase.arg1.elements[0] = 0x0001;
    testcase.arg1.elements[1] = 0x0001;
    testcase.arg1.elements[2] = 0x0001;
    testcase.arg1.elements[3] = 0x0001;
    testcase.arg1.elements[4] = 0x0001;
    testcase.arg1.elements[5] = 0x0001;
    testcase.arg1.elements[6] = 0x0001;
    testcase.arg1.elements[7] = 0x0001;

    testcase.arg2.elements[0] = 0x0000;
    testcase.arg2.elements[1] = 0x0001;
    testcase.arg2.elements[2] = 0x0002;
    testcase.arg2.elements[3] = 0x0003;
    testcase.arg2.elements[4] = 0x0004;
    testcase.arg2.elements[5] = 0x0005;
    testcase.arg2.elements[6] = 0x0006;
    testcase.arg2.elements[7] = 0x0007;

    load_data(&testcase, sizeof(testcase));
    run_ucode();

    while(1) {
        bool displayed_results = false;
        if (broke && !displayed_results) {
            read_data(&testcase, sizeof(testcase));
            broke = false;

            print_vecr_ln(&testcase.arg1);
            print_vecr_ln(&testcase.arg2);
            printf("\n");
            for (int e = 0; e < 16; e++) {
                printf("VADD e%d\n", e);
                print_vecr_ln(&testcase.result_elements[e].res);
            }

            displayed_results = true;
        }
        console_render();
    }
}
