cmake_minimum_required (VERSION 3.20)
project(VoxComm)

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW) 
endif()

set(CMAKE_CXX_STANDARD 20)
include(CTest)
enable_testing()

include(cmake/common.cmake)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file (STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/.voxversion VOX_VERSION_STRING)
vox_set_version("${VOX_VERSION_STRING}")


add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)
