include_directories(${CMAKE_SOURCE_DIR}/src)
add_library(ui
    ctqtreescene.h
    ctqtreescene.cpp
    ctqview.h
    ctqview.cpp
    itemdialog.h
    itemdialog.cpp
    mainwindow.h
    mainwindow.cpp
    treeview.h
    treeview.cpp
    utilities.h
    utilities.cpp
    ${RESOURCES}
    )

target_link_libraries(ui datamodel Qt6::Widgets)
