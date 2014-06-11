cmake_minimum_required(VERSION 2.8)
project(tam)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS system filesystem regex program_options)

# Set the build type if it isn't already
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Enable ExternalProject CMake module
include(ExternalProject)

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/ThirdParty)

ExternalProject_Add(
    rapidjson
    SVN_REPOSITORY "http://rapidjson.googlecode.com/svn/trunk"
    UPDATE_COMMAND "" # skip update
    CONFIGURE_COMMAND "" # skip configure
    BUILD_COMMAND "" # skip build
    INSTALL_COMMAND "" # skip install
    )

# set(RAPIDJSON_INCLUDE_DIRS ${CMAKE_BINARY_DIR}/ThirdParty/src/rapidjson/include)
include_directories(${CMAKE_BINARY_DIR}/ThirdParty/src/rapidjson/include)

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

include_directories(${source_dir}/lib/SoDA/inc)
include_directories(${source_dir}/lib/SoDAEngine/inc)

include_directories(${Qt5Widgets_INCLUDE_DIRS})

# get all cpp files in libs
file(GLOB tam_SOURCES ${tam_SOURCE_DIR}/gui/*.cpp)
file(GLOB tam_FORMS ${tam_SOURCE_DIR}/gui/*.ui)

QT5_WRAP_UI(tam_FORMS_HEADERS ${tam_FORMS})

# aux_source_directory(${tam_SOURCE_DIR}/lib tam_lib)

add_executable(test-suite-analysis-and-manipulation ${tam_SOURCES} ${tam_FORMS_HEADERS} ${tam_SOURCE_DIR}/lib/CWorkspace.cpp ${tam_SOURCE_DIR}/lib/CWorkspace.h)
target_link_libraries(test-suite-analysis-and-manipulation ${binary_dir}/lib/SoDA/${CMAKE_FIND_LIBRARY_PREFIXES}SoDA.so ${Boost_LIBRARIES})
qt5_use_modules(test-suite-analysis-and-manipulation Widgets)
