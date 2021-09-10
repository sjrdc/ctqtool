add_subdirectory(datamodel)

FILE(GLOB RESOURCES ../res/* ../qdarkstyle/style.*)

add_executable(ctqtool

    WIN32
    main.cpp
    mainwindow.h
    mainwindow.cpp
    ctqtreescene.h
    ctqtreescene.cpp
    utilities.h
    utilities.cpp
    ${RESOURCES}
    )

target_link_libraries(ctqtool datamodel Qt5::Widgets)
