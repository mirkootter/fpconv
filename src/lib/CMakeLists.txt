cmake_minimum_required (VERSION 3.8)

find_package(Qt5Widgets CONFIG REQUIRED)

add_library(lib
	"fpconv.cpp"

	"include/lib/fpconv.h")

target_include_directories(lib INTERFACE "include/" PRIVATE "include/lib/")
target_link_libraries(lib PUBLIC Qt5::Core)
target_compile_features(lib PUBLIC cxx_std_11)
