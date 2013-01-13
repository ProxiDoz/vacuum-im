set(TARGET_LOADER vacuum)
set(TARGET_UTILS vacuumutils)

if (UNIX)
	set(VERSION_UTILS 1.2.0)
	set(VERSION_UTILS_ABI 1.2)
endif (UNIX)

if (APPLE)
	set(INSTALL_PREFIX "/Applications")
	set(INSTALL_APP_DIR "vacuum")
	set(INSTALL_LIB_DIR "Frameworks")
	set(INSTALL_RES_DIR "Resources")
elseif (UNIX)
	set(INSTALL_PREFIX "/usr/local")
	set(INSTALL_APP_DIR "vacuum")
	set(INSTALL_LIB_DIR "lib" CACHE STRING "Name of directory for shared libraries on target system")
	set(INSTALL_RES_DIR "share")
elseif (WIN32)
	set(INSTALL_PREFIX "C:")
	set(INSTALL_APP_DIR "vacuum")
	set(INSTALL_LIB_DIR ".")
	set(INSTALL_RES_DIR ".")
endif (APPLE)

if (CMAKE_INSTALL_PREFIX STREQUAL "")
	set(CMAKE_INSTALL_PREFIX "${INSTALL_PREFIX}")
endif (CMAKE_INSTALL_PREFIX STREQUAL "")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(PLUGINS_DIR "./plugins")
	set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/resources")
	set(TRANSLATIONS_DIR "./translations")
elseif (APPLE)
	set(PLUGINS_DIR "../PlugIns")
	set(RESOURCES_DIR "../${INSTALL_RES_DIR}")
	set(TRANSLATIONS_DIR "./${INSTALL_RES_DIR}/translations")
elseif (UNIX)
	set(PLUGINS_DIR "../${INSTALL_LIB_DIR}/${INSTALL_APP_DIR}/plugins")
	set(RESOURCES_DIR "../${INSTALL_RES_DIR}/${INSTALL_APP_DIR}/resources")
	set(TRANSLATIONS_DIR "../${INSTALL_RES_DIR}/${INSTALL_APP_DIR}/translations")
elseif (WIN32)
	set(PLUGINS_DIR "./${INSTALL_LIB_DIR}/plugins")
	set(RESOURCES_DIR "./${INSTALL_RES_DIR}/resources")
	set(TRANSLATIONS_DIR "./${INSTALL_RES_DIR}/translations")
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

add_definitions(-DPLUGINS_DIR="${PLUGINS_DIR}")
add_definitions(-DRESOURCES_DIR="${RESOURCES_DIR}")
add_definitions(-DTRANSLATIONS_DIR="${TRANSLATIONS_DIR}")

include_directories(${CMAKE_SOURCE_DIR}/src)

if (WIN32)
	set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif (WIN32)

include("${CMAKE_SOURCE_DIR}/install.cmake")
include("${CMAKE_SOURCE_DIR}/translations.cmake")
