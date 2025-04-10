# CMakeList.txt: file di progetto CMake di primo livello. Eseguire la configurazione globale
# e includere qui i sottoprogetti.
#
cmake_minimum_required (VERSION 3.8)

project ("TMT_Final_Project")											# Nuovo progetto nuovo nome 

set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable("TMT_Final_Project" Main.cpp Task1.cpp meanShift.cpp showHistogram.cpp hsv_space.cpp hsv_abgr.cpp hsv.cpp)					# Nuovo progetto nuovo nome e nuovo cpp

target_link_libraries("TMT_Final_Project" ${OpenCV_LIBS} )				# Nuovo progetto nuovo nome 

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
