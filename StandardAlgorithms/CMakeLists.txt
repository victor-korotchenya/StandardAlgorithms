# CMakeList.txt : CMake project for StandardAlgorithms, include source and define project specific logic here.
#
cmake_minimum_required (VERSION 3.16)

file(GLOB_RECURSE StandardAlgorithms_Source
     "*.h"
     "*.cpp"
)
add_executable(StandardAlgorithms ${StandardAlgorithms_Source})

target_include_directories(StandardAlgorithms PRIVATE
$ENV{GSL_INCLUDE}
)

if (MSVC)
    # warning level 4 and all warnings as errors
    #add_compile_options(/W4 /WX)
    string(REGEX REPLACE "/W[3|4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

if (MSVC_VERSION GREATER_EQUAL "1900")
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
target_compile_features(StandardAlgorithms PRIVATE cxx_std_23)
#add_compile_definitions(a=1) == #define a 1

#set_property(TARGET StandardAlgorithms PROPERTY CXX_STANDARD 23)
# TODO: Add tests and install targets if needed.

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++23")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
#target_compile_options(StandardAlgorithms PUBLIC "/Zc:__cplusplus")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
