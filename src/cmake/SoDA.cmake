# Add SoDA
ExternalProject_Add(
    SoDA
    SVN_REPOSITORY https://github.com/sed-szeged/soda/trunk/src
    SVN_REVISION -r 457
    TIMEOUT 10
    # Force separate output paths for debug and release builds to allow easy
    # identification of correct lib in subsequent TARGET_LINK_LIBRARIES commands
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
               -Dwithoutcl=1
    # Disable install step
    INSTALL_COMMAND ""
    # Wrap download, configure and build steps in a script to log output
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON)

# Specify include dir
ExternalProject_Get_Property(SoDA source_dir)
ExternalProject_Get_Property(SoDA binary_dir)
set(SoDA_BINARY_DIR ${binary_dir})

include_directories(${source_dir}/lib/SoDA/inc)
include_directories(${source_dir}/lib/SoDAEngine/inc)
include_directories(${binary_dir}/ext/src/rapidjson/trunk/include)

if(APPLE)
    set(SoDA_LIB ${binary_dir}/lib/SoDA/${CMAKE_FIND_LIBRARY_PREFIXES}SoDA.dylib)
    set(SoDAEngine_LIB ${binary_dir}/lib/SoDAEngine/${CMAKE_FIND_LIBRARY_PREFIXES}SoDAEngine.dylib)
    set(Unqlite_LIB ${CMAKE_BINARY_DIR}/ThirdParty/src/Unqlite-build/${CMAKE_FIND_LIBRARY_PREFIXES}Unqlite.dylib)
elseif(WIN32)
    set(SoDA_LIB ${binary_dir}/lib/SoDA/libSoDA.dll)
    set(SoDAEngine_LIB ${binary_dir}/lib/SoDAEngine/libSoDAEngine.dll)
    set(Unqlite_LIB ${CMAKE_BINARY_DIR}/ThirdParty/src/Unqlite-build/libUnqlite.dll)
else()
    set(SoDA_LIB ${binary_dir}/lib/SoDA/${CMAKE_FIND_LIBRARY_PREFIXES}SoDA.so)
    set(SoDAEngine_LIB ${binary_dir}/lib/SoDAEngine/${CMAKE_FIND_LIBRARY_PREFIXES}SoDAEngine.so)
    set(Unqlite_LIB ${CMAKE_BINARY_DIR}/ThirdParty/src/Unqlite-build/${CMAKE_FIND_LIBRARY_PREFIXES}Unqlite.so)
endif()

