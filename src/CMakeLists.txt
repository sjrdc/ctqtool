add_subdirectory(datamodel)

FILE(GLOB RESOURCES ../res/* ../qdarkstyle/style.*)

add_executable(ctqtool

    WIN32
    ctqtreescene.h
    ctqtreescene.cpp
    itemdialog.h
    itemdialog.cpp
    main.cpp
    mainwindow.h
    mainwindow.cpp
    treeview.h
    treeview.cpp
    utilities.h
    utilities.cpp
    ${RESOURCES}
    )

target_link_libraries(ctqtool datamodel Qt5::Widgets)
