add_library(datamodel
  ctq.cpp
  driver.cpp
  item.cpp
  measurement.cpp
  target.cpp
  userneed.cpp
  )

target_link_libraries(datamodel Qt5::Core)
