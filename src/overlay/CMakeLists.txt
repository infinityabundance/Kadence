qt_add_executable(kadence-overlay
    main.cpp
)

qt_add_qml_module(kadence-overlay
    URI KadenceOverlay
    VERSION 1.0
    QML_FILES
        qml/Overlay.qml
)

target_compile_features(kadence-overlay PRIVATE cxx_std_20)

target_link_libraries(kadence-overlay
    PRIVATE
        kadence_core
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
)
