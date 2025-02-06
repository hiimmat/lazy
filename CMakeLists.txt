cmake_minimum_required(VERSION 3.28.2)

project("lazy"
        VERSION 0.1.0
        DESCRIPTION "A C++ library for lazy evaluation"
        LANGUAGES CXX
)

if(CMAKE_PROJECT_NAME STREQUAL ${PROJECT_NAME})
  set(LAZY_IS_ROOT_PROJECT TRUE)
else()
  set(LAZY_IS_ROOT_PROJECT FALSE)
endif()

option(LAZY_ENABLE_EXPECTS "Enable Expects" TRUE)
option(LAZY_ENABLE_ENSURES "Enable Ensures" TRUE)
option(LAZY_THROW_ON_CONTRACT_VIOLATION "Throw on contract violation instead of terminating the application" FALSE)
option(LAZY_BUILD_TESTS "Build lazy tests" TRUE)

add_library(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PUBLIC
        FILE_SET
            CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/include
        FILES
            ${PROJECT_SOURCE_DIR}/include/array_traits.cppm
            ${PROJECT_SOURCE_DIR}/include/einsum.cppm
            ${PROJECT_SOURCE_DIR}/include/expression.cppm
            ${PROJECT_SOURCE_DIR}/include/invocable_traits.cppm
            ${PROJECT_SOURCE_DIR}/include/string_literal.cppm
            ${PROJECT_SOURCE_DIR}/include/utilities.cppm
)

# add alias so the project can be uses with add_subdirectory
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<BOOL:${LAZY_ENABLE_EXPECTS}>:LAZY_ENABLE_EXPECTS>")
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<BOOL:${LAZY_ENABLE_ENSURES}>:LAZY_ENABLE_ENSURES>")
target_compile_definitions(${PROJECT_NAME} PUBLIC "$<$<BOOL:${LAZY_THROW_ON_CONTRACT_VIOLATION}>:LAZY_THROW_ON_CONTRACT_VIOLATION>")

if(LAZY_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(LAZY_INSTALL_LIBRARY)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    write_basic_package_version_file(
        "${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMinorVersion
    )

    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
    )

    install(EXPORT ${PROJECT_NAME}_Targets
            FILE ${PROJECT_NAME}Targets.cmake
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
            CXX_MODULES_DIRECTORY include
    )

    install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake
    )

    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/license.txt")

    include(CPack)
endif()
