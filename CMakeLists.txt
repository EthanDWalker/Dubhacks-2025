cmake_minimum_required(VERSION 3.22)

project(project VERSION 0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG 40626af88bd7df9a5fb80be7b25ac85b122d6c21)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG a532f5b1cf27d6a3c099437e6959cf7e398a0a67)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  fastgltf
  GIT_REPOSITORY https://github.com/spnda/fastgltf.git
  GIT_TAG d4734a7e55d8d25155e755d895e018e423845cd0)
FetchContent_MakeAvailable(fastgltf)

set(DAWN_FETCH_DEPENDENCIES ON)
set(DAWN_BUILD_MONOLITHIC_LIBRARY STATIC)
add_subdirectory("dawn" EXCLUDE_FROM_ALL)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG 9a5d5c45f54b1301ea471622eddede70384243af)
FetchContent_MakeAvailable(imgui)

add_library(
  imgui STATIC
  ${imgui_SOURCE_DIR}/backends/imgui_impl_wgpu.h
  ${imgui_SOURCE_DIR}/backends/imgui_impl_wgpu.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
  ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
  ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.h
  ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
  ${imgui_SOURCE_DIR}/imconfig.h
  ${imgui_SOURCE_DIR}/imgui.h
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_internal.h
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imstb_rectpack.h
  ${imgui_SOURCE_DIR}/imstb_textedit.h
  ${imgui_SOURCE_DIR}/imstb_truetype.h)

target_compile_definitions(imgui PRIVATE IMGUI_IMPL_WEBGPU_BACKEND_DAWN)

target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR}
                                        ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PUBLIC webgpu_dawn glfw)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_executable(engine ${SOURCES})

if(EMSCRIPTEN)
  set_target_properties(engine PROPERTIES SUFFIX ".html")
  target_link_libraries(engine PRIVATE emdawnwebgpu_cpp webgpu_glfw)

  set(PRELOAD_FLAGS "--preload-file" "${CMAKE_SOURCE_DIR}/assets/@../assets")

  target_link_options(engine PRIVATE "-sASYNCIFY=1" "-sUSE_GLFW=3"
                      "-sALLOW_MEMORY_GROWTH" ${PRELOAD_FLAGS})
else()
  target_link_libraries(engine PRIVATE webgpu_dawn webgpu_glfw glfw)
endif()

target_link_libraries(engine PRIVATE fmt::fmt glm fastgltf::fastgltf imgui)

target_include_directories(engine PUBLIC ${CMAKE_SOURCE_DIR}/src)
