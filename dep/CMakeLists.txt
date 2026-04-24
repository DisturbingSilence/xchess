option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)
option(GLFW_BUILD_EXAMPLES "" OFF)

include(FetchContent)
FetchContent_Declare(
  glfw
	GIT_REPOSITORY https://github.com/glfw/glfw
	GIT_TAG  3.4
	GIT_SHALLOW  TRUE
	GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glfw)
add_library(glad glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)
