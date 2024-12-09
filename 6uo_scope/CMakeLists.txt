add_executable (6uo_scope "6uo_scope.c" "6uo_scope.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET 6uo_scope PROPERTY C_STANDARD 17)
endif()

target_link_libraries(6uo_scope
  rsvisa::rsvisa
  inih
)
