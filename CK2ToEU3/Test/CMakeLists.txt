# We need thread support
find_package(Threads REQUIRED)

# Prepare test framework
if (MSVC11)
    get_filename_component(VS_DIR [HKEY_CURRENT_USER\\Software\\Microsoft\\VisualStudio\\11.0_Config\\Setup\\VS;ProductDir] REALPATH CACHE)
elseif (MSVC10)
    get_filename_component(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\10.0\\Setup\\VS;ProductDir] REALPATH CACHE)
elseif (MSVC90)
    get_filename_component(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0\\Setup\\VS;ProductDir] REALPATH CACHE)
elseif (MSVC80)
    get_filename_component(VS_DIR [HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\8.0\\Setup\\VS;ProductDir] REALPATH CACHE)
endif()

include_directories("${VS_DIR}/VC/UnitTest/include")

# Setup platform-specific configuration

if (MSVC)
	message(STATUS "The Visual Studio install directory is: " ${VS_DIR})
endif()

if (WIN32)
    set(DISABLE_PTHREADS 1)
else()
    set(DISABLE_PTHREADS 0)
endif()

##########################################################

# Include test dependencies

include(ExternalProject)

#	gmock

if(MSVC_VERSION GREATER 1600)

	set(GMOCK_BINARY_DIR ${CMAKE_BINARY_DIR}/Dependencies/gmock)
	ExternalProject_Add(
	    gmock
	    GIT_REPOSITORY https://github.com/google/googletest.git
	    PREFIX ${GMOCK_BINARY_DIR}
	    CONFIGURE_COMMAND ""
	    BUILD_COMMAND ${CMAKE_CXX_COMPILER} /c /EHsc /MD /I ${GMOCK_BINARY_DIR}/src/gmock/googletest/include /I ${GMOCK_BINARY_DIR}/src/gmock/googletest /I ${GMOCK_BINARY_DIR}/src/gmock/googlemock/include /I ${GMOCK_BINARY_DIR}/src/gmock/googlemock /D_VARIADIC_MAX=10 ${GMOCK_BINARY_DIR}/src/gmock/googlemock/src/gmock-all.cc ${GMOCK_BINARY_DIR}/src/gmock/googletest/src/gtest-all.cc && lib gmock-all.obj gtest-all.obj /OUT:gmock_main.a
	    INSTALL_COMMAND ""
	    CMAKE_ARGS ""
	)

else()

	set(GMOCK_BINARY_DIR ${CMAKE_BINARY_DIR}/Dependencies/gmock)
	ExternalProject_Add(
	    gmock
	    URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
	    URL_HASH SHA1=f9d9dd882a25f4069ed9ee48e70aff1b53e3c5a5
	    PREFIX ${GMOCK_BINARY_DIR}
	    CONFIGURE_COMMAND ""
	    BINARY_DIR ${GMOCK_BINARY_DIR}/src/gmock/make
	    INSTALL_COMMAND ""
	    CMAKE_ARGS -Dgtest_disable_pthreads=${DISABLE_PTHREADS}
	)

endif()

add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gmock)
ExternalProject_Get_Property(gmock source_dir binary_dir)
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/gmock_main.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

if(MSVC_VERSION GREATER 1600)
    include_directories("${source_dir}/googlemock/include")
    include_directories("${source_dir}/googletest/include")
    add_definitions(-D_VARIADIC_MAX=10)
else()
    include_directories("${source_dir}/include")
    include_directories("${source_dir}/gtest/include")
endif()

#	boost
ExternalProject_Get_Property(boost source_dir)
set(BOOST_INCLUDE_DIR ${source_dir})
include_directories("${BOOST_INCLUDE_DIR}")
link_directories("${BOOST_BINARY_DIR}/src/boost/stage/lib")

############################################################

# Include sources
include_directories(${PROJECT_SOURCE_DIR}/Source)
include_directories(${PROJECT_SOURCE_DIR}/Test)

# Get tests
file(GLOB TEST *.cpp)
file(GLOB CK2World CK2World/*.cpp)
file(GLOB CK2World_Character CK2World/Character/*.cpp)
file(GLOB EU3World EU3World/*.cpp)
file(GLOB ModWorld ModWorld/*.cpp)
file(GLOB Common Common/*.cpp)
file(GLOB Parsers Parsers/*.cpp)

# Group tests
source_group("Source Files" FILES ${TEST})
source_group("Source Files\\CK2World" FILES ${CK2World})
source_group("Source Files\\CK2World\\Character" FILES ${CK2World_Character})
source_group("Source Files\\EU3World" FILES ${EU3World})
source_group("Source Files\\ModWorld" FILES ${ModWorld})
source_group("Source Files\\Common" FILES ${Common})
source_group("Source Files\\Parsers" FILES ${Parsers})

# Make test executable
add_executable(
    ConverterTests
    ${TEST}
    ${CK2World}
    ${CK2World_Character}
    ${EU3World}
    ${ModWorld}
    ${Common}
    ${Parsers})

target_link_libraries(ConverterTests CK2ToEU3 Parser Common libgmock)