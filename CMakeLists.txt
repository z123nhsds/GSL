cmake_minimum_required(VERSION 3.24)

project(safe_app_project LANGUAGES CXX)

# ==========================================================================
# 使用 FetchContent 锁定 GSL 版本为 v4.2.0
# 并链接 Microsoft.GSL::GSL 至目标 safe_app
# ==========================================================================
include(FetchContent)

FetchContent_Declare(
    GSL
    GIT_REPOSITORY  https://github.com/microsoft/GSL.git
    GIT_TAG         v4.2.0
    GIT_SHALLOW     TRUE
)

# 可选：关闭 GSL 自身的测试和安装，加速构建
set(GSL_TEST OFF CACHE BOOL "" FORCE)
set(GSL_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(GSL)

# ==========================================================================
# 目标 safe_app
# ==========================================================================
add_executable(safe_app
    main.cpp
    finally_raii_demo.cpp
)

target_link_libraries(safe_app PRIVATE Microsoft.GSL::GSL)

target_compile_features(safe_app PRIVATE cxx_std_17)