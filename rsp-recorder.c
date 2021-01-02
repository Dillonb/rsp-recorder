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

static volatile bool rsp_execution_complete = false;

static void sp_handler() {
    rsp_execution_complete = true;
}

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

word element_instruction(word instruction, int element) {
    return instruction | (element << 21);
}

void load_replacement_ucode(word instruction, int* replacement_indices, unsigned long ucode_size) {
    printf("Loading replacement ucode\n");
    word* ucode = malloc(ucode_size);
    word* uncached_ucode = UncachedAddr(ucode);
    memcpy(uncached_ucode, &__basic_ucode_start, ucode_size);
    for (int i = 0; i < 16; i++) {
        uncached_ucode[replacement_indices[i]] = element_instruction(instruction, i);
    }
    load_ucode(uncached_ucode, ucode_size);
    printf("Done loading.\n");
    free(ucode);
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

    int replacement_indices[16];
    int found = 0;
    word* ucodeptr = (word*)&__basic_ucode_start;
    for (int i = 0; i < (ucode_size / 4); i++) {
        if (ucodeptr[i] == 0xFFFFFFFF) {
            replacement_indices[found++] = i;
        }
    }

    if (found != 16) {
        printf("Didn't find exactly 16 instances, found %d instead. Bad!\n", found);
        while (true) {
            console_render();
        }
    }

    bi_init();

    console_clear();

    printf("Ready!\n");

    console_render();

    word old_instruction = 0xFFFFFFFF;

    while (1) {
        while (!bi_usb_can_rd()) {
            console_render();
        }
        memset(&testcase, 0, sizeof(testcase));
        word instruction;
        bi_usb_rd(&instruction, sizeof(word));
        bi_usb_rd(&testcase.arg1, sizeof(v128_t));
        bi_usb_rd(&testcase.arg2, sizeof(v128_t));

        if (instruction != old_instruction) {
            load_replacement_ucode(instruction, replacement_indices, ucode_size);
            old_instruction = instruction;
        }

        load_data(&testcase, sizeof(testcase));
        run_ucode();

        while (!rsp_execution_complete) {
            console_render();
        }

        read_data(&testcase, sizeof(testcase));
        rsp_execution_complete = false;

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
