
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(datamodel
  ctq.cpp
  ctqtree.cpp
  ctqmodel.cpp
  ctqproxymodel.cpp
  driver.cpp
  item.cpp
  measurement.cpp
  target.cpp
  userneed.cpp
  )

target_link_libraries(datamodel Qt5::Core)
