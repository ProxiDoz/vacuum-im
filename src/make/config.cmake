set(VACUUM_LOADER_NAME vacuum)
set(VACUUM_UTILS_NAME vacuumutils)

if (UNIX)
	set(VACUUM_UTILS_ABI 32)
endif (UNIX)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions(-DDEBUG_MODE)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

if (WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif (WIN32)


if (APPLE)
	set(INSTALL_APP_DIR "vacuum" CACHE STRING "Main binary bundle name")
	set(INSTALL_LIB_DIR "Frameworks")
	set(INSTALL_RES_DIR "Resources")
	set(INSTALL_SDK_DIR "include")
elseif (HAIKU)
	set(INSTALL_APP_DIR "Vacuum" CACHE STRING "Main binary directory name")
	set(INSTALL_LIB_DIR ".")
	set(INSTALL_RES_DIR ".")
	set(INSTALL_SDK_DIR "sdk")
elseif (UNIX)
	set(INSTALL_APP_DIR "vacuum" CACHE STRING "Main binary file name")
	set(INSTALL_LIB_DIR "lib" CACHE STRING "Name of directory for shared libraries on target system")
	set(INSTALL_RES_DIR "share")
	set(INSTALL_SDK_DIR "include")
	set(INSTALL_DOC_DIR "${INSTALL_RES_DIR}/doc" CACHE STRING "Path to install doc files")
elseif (WIN32)
	set(INSTALL_APP_DIR "vacuum" CACHE STRING "Main binary directory name")
	set(INSTALL_LIB_DIR ".")
	set(INSTALL_RES_DIR ".")
	set(INSTALL_SDK_DIR "sdk")
endif (APPLE)


set(RUN_FROM_BUILD_DIR OFF CACHE BOOL "Build executable which can be launched directly from build directory")

if (RUN_FROM_BUILD_DIR)
	set(PLUGINS_DIR "./plugins")
	set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/resources")
	set(TRANSLATIONS_DIR "./translations")
elseif (WIN32)
	set(PLUGINS_DIR "./${INSTALL_LIB_DIR}/plugins")
	set(RESOURCES_DIR "./${INSTALL_RES_DIR}/resources")
	set(TRANSLATIONS_DIR "./${INSTALL_RES_DIR}/translations")
elseif (HAIKU)
	set(PLUGINS_DIR "./${INSTALL_LIB_DIR}/plugins")
	set(RESOURCES_DIR "./${INSTALL_RES_DIR}/resources")
	set(TRANSLATIONS_DIR "./${INSTALL_RES_DIR}/translations")
elseif (APPLE)
	set(PLUGINS_DIR "../PlugIns")
	set(RESOURCES_DIR "../${INSTALL_RES_DIR}")
	set(TRANSLATIONS_DIR "./${INSTALL_RES_DIR}/translations")
elseif (UNIX)
	set(PLUGINS_DIR "../${INSTALL_LIB_DIR}/${INSTALL_APP_DIR}/plugins")
	set(RESOURCES_DIR "../${INSTALL_RES_DIR}/${INSTALL_APP_DIR}/resources")
	set(TRANSLATIONS_DIR "../${INSTALL_RES_DIR}/${INSTALL_APP_DIR}/translations")
endif (RUN_FROM_BUILD_DIR)

add_definitions(-DPLUGINS_DIR="${PLUGINS_DIR}")
add_definitions(-DRESOURCES_DIR="${RESOURCES_DIR}")
add_definitions(-DTRANSLATIONS_DIR="${TRANSLATIONS_DIR}")


# All paths given relative to ${CMAKE_INSTALL_PREFIX}
if (WIN32)
	set(INSTALL_BINS "${INSTALL_APP_DIR}")
	set(INSTALL_LIBS "${INSTALL_APP_DIR}/${INSTALL_LIB_DIR}")
	set(INSTALL_INCLUDES "${INSTALL_APP_DIR}/${INSTALL_SDK_DIR}")
	set(INSTALL_PLUGINS "${INSTALL_APP_DIR}/${INSTALL_LIB_DIR}/plugins")
	set(INSTALL_RESOURCES "${INSTALL_APP_DIR}/${INSTALL_RES_DIR}/resources")
	set(INSTALL_DOCUMENTS "${INSTALL_APP_DIR}/${INSTALL_RES_DIR}")
	set(INSTALL_TRANSLATIONS "${INSTALL_APP_DIR}/${INSTALL_RES_DIR}/translations")
elseif (HAIKU)
	set(INSTALL_BINS "${INSTALL_APP_DIR}")
	set(INSTALL_LIBS "${INSTALL_APP_DIR}/${INSTALL_LIB_DIR}")
	set(INSTALL_INCLUDES "${INSTALL_APP_DIR}/${INSTALL_SDK_DIR}")
	set(INSTALL_PLUGINS "${INSTALL_APP_DIR}/${INSTALL_LIB_DIR}/plugins")
	set(INSTALL_RESOURCES "${INSTALL_APP_DIR}/${INSTALL_RES_DIR}/resources")
	set(INSTALL_DOCUMENTS "${INSTALL_APP_DIR}/${INSTALL_RES_DIR}/doc")
	set(INSTALL_TRANSLATIONS "${INSTALL_APP_DIR}/${INSTALL_RES_DIR}/translations")
elseif (APPLE)
	set(INSTALL_BINS ".")
	set(INSTALL_LIBS "${INSTALL_APP_DIR}/Contents/${INSTALL_LIB_DIR}")
	set(INSTALL_INCLUDES "${INSTALL_APP_DIR}/Contents/${INSTALL_RES_DIR}/${INSTALL_SDK_DIR}")
	set(INSTALL_PLUGINS "${INSTALL_APP_DIR}/Contents/PlugIns")
	set(INSTALL_RESOURCES "${INSTALL_APP_DIR}/Contents/${INSTALL_RES_DIR}")
	set(INSTALL_DOCUMENTS "${INSTALL_APP_DIR}/Contents/${INSTALL_RES_DIR}")
	set(INSTALL_TRANSLATIONS "${INSTALL_APP_DIR}/Contents/${INSTALL_RES_DIR}/translations")
elseif (UNIX)
	set(INSTALL_BINS "bin")
	set(INSTALL_LIBS "${INSTALL_LIB_DIR}")
	set(INSTALL_INCLUDES "${INSTALL_SDK_DIR}/${INSTALL_APP_DIR}")
	set(INSTALL_PLUGINS "${INSTALL_LIB_DIR}/${INSTALL_APP_DIR}/plugins")
	set(INSTALL_RESOURCES "${INSTALL_RES_DIR}/${INSTALL_APP_DIR}/resources")
	set(INSTALL_DOCUMENTS "${INSTALL_DOC_DIR}/${INSTALL_APP_DIR}")
	set(INSTALL_TRANSLATIONS "${INSTALL_RES_DIR}/${INSTALL_APP_DIR}/translations")
endif (WIN32) 
