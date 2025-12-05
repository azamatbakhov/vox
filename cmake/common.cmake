###     vox_collect_files(<variable>
###         [DIRECTORIES <directories>]
###         [RELATIVE <relative-path>]
###         [MASK <globbing-expressions>]
###         [EXCLUDE <regex-to-exclude>]
###    )
###
###  Generate a list of files from <directories> (traverse all the subdirectories) that match the <globbing-expressions> and store it into the <variable>
###  If RELATIVE flag is specified, the results will be returned as relative paths to the given path.
###  If EXCLUDE is specified, all paths that matches any <regex-to-exclude> willbe removed from result
function (vox_collect_files VARIABLE) 
  set(oneValueArgs RELATIVE PREPEND)
  set(multiValueArgs DIRECTORIES EXCLUDE INCLUDE MASK)
  cmake_parse_arguments(COLLECT "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (COLLECT_EXCLUDE AND COLLECT_INCLUDE)
    message(FATAL_ERROR "vox_collect_files must not specify both EXCLUDE and INCLUDE parameters")
  endif()

  set (allFiles)

  foreach (dir ${COLLECT_DIRECTORIES})
    foreach (msk ${COLLECT_MASK})
      set(globExpr "${dir}/${msk}")

      if (COLLECT_RELATIVE)
        file (GLOB_RECURSE files RELATIVE ${COLLECT_RELATIVE} ${globExpr} )
      else()
        file (GLOB_RECURSE files ${globExpr})
      endif()

      list(APPEND allFiles ${files})
    endforeach()
  endforeach()

  if (COLLECT_EXCLUDE)
    foreach (re ${COLLECT_EXCLUDE})
      list (FILTER allFiles EXCLUDE REGEX ${re})
    endforeach()
  endif(COLLECT_EXCLUDE) # COLLECT_EXCLUDE

  if (COLLECT_INCLUDE)
    foreach (re ${COLLECT_INCLUDE})
        list (FILTER allFiles INCLUDE REGEX ${re})
    endforeach()
  endif(COLLECT_INCLUDE) # COLLECT_INCLUDE

  if (COLLECT_PREPEND)
    list(TRANSFORM allFiles PREPEND "${COLLECT_PREPEND}")
  endif()


  if (${VARIABLE})
    list(APPEND ${VARIABLE} ${allFiles})
    set(${VARIABLE} ${${VARIABLE}} PARENT_SCOPE)
  else()
    set(${VARIABLE} ${allFiles} PARENT_SCOPE)
  endif()

endfunction()

##
##
function(vox_collect_cmake_subdirectories VARIABLE SCAN_DIR)

  file(GLOB children RELATIVE ${SCAN_DIR} ${SCAN_DIR}/*)
  set(result "")
  foreach(child ${children})
    
    if(IS_DIRECTORY ${SCAN_DIR}/${child} AND (EXISTS ${SCAN_DIR}/${child}/CMakeLists.txt))
      list(APPEND result ${child})
    endif()
  endforeach()

  set(${VARIABLE} ${result} PARENT_SCOPE)

endfunction()
    
function(vox_set_version VOX_VERSION_STRING)
    string(REPLACE "." ";" VOX_VERSIONS_NUMBERS ${VOX_VERSION_STRING})

    LIST(GET VOX_VERSIONS_NUMBERS 0 VERSION_MAJOR)
    LIST(GET VOX_VERSIONS_NUMBERS 1 VERSION_MINOR)
    LIST(GET VOX_VERSIONS_NUMBERS 2 VERSION_PATCH)

    set(VOX_VERSION_MAJOR ${VERSION_MAJOR} PARENT_SCOPE)
    set(VOX_VERSION_MINOR ${VERSION_MINOR} PARENT_SCOPE)
    set(VOX_VERSION_PATCH ${VERSION_PATCH} PARENT_SCOPE)

    add_compile_definitions(VOX_VERSION_MAJOR=${VERSION_MAJOR})
    add_compile_definitions(VOX_VERSION_MINOR=${VERSION_MINOR})
    add_compile_definitions(VOX_VERSION_PATCH=${VERSION_PATCH})
    add_compile_definitions(VOX_VERSION="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
endfunction()