add_executable(kadenced
    kadenced.cpp
)

target_compile_features(kadenced PRIVATE cxx_std_20)

target_link_libraries(kadenced
    PRIVATE
        kadence_core
        Qt6::Core
)
