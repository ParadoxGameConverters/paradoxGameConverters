# Include required CMake packages
include(ExternalProject)

set(BOOST_BINARY_DIR ${CMAKE_BINARY_DIR}/Dependencies/boost)

# Build Boost
ExternalProject_Add(
    boost
    URL http://downloads.sourceforge.net/project/boost/boost/1.59.0/boost_1_59_0.7z
    URL_HASH SHA1=ea34e49d9e31d6e493620776e30f2cfe31e89b85
    PREFIX ${BOOST_BINARY_DIR}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

set_target_properties(boost PROPERTIES EXCLUDE_FROM_ALL TRUE)
add_definitions(-DBOOST_USE_WINDOWS_H)

ExternalProject_Get_Property(boost source_dir)
set(BOOST_INCLUDE_DIR ${source_dir})
set(BOOST_LIB_DIR ${BOOST_BINARY_DIR}/src/boost/stage/lib)
include_directories("${BOOST_INCLUDE_DIR}")
link_directories("${BOOST_BINARY_DIR}/src/boost/stage/lib")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND WIN32)
    add_custom_target(boost-bootstrap 
       COMMAND bootstrap --with-toolset=clang --with-libraries=filesystem,system
       WORKING_DIRECTORY = ${source_dir}
    )
    add_custom_target(boost-b2 
       COMMAND b2.exe clean && b2.exe link=static toolset=clang cxxflags="-mmmx -std=c++11 -DBOOST_USE_WINDOWS_H" linkflags="-std=c++11" --build-dir=.. --layout=tagged
       WORKING_DIRECTORY = ${source_dir}
    )
else()
    add_custom_target(boost-bootstrap
       COMMAND bootstrap
       WORKING_DIRECTORY = ${source_dir}
    )
    add_custom_target(boost-b2
       COMMAND b2.exe --build-dir=..
       WORKING_DIRECTORY = ${source_dir}
    )
endif()

set_target_properties(boost-bootstrap PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(boost-b2 PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_dependencies(boost-bootstrap boost)
add_dependencies(boost-b2 boost-bootstrap)

# Include directory this file is located on
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Get sources
file(GLOB SRC *.cpp)
file(GLOB CK2World CK2World/*.cpp)
file(GLOB CK2World_Character CK2World/Character/*.cpp)
file(GLOB CK2World_Opinion CK2World/Opinion/*.cpp)
file(GLOB EU3World EU3World/*.cpp)
file(GLOB ModWorld ModWorld/*.cpp)
file(GLOB Common Common/*.cpp)
file(GLOB Parsers Parsers/*.cpp)

# Group sources
source_group("Source Files" FILES ${TEST})
source_group("Source Files\\CK2World" FILES ${CK2World})
source_group("Source Files\\CK2World\\Character" FILES ${CK2World_Character})
source_group("Source Files\\CK2World\\Opinion" FILES ${CK2World_Opinion})
source_group("Source Files\\EU3World" FILES ${EU3World})
source_group("Source Files\\ModWorld" FILES ${ModWorld})
source_group("Source Files\\Common" FILES ${Common})
source_group("Source Files\\Parsers" FILES ${Parsers})

# Make sublibraries
list(REMOVE_ITEM SRC "Configuration.cpp" "Date.cpp" "mappers.cpp" "Log.cpp")
add_library(Common ${Common} Configuration.cpp mappers.cpp Log.cpp)
add_library(Parser ${Parsers})
add_library(CK2ToEU3 ${CK2World} ${CK2World_Character} ${CK2World_Opinion} ${EU3World} ${ModWorld})
target_link_libraries(Parser Common)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND WIN32)
    target_link_libraries(CK2ToEU3 Parser "${BOOST_LIB_DIR}/libboost_filesystem-mt.lib" "${BOOST_LIB_DIR}/libboost_system-mt.lib")
else()
    target_link_libraries(CK2ToEU3 Parser)
endif()

# Make executable
add_executable(
	ConverterApp
	Convert.cpp
	Temp.cpp)

target_link_libraries(ConverterApp CK2ToEU3)

add_custom_command(TARGET CK2ToEU3
	POST_BUILD
	COMMAND Copy_Files ${CMAKE_CURRENT_BINARY_DIR}/Release
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Add "build to archive" target
add_custom_target(build-to-archive EXCLUDE_FROM_ALL)
add_custom_command(TARGET build-to-archive
	POST_BUILD
	COMMAND build_to_archive ${CMAKE_CURRENT_BINARY_DIR}/Release
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

# Add integration testing target
add_custom_target(test-all-configs)
add_custom_command(TARGET test-all-configs
	PRE_BUILD
	COMMAND test_all_configs ${CMAKE_CURRENT_BINARY_DIR}
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})