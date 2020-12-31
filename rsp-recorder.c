#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>

#include <libdragon.h>
#include <rsp.h>

#include "bios.h"

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

typedef uint64_t dword;
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

typedef union flag_result {
    struct {
        half vcc;
        half vco;
        half vce;
        half padding;
    };
    dword packed;
} flag_result_t;
_Static_assert(sizeof(flag_result_t) == sizeof(dword), "flag_result_t should be 64 bits");

struct {
    v128_t zero;

    v128_t arg1;
    v128_t arg2;

    v_result_t result_elements[16];
    flag_result_t flag_elements[16];
} testcase;

_Static_assert(sizeof(testcase) ==
                       (16 * 3) +      // zero, arg1, arg2
                       (16 * 4 * 16) + // 4x res, acch, accm, accl * 16
                       (8 * 16), // 16x flag_result_t (5 bytes with 3 bytes padding)
                       "Testcase blob is expected size");

void print_v128(v128_t* r) {
    printf("%04X%04X%04X%04X%04X%04X%04X%04X",
           r->elements[0], r->elements[1], r->elements[2], r->elements[3],
           r->elements[4], r->elements[5], r->elements[6], r->elements[7]);
}

void print_v128_ln(v128_t* r) {
    print_v128(r);
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

    bi_init();

    console_clear();

    printf("Ready!\n");

    console_render();

    while (1) {
        while (!bi_usb_can_rd()) {
            console_render();
        }
        memset(&testcase, 0, sizeof(testcase));
        bi_usb_rd(&testcase.arg1, sizeof(v128_t));
        while (!bi_usb_can_rd()) {
            console_render();
        }
        bi_usb_rd(&testcase.arg2, sizeof(v128_t));
        print_v128_ln(&testcase.arg1);
        print_v128_ln(&testcase.arg2);

        load_data(&testcase, sizeof(testcase));
        run_ucode();

        while (!broke) {
            console_render();
        }

        read_data(&testcase, sizeof(testcase));
        broke = false;

        printf("Result:\n");
        print_v128_ln(&testcase.result_elements[0].res);
        printf("\n");

        for (int e = 0; e < 16; e++) {
            bi_usb_wr(&testcase.result_elements[e].res, sizeof(v128_t));
            bi_usb_wr(&testcase.result_elements[e].acch, sizeof(v128_t));
            bi_usb_wr(&testcase.result_elements[e].accm, sizeof(v128_t));
            bi_usb_wr(&testcase.result_elements[e].accl, sizeof(v128_t));
            bi_usb_wr(&testcase.flag_elements[e].packed, sizeof(dword));
        }

        console_render();
    }
}
