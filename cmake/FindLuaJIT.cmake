find_path(LUAJIT_INCLUDE_DIR luajit.h /usr/include/luajit-2.0 /usr/local/include/luajit-2.0)
find_library(LUAJIT_LIBRARY NAME luajit-5.1 PATHS /usr/lib /usr/local/lib)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LuaJIT DEFAULT_MSG LUAJIT_INCLUDE_DIR LUAJIT_LIBRARY)