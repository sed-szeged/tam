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

include(${tam_SOURCE_DIR}/cmake/FindBoost.cmake)
include(${tam_SOURCE_DIR}/cmake/FindQt.cmake)

# Enable ExternalProject CMake module
include(ExternalProject)
# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/ThirdParty)

include(${tam_SOURCE_DIR}/cmake/unqlite.cmake)
include(${tam_SOURCE_DIR}/cmake/SoDA.cmake)

include_directories(${tam_SOURCE_DIR}/lib/inc)
include_directories(${tam_SOURCE_DIR}/gui/inc)

set(APP ${tam_SOURCE_DIR}/app/main.cpp)
#Lib files
file(GLOB_RECURSE lib_headers ${tam_SOURCE_DIR}/lib/inc/*.h)
file(GLOB_RECURSE lib_src ${tam_SOURCE_DIR}/lib/src/*.cpp)

#App files
file(GLOB_RECURSE tam_headers ${tam_SOURCE_DIR}/gui/inc/*.h)
file(GLOB_RECURSE tam_src ${tam_SOURCE_DIR}/gui/src/*.cpp)

file(GLOB tam_forms ${tam_SOURCE_DIR}/gui/ui/*.ui)
qt5_wrap_ui(tam_form_headers ${tam_forms})
qt5_add_resources(tam_rc ${tam_SOURCE_DIR}/resource.qrc)

add_executable(test-suite-analysis-and-manipulation ${APP} ${tam_rc} ${tam_src} ${tam_headers} ${tam_form_headers} ${lib_src} ${lib_headers})
target_link_libraries(test-suite-analysis-and-manipulation ${SoDA_LIB} ${SoDAEngine_LIB} ${Boost_LIBRARIES} ${Unqlite_LIB})
qt5_use_modules(test-suite-analysis-and-manipulation WebKitWidgets Widgets)

if (WIN32)
    set(DIRS  ${Boost_LIBRARY_DIR} ${QtLoc} ${Unqlite_BINARY_DIR} ${SoDA_BINARY_DIR}/lib/SoDA ${SoDA_BINARY_DIR}/lib/SoDAEngine)
    set(APPS "\${CMAKE_INSTALL_PREFIX}/bin/test-suite-analysis-and-manipulation.exe" )
    get_target_property(QtLoc Qt5::Widgets LOCATION)
    get_filename_component(QtLoc ${QtLoc} DIRECTORY)

    install(TARGETS test-suite-analysis-and-manipulation DESTINATION bin)
    install(CODE "
        include(BundleUtilities)
        fixup_bundle(\"${APPS}\"   \"\"   \"${DIRS}\")
        " COMPONENT Runtime)
endif()
