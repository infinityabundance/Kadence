add_library(kadence_core STATIC
    kadence_core.cpp
)

target_include_directories(kadence_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(kadence_core PUBLIC cxx_std_20)

target_link_libraries(kadence_core
    PUBLIC
        Qt6::Core
)
