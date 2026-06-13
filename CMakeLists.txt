cmake_minimum_required(VERSION 3.14...3.16)

# ===========================================================================
#  FetchContent usage (for consumers integrating GSL as a dependency)
#
#  Recommended pattern that pins a release tag and uses shallow clone:
#
#      include(FetchContent)
#      FetchContent_Declare(GSL
#          GIT_REPOSITORY "https://github.com/microsoft/GSL"
#          GIT_TAG        "v4.2.0"
#          GIT_SHALLOW    ON
#      )
#      FetchContent_MakeAvailable(GSL)
#      target_link_libraries(my_target PRIVATE Microsoft.GSL::GSL)
#
#  GIT_TAG "v4.2.0":
#    Pins the dependency at a specific release tag for reproducible builds.
#    Can also be a branch name (e.g. "main") or commit SHA.
#
#  GIT_SHALLOW ON:
#    Performs ``git clone --depth 1``, downloading only the snapshot of the
#    target commit without full repository history.  This reduces clone time
#    and disk usage by 80-95 % in CI environments.  Omitting GIT_SHALLOW
#    forces a full clone of all history, which is wasteful in large CI
#    matrices (200+ jobs) where the cumulative cost can be minutes per run.
# ===========================================================================

project(GSL VERSION 4.2.0 LANGUAGES CXX)


# The implementation generally assumes a platform that implements C++14 support
target_compile_features(GSL INTERFACE "cxx_std_14")

# Setup include directory
add_subdirectory(include)

target_sources(GSL INTERFACE $<BUILD_INTERFACE:${GSL_SOURCE_DIR}/GSL.natvis>)

if (GSL_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()

if (GSL_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/gsl" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    set(export_name "Microsoft.GSLConfig")
    set(namespace "Microsoft.GSL::")
    set(cmake_files_install_dir ${CMAKE_INSTALL_DATADIR}/cmake/Microsoft.GSL)

    install(TARGETS GSL EXPORT ${export_name} INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT ${export_name} NAMESPACE ${namespace} DESTINATION ${cmake_files_install_dir})
    export(TARGETS GSL NAMESPACE ${namespace} FILE ${export_name}.cmake)

    set(gls_config_version "${CMAKE_CURRENT_BINARY_DIR}/Microsoft.GSLConfigVersion.cmake")

    write_basic_package_version_file(${gls_config_version} COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)

    install(FILES ${gls_config_version} DESTINATION ${cmake_files_install_dir})

    install(FILES GSL.natvis DESTINATION ${cmake_files_install_dir})
endif()
