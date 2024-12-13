set(COMPONENT_SRCDIRS
    "src"
    "src/entities"
    "src/modules"
    "src/services"
   
)
set(COMPONENT_ADD_INCLUDEDIRS
    "src"
)
set(COMPONENT_REQUIRES
    nvs_flash
    mbedtls
    driver
    esp_event
    esp_timer
)
file(GLOB_RECURSE COMPONENT_SRCS "src/*.cpp")

register_component()

if (IDF_VER STRGREATER_EQUAL "v4.0")
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-expansion-to-defined)
endif()

