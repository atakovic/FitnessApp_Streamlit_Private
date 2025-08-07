cmake_minimum_required(VERSION 3.25)
project(FitnessApp)

set(CMAKE_CXX_STANDARD 17)

# GTKmm 4.0 verwenden – pkg-config Integration
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED gtkmm-4.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
add_definitions(${GTKMM_CFLAGS_OTHER})

# Deine Quelldateien
add_executable(FitnessApp
        main.cpp
        1_MyWindow.cpp
        2_Add_Workout.cpp
        3_database.cpp
)

# GTKmm-Bibliotheken beim Linken einbinden
target_link_libraries(FitnessApp ${GTKMM_LIBRARIES})
