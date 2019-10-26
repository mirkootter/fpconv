cmake_minimum_required (VERSION 3.8)

find_package(Qt5Widgets CONFIG REQUIRED)

#qt5_add_resources(QT_RESSOURCES "res/main.qrc")
qt5_wrap_cpp(QT_MOCFILES
	"app_mainwindow.h")
qt5_wrap_ui(QT_UIFILES
	"app_mainwindow.ui")

add_library(app ${QT_MOCFILES} ${QT_UIFILES}
	"app_mainwindow.cpp"
	"main.cpp"

	"app_mainwindow.h")

target_include_directories(lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(app PRIVATE lib)
target_link_libraries(app PRIVATE Qt5::Widgets)
target_compile_features(app PRIVATE cxx_std_11)
