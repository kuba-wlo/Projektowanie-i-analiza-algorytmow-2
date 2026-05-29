# Wersja graficzna (Qt 6 Widgets).
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# Qt potrzebuje automatycznego uruchamiania moc (Meta-Object Compiler)
# dla klas z makrem Q_OBJECT (sygnaly/sloty).
set(CMAKE_AUTOMOC ON)

qt_standard_project_setup()

qt_add_executable(tictactoe_gui
    main.cpp
    MainWindow.hpp
    MainWindow.cpp
    BoardWidget.hpp
    BoardWidget.cpp
)

target_link_libraries(tictactoe_gui PRIVATE
    ttt_core
    Qt6::Widgets
)

# Na Windows: aplikacja okienkowa (bez konsoli) w trybie Release.
set_target_properties(tictactoe_gui PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
