set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS system filesystem regex program_options thread)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_INCLUDE_DIRS})

