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
    ui/UserManagementDialog.h
    ui/UserManagementDialog.cpp
    ui/UserManagementDialog.ui
    core/ODataClient.h
    core/ODataClient.cpp
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

qt_add_resources(ArduinoGatekeeperAdmin "Icons"
    PREFIX "/"
    BASE "resources"
    FILES
        resources/icons/user.svg
        resources/icons/users.svg
        resources/icons/cpu.svg
        resources/icons/pie-chart.svg
        resources/icons/info.svg
        resources/icons/edit.svg
        resources/icons/user-plus.svg
        resources/icons/user-minus.svg
        resources/icons/refresh-cw.svg
        resources/icons/folder.svg
)

install(SCRIPT ${deploy_script})
