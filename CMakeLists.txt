# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()

set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(FishCare C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

include_directories(
    ${PICO_SDK_PATH}/src/rp2_common/cyw43_arch/include
)

# Add executable. Default name is the project name, version 0.1
add_executable(FishCare
    src/main.c
    # Bibliotecas Copiadas
    inc/ssd1306_i2c.c
    inc/pico_1wire.c
    # Bibliotecas Criadas
    lib/fishcare.c
    lib/oled.c           
    lib/motor.c
    lib/rgb.c
    lib/temperatura.c
    lib/util.c
    lib/wifi.c
    lib/memory.c
)

# Incluir diretórios de cabeçalhos
target_include_directories(FishCare PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    inc                 # Pasta 'inc' para os cabeçalhos
    lib                 # Pasta 'lib' para os cabeçalhos das bibliotecas
)

pico_set_program_name(FishCare "FishCare")
pico_set_program_version(FishCare "0.1")

# Generate PIO header
pico_generate_pio_header(FishCare ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(FishCare 0)
pico_enable_stdio_usb(FishCare 1)

# Habilita SNTP no lwIP
target_compile_definitions(FishCare PRIVATE LWIP_SNTP=1 SNTP_SERVER_DNS=1)

target_sources(FishCare PRIVATE
    ${PICO_SDK_PATH}/lib/lwip/src/apps/sntp/sntp.c
)

# Add the standard library to the build
target_link_libraries(FishCare
    pico_stdlib
    pico_multicore
    hardware_spi
    hardware_i2c
    hardware_pio
    hardware_timer
    hardware_rtc
    hardware_pwm
    pico_cyw43_arch
    pico_cyw43_arch_lwip_threadsafe_background
)

# Add the standard include files to the build
target_include_directories(FishCare PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/inc
    ${CMAKE_CURRENT_LIST_DIR}/lib
)

# Add any user requested libraries
pico_add_extra_outputs(FishCare)
