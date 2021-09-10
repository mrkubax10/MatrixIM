cmake_minimum_required(VERSION 3.10)
project(MatrixIM C)
set(CMAKE_BUILD_TYPE Debug)
add_executable(MatrixIM
	src/main.c
	src/app.c
	src/utils.c
	src/http/http.c
	src/matrix/login.c
	src/matrix/register.c
	src/matrix/room.c
	src/matrix/homeserver.c
	src/network/socket.c
	src/network/socket_unix.c
	src/screens/login_screen.c
	src/screens/main_screen.c
	src/screens/new_room_dialog.c
	src/utils/message.c
	src/utils/vector.c
	src/utils/filesystem.c
)
if(UNIX)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
elseif(GTK3_LIBRARIES AND GTK3_INCLUDE_DIRS)
	target_link_libraries(MatrixIM ${GTK3_LIBRARIES})
	target_include_directories(MatrixIM ${GTK3_INCLUDE_DIRS})
else()
	message(FATAL_ERROR "You need manually specify GTK3 directory on Windows via setting GTK3_LIBRARIES and GTK3_INCLUDE_DIRS variables")
endif(UNIX)
set(THREADS_PREFER_PTHREAD_FLAG ON)
if(NOT Threads::Threads)
	find_package(Threads REQUIRED)
endif(NOT Threads::Threads)
if(NOT CJSON_LIBRARIES AND NOT CJSON_INCLUDE_DIRS)
	find_package(cJSON REQUIRED)
endif(NOT CJSON_LIBRARIES AND NOT CJSON_INCLUDE_DIRS)
target_link_libraries(MatrixIM PRIVATE ${GTK3_LIBRARIES})
target_link_libraries(MatrixIM PRIVATE Threads::Threads)
target_link_libraries(MatrixIM PRIVATE ${CJSON_LIBRARIES})
target_include_directories(MatrixIM PRIVATE ${GTK3_INCLUDE_DIRS})
target_include_directories(MatrixIM PRIVATE ${CJSON_INCLUDE_DIRS})
target_include_directories(MatrixIM PRIVATE ${PROJECT_SOURCE_DIR}/src)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -rdynamic")
