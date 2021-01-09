cmake_minimum_required(VERSION 3.13)
project(rsp-recorder)
enable_language(C ASM)

# Setup toolchain
set(tools /usr)
set(CMAKE_C_COMPILER ${tools}/bin/mips64-elf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/mips64-elf-g++)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_OBJCOPY ${tools}/bin/mips64-elf-objcopy)
set(N64TOOL ${tools}/bin/n64tool)
set(CHKSUM64 ${tools}/bin/chksum64)
include_directories(${tools}/mips64-elf/include)
link_directories(${tools}/mips64-elf/lib)
add_compile_options(-std=gnu99 -march=vr4300 -mtune=vr4300)

# Build RSP microcode
add_custom_command(OUTPUT rsp.o
        COMMAND ${CMAKE_C_COMPILER} -c -o rsp.o ${CMAKE_CURRENT_SOURCE_DIR}/rsp.S
        MAIN_DEPENDENCY rsp.S)
add_custom_command(OUTPUT text.section.bin data.section.bin
        COMMAND ${CMAKE_OBJCOPY} --dump-section .text=text.section.bin --dump-section .data=data.section.bin rsp.o
        MAIN_DEPENDENCY rsp.o)

# Build and link main code to ELF format
SET_SOURCE_FILES_PROPERTIES(loader.s OBJECT_DEPENDS "text.section.bin")
add_executable(rsp-recorder.elf rsp-recorder.c sys.c sys.h bios.c bios.h loader.s)
target_compile_options(rsp-recorder.elf PUBLIC -Wall -Werror)
target_link_libraries(rsp-recorder.elf PUBLIC dragon c m dragonsys)
target_link_options(rsp-recorder.elf PUBLIC -Tn64.ld)

# Get just the code binary format
add_custom_command(OUTPUT rsp-recorder.bin
        COMMAND ${CMAKE_OBJCOPY} rsp-recorder.elf rsp-recorder.bin -O binary
        MAIN_DEPENDENCY rsp-recorder.elf)

# Turn it into a ROM
set(N64_HEADER ${tools}/mips64-elf/lib/header)
set(N64_FLAGS -l 2M -h ${N64_HEADER})
add_custom_command(OUTPUT rsp-recorder.z64
        COMMAND ${N64TOOL} ${N64_FLAGS} -o rsp-recorder.z64 -t "RSP Recorder" rsp-recorder.bin && pwd && ${CHKSUM64} rsp-recorder.z64
        MAIN_DEPENDENCY rsp-recorder.bin)

add_custom_target(TARGET-rsp-recorder.z64 ALL DEPENDS rsp-recorder.z64)