set(TargetName common)

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

vox_collect_files(ProtoFiles
  DIRECTORIES proto
  MASK "*.proto"
)

message(STATUS "SRC: ${Sources}")
message(STATUS "Headers: ${Headers}")
message(STATUS "PROTO_FILES: ${ProtoFiles}")

add_library(${TargetName} ${Sources} ${Headers} ${ProtoFiles})

set(PROTO_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/vox/proto")
file(MAKE_DIRECTORY ${PROTO_OUTPUT_DIR})
    
find_package(Protobuf  REQUIRED)
protobuf_generate(
    TARGET ${TargetName}
    LANGUAGE cpp
    OUT_VAR PROTOBUF_GENERATED_FILES
    IMPORT_DIRS "${moduleRoot}/proto"
    PROTOC_OUT_DIR ${PROTO_OUTPUT_DIR}
)
target_include_directories(${TargetName} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})  
    


message(STATUS "PROTO_COMPILED: ${PROTO_OUTPUT_DIR} ${PROTOBUF_GENERATED_FILES} ${protoHeaders}")


target_precompile_headers(${TargetName} PRIVATE ${moduleRoot}/pch.h)

target_include_directories(${TargetName} PUBLIC
  $<BUILD_INTERFACE:${moduleRoot}/include>
)

target_link_libraries(${TargetName} PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

if (Compiler_MSVC)
  target_compile_options(${TargetName} PRIVATE
#    /Zc:preprocessor
    /GR-
  )
elseif(Compiler_ClangCl)
  target_compile_options(${TargetName} PRIVATE 
    /clang:-fno-rtti)

endif()
 