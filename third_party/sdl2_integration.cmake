add_library(SDL2_Integrated INTERFACE)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(SDL2_Integrated INTERFACE
    SDL2::SDL2)
