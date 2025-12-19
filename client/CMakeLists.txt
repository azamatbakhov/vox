set(TargetName VoxClient)

set(moduleRoot ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(boost_asio REQUIRED CONFIG)
find_package(Qt6 COMPONENTS Quick REQUIRED)

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

vox_collect_files(QmlFiles
  DIRECTORIES qml
  RELATIVE ${moduleRoot}
  MASK "*.qml"
)

vox_collect_files(Resources
  DIRECTORIES res
  RELATIVE ${moduleRoot}
  MASK "*.qrc"
)

qt_add_resources(resource_cpp_sources ${Resources})


add_executable(${TargetName} WIN32 
    ${Sources}
    ${Headers} 
    ${resource_cpp_sources}
    ${QmlFiles}
)

qt_add_qml_module(${TargetName}
    URI Vox
    VERSION 1.0
    QML_FILES ${QmlFiles}
    RESOURCES ${Resources}
)


target_precompile_headers(${TargetName} PRIVATE ${moduleRoot}/pch.h)

target_include_directories(${TargetName} PUBLIC
  $<BUILD_INTERFACE:${moduleRoot}/include>
)


target_link_libraries(${TargetName} PRIVATE common Boost::asio Qt6::Quick)

if (Compiler_MSVC)
  target_compile_options(${TargetName} PRIVATE
#    /Zc:preprocessor
    /GR-
  )
elseif(Compiler_ClangCl)
  target_compile_options(${TargetName} PRIVATE 
    /clang:-fno-rtti)

endif()

set_target_properties(${TargetName} PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Hang-Man
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

source_group(TREE ${moduleRoot}/src PREFIX Sources FILES ${Sources})
source_group(TREE ${moduleRoot}/include PREFIX Headers FILES ${Headers})

qt_generate_deploy_qml_app_script(
    TARGET ${TargetName}
    FILENAME_VARIABLE  deploy_script
# Optional: NO_UNSUPPORTED_PLATFORM_ERROR if targeting multiple platforms
)

install(SCRIPT "${deploy_script}")

install(TARGETS  ${TargetName}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR} # for macOS bundles
)