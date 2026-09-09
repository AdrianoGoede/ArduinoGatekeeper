cmake_minimum_required(VERSION 3.19)
project(ArduinoGatekeeperAdmin LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(ArduinoGatekeeperAdmin
    WIN32 MACOSX_BUNDLE
    main.cpp
    ui/MainWindow.cpp
    ui/MainWindow.h
    ui/MainWindow.ui
    ui/AuthDialog.h
    ui/AuthDialog.cpp
    ui/AuthDialog.ui
    ui/UsersDialog.h
    ui/UsersDialog.cpp
    ui/UsersDialog.ui
    ui/DoorsDialog.h
    ui/DoorsDialog.cpp
    ui/DoorsDialog.ui
    ui/DashboardDialog.h
    ui/DashboardDialog.cpp
    ui/DashboardDialog.ui
    resources/Icons.qrc
)

target_link_libraries(ArduinoGatekeeperAdmin
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

install(TARGETS ArduinoGatekeeperAdmin
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ArduinoGatekeeperAdmin
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
