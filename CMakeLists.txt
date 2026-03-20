cmake_minimum_required(VERSION 3.10)
project(MRTD_Automatisation_GPIB C)

# Trouver le paquet GTK4
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

# Créer l'exécutable
add_executable(mon_app
    src/main.c
    src/hmi/hmi_ui.c
    src/gpib/gpib_linux/gpib_linux_ctrl.c
    src/gpib/gpib_linux/gpib_linux_write.c
    src/gpib/gpib_linux/gpib_linux_read.c
    src/vision/vision_mrtd.c
)

target_include_directories(
    mon_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Lier GTK4 à l'exécutable (gère les includes et les libs)
target_link_libraries(mon_app PkgConfig::GTK4)