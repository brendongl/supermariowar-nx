#
# Nintendo Switch (devkitPro / libnx) config
#
set(NO_NETWORK ON CACHE BOOL "" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SMW_INSTALL_PORTABLE ON CACHE BOOL "" FORCE)

# Prebuilt SDL3 stack from platform/switch/build-deps.sh
set(SMW_SWITCH_DEPS "${CMAKE_SOURCE_DIR}/.deps-switch" CACHE PATH "Switch dependency prefix")
list(PREPEND CMAKE_PREFIX_PATH "${SMW_SWITCH_DEPS}")
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE ALWAYS)
set(FETCHCONTENT_FULLY_DISCONNECTED ON)

set(SMW_SWITCH_TITLE   "Super Mario War" CACHE STRING "")
set(SMW_SWITCH_AUTHOR  "72dpiarmy & contributors" CACHE STRING "")
# The Switch port has its own release numbering, separate from the upstream game (2.0).
set(SMW_SWITCH_VERSION "1.0.0" CACHE STRING "")
