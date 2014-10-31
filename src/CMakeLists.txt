cmake_minimum_required(VERSION 2.8)
project(tam)

# Set the build type if it isn't already
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS system filesystem regex program_options thread)

# Enable ExternalProject CMake module
include(ExternalProject)

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/ThirdParty)

# Compile Unqlite
ExternalProject_Add(
    Unqlite
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/../ext/unqlite
    INSTALL_COMMAND ""
)

include_directories(${CMAKE_SOURCE_DIR}/../ext)

# rapidjson
ExternalProject_Add(
    rapidjson
    SVN_REPOSITORY "https://github.com/miloyip/rapidjson"
    CONFIGURE_COMMAND "" # skip configure
    BUILD_COMMAND "" # skip build
    INSTALL_COMMAND "" # skip install
    )

include_directories(${CMAKE_BINARY_DIR}/ThirdParty/src/rapidjson/trunk/include)

# Add SoDA
ExternalProject_Add(
    SoDA
    SVN_REPOSITORY https://github.com/sed-szeged/soda/trunk/src
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

add_definitions("-std=c++0x")
include_directories(${source_dir}/lib/SoDA/inc)
include_directories(${source_dir}/lib/SoDAEngine/inc)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

# get all cpp files in libs
file(GLOB tam_SOURCES ${tam_SOURCE_DIR}/gui/*.cpp)
file(GLOB tam_FORMS ${tam_SOURCE_DIR}/gui/*.ui)

QT5_WRAP_UI(tam_FORMS_HEADERS ${tam_FORMS})
QT5_ADD_RESOURCES(tam_RESOURCES ${tam_SOURCE_DIR}/resource.qrc)

aux_source_directory(${tam_SOURCE_DIR}/lib tam_lib)

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

add_executable(test-suite-analysis-and-manipulation ${tam_SOURCES} ${tam_FORMS_HEADERS} ${tam_RESOURCES} ${tam_lib})
target_link_libraries(test-suite-analysis-and-manipulation ${SoDA_LIB} ${SoDAEngine_LIB} ${Boost_LIBRARIES} ${Unqlite_LIB})
qt5_use_modules(test-suite-analysis-and-manipulation WebKitWidgets Widgets)

if (WIN32)
    set(DIRS  ${Boost_LIBRARY_DIR} ${QtLoc} ${CMAKE_BINARY_DIR}/ThirdParty/src/Unqlite-build ${CMAKE_BINARY_DIR}/ThirdParty/src/SoDA-build/lib/SoDA ${CMAKE_BINARY_DIR}/ThirdParty/src/SoDA-build/lib/SoDAEngine)
    set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/test-suite-analysis-and-manipulation.exe" )
    get_target_property(QtLoc Qt5::Widgets LOCATION)
    get_filename_component(QtLoc ${QtLoc} DIRECTORY)

    install(TARGETS test-suite-analysis-and-manipulation DESTINATION bin)
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\"   \"\"   \"${DIRS}\")
        " COMPONENT Runtime)
endif()
