cmake_minimum_required(VERSION 3.5)

project(SECURE-MESSENGER)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Add source files
file(GLOB_RECURSE NETWORKING "networking/*.c")
file(GLOB_RECURSE NETWORKING_H "networking/*.h")
file(GLOB_RECURSE GUI "gui/*.c")
file(GLOB_RECURSE GUI_H "gui/*.h")
file(GLOB_RECURSE ENC "encryption/*.c")
file(GLOB_RECURSE ENC_H "encryption/*.h")

add_executable(Messenger
    main.c
    ${ENC}
    ${ENC_H}
    ${GUI}
    ${GUI_H}
    ${NETWORKING}
    ${NETWORKING_H}
)

# Include directories for external libraries
include_directories(src/external/onnxruntime/include)

# Link directories for external libraries (if needed)
link_directories(src/external/onnxruntime/lib)

# Find GTK4 using pkg-config
find_package(PkgConfig REQUIRED)

# Set the PKG_CONFIG_PATH environment variable if needed (adjust the path as necessary)
set(ENV{PKG_CONFIG_PATH} "C:/msys64/ucrt64/lib/pkgconfig")

# Check for GTK4 package
pkg_check_modules(GTK4 REQUIRED gtk4)

# Include GTK4 directories
include_directories(${GTK4_INCLUDE_DIRS})

# Link GTK4 libraries
target_link_libraries(Messenger PRIVATE ${GTK4_LIBRARIES} ws2_32)

# Find OpenSSL package
find_package(OpenSSL REQUIRED)

# Include OpenSSL directories
include_directories(${OPENSSL_INCLUDE_DIR})

# Link OpenSSL libraries
target_link_libraries(Messenger PRIVATE ${OPENSSL_LIBRARIES})

# Add GTK4-specific compile flags (optional)
add_definitions(${GTK4_CFLAGS_OTHER})


# Add CPack configuration
set(CPACK_GENERATOR "ZIP;NSIS") # Choose ZIP and NSIS as generators
set(CPACK_PACKAGE_NAME "SecureMessenger")
set(CPACK_PACKAGE_VERSION "1.1.0")
set(CPACK_PACKAGE_VENDOR "jamesm")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Secure Messenger Application")
set(CPACK_PACKAGE_EXECUTABLES "Messenger" "Secure Messenger")
set(CPACK_NSIS_DISPLAY_NAME "Secure Messenger")
set(CPACK_NSIS_INSTALLED_ICON_NAME "Messenger.exe")

# Add any additional files or directories you want to include in the package
install(TARGETS Messenger DESTINATION .)
set(GTK4_BIN_DIR "C:/msys64/ucrt64/bin")
# Install all .dll files and gdbus.exe
install(DIRECTORY "${GTK4_BIN_DIR}/" 
    DESTINATION .
    FILES_MATCHING PATTERN "*.dll" 
                   PATTERN "gdbus.exe"
)
set(ONNX_RUNTIME_DIR "${CMAKE_SOURCE_DIR}/external/onnxruntime")
install(FILES 
    "${ONNX_RUNTIME_DIR}/lib/onnxruntime.dll"
    DESTINATION .
)

# Run CPack
include(CPack)
