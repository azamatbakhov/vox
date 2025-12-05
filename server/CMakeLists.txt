set(TargetName VoxServer)

set(moduleRoot ${CMAKE_CURRENT_SOURCE_DIR})

vox_collect_files(Sources
  DIRECTORIES src 
  RELATIVE ${moduleRoot}
  MASK "*.cpp" "*.h" "*.hpp"
)

vox_collect_files(Headers
  DIRECTORIES include
  RELATIVE ${moduleRoot}
  MASK "*.cpp" "*.h" "*.hpp"
)

message(STATUS "SRC: ${Sources}")
message(STATUS "Headers: ${Headers}")

add_executable(${TargetName} ${Sources} ${Headers})

target_precompile_headers(${TargetName} PRIVATE ${moduleRoot}/pch.h)

target_include_directories(${TargetName} PUBLIC
  $<BUILD_INTERFACE:${moduleRoot}/include>
)

find_package(boost_asio REQUIRED CONFIG)
find_package(boost_uuid REQUIRED CONFIG)
find_package(libpqxx REQUIRED CONFIG)
target_link_libraries(${TargetName} PRIVATE common Boost::asio Boost::uuid libpqxx::pqxx)

if (Compiler_MSVC)
  target_compile_options(${TargetName} PRIVATE
#    /Zc:preprocessor
    /GR-
  )
elseif(Compiler_ClangCl)
  target_compile_options(${TargetName} PRIVATE 
    /clang:-fno-rtti)

endif()

source_group(TREE ${moduleRoot}/src PREFIX Sources FILES ${Sources})
source_group(TREE ${moduleRoot}/include PREFIX Headers FILES ${Headers})

#nau_install(${TargetName} core)

#install(DIRECTORY ${moduleRoot}/include
#    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/core/kernel
#    FILES_MATCHING 
#      PATTERN "*.h"
#      PATTERN "*.hpp"
#      PATTERN "*.ipp"
#)
