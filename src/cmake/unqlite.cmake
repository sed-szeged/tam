# Compile Unqlite
ExternalProject_Add(
    Unqlite
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/../ext/unqlite
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(Unqlite binary_dir)
set(Unqlite_BINARY_DIR ${binary_dir})

include_directories(${CMAKE_SOURCE_DIR}/../ext)

if(APPLE)
    set(Unqlite_LIB ${CMAKE_BINARY_DIR}/ThirdParty/src/Unqlite-build/${CMAKE_FIND_LIBRARY_PREFIXES}Unqlite.dylib)
elseif(WIN32)
    set(Unqlite_LIB ${CMAKE_BINARY_DIR}/ThirdParty/src/Unqlite-build/libUnqlite.dll)
else()
    set(Unqlite_LIB ${CMAKE_BINARY_DIR}/ThirdParty/src/Unqlite-build/${CMAKE_FIND_LIBRARY_PREFIXES}Unqlite.so)
endif()

