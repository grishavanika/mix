add_library(cxxopts INTERFACE)
find_package(cxxopts CONFIG REQUIRED)
target_link_libraries(cxxopts INTERFACE cxxopts::cxxopts)
