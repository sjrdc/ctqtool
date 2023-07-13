cmake_minimum_required(VERSION 3.16)
project(ctqtool VERSION 0.1.0 LANGUAGES CXX)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)

  set(CMAKE_PREFIX_PATH "D:\\Qt5.14.2\\5.14.2\\msvc2017")
  set(Boost_USE_STATIC_LIBS ON)
  
  set(BOOST_ROOT          $ENV{BOOST_LOCATION})
  set(BOOST_INCLUDE_DIR   $ENV{BOOST_LOCATION})
  set(BOOST_LIBRARYDIR    $ENV{BOOST_LOCATION}/stage/lib/x86)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP8") 

elseif(UNIX)

endif()

find_package(Boost)
if(${Boost_FOUND})
  include_directories(${Boost_INCLUDE_DIRS}/) 
  link_directories(${Boost_LIBRARY_DIRS})
else()
  message(ERROR "Failed to load boost")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)    
# Instruct CMake to Run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(src)
