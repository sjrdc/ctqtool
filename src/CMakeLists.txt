add_subdirectory(datamodel)
add_subdirectory(ui)

FILE(GLOB RESOURCES ../res/* ../qdarkstyle/style.*)

add_executable(ctqtool main.cpp ${RESOURCES})
target_link_libraries(ctqtool ui)
