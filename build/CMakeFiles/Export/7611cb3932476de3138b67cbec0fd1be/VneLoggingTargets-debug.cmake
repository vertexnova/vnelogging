#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "vnelogging::vnelogging" for configuration "Debug"
set_property(TARGET vnelogging::vnelogging APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(vnelogging::vnelogging PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libvnelogging.a"
  )

list(APPEND _cmake_import_check_targets vnelogging::vnelogging )
list(APPEND _cmake_import_check_files_for_vnelogging::vnelogging "${_IMPORT_PREFIX}/lib/libvnelogging.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
