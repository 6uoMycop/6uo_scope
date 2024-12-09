# CMakeList.txt: файл проекта CMake верхнего уровня; проведите глобальную конфигурацию
# и включите сюда подпроекты.
#
cmake_minimum_required (VERSION 3.8)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("6uo_scope")

# CONFIG
include(DefineOptions.cmake)

# Зависимости
find_package(RsVisa REQUIRED)
add_subdirectory ("deps/inih")  # INI parser library

include_directories(
  "deps/inih"
)
link_directories(
  "${CMAKE_BINARY_DIR}/deps/inih"
)

# Включите подпроекты.
add_subdirectory ("6uo_scope")
