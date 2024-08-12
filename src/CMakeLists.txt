cmake_minimum_required(VERSION 3.5)
project(SECURE-MESSENGER)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Paths for GTK3
set(ENV{PKG_CONFIG_PATH} "C:/msys64/mingw64/lib/pkgconfig")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIBRARY_DIRS})
add_definitions(${GTK4_CFLAGS_OTHER})

# Add ONNX Runtime include and lib directories
set(ONNX_RUNTIME_DIR "${CMAKE_SOURCE_DIR}/external/onnxruntime")
include_directories(${ONNX_RUNTIME_DIR}/include)
link_directories(${ONNX_RUNTIME_DIR}/lib)

# Add OpenSSL include and lib directories
set(OPENSSL_ROOT_DIR "C:/msys64/mingw64/include/openssl")  # Update this path to your OpenSSL installation
include_directories(${OPENSSL_ROOT_DIR}/include)
link_directories(${OPENSSL_ROOT_DIR}/lib)

file(GLOB NETWORKING_SOURCES "${CMAKE_SOURCE_DIR}/networking/*.c")
file(GLOB GUI_SOURCES "${CMAKE_SOURCE_DIR}/gui/*.c")

# Add source files
add_executable(Messenger main.c ${GUI_SOURCES} ${NETWORKING_SOURCES}) #ai_integration/anti_phishing.c)
#set_target_properties(Messenger PROPERTIES WIN32_EXECUTABLE true)  # If you dont want console to run with your .exe

# Link libraries
target_link_libraries(Messenger ${GTK4_LIBRARIES})
target_link_libraries(Messenger Ws2_32)

#Link ONNX Runtime library
if(WIN32)
    target_link_libraries(Messenger onnxruntime)
elseif(UNIX)
    target_link_libraries(Messenger onnxruntime)
endif()

# Link OpenSSL libraries
target_link_libraries(Messenger libssl libcrypto)

target_link_libraries(Messenger Ws2_32)
