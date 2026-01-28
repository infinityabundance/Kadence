qt_add_executable(kadence-ui
    main.cpp
)

qt_add_qml_module(kadence-ui
    URI Kadence
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

target_compile_features(kadence-ui PRIVATE cxx_std_20)

target_link_libraries(kadence-ui
    PRIVATE
        kadence_core
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
)
