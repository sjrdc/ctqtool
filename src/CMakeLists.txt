FILE(GLOB RESOURCES ../res/* ../qdarkstyle/style.*)

add_executable(ctqtool

    WIN32
    main.cpp
    mainwindow.h
    mainwindow.cpp
    utilities.h
    utilities.cpp
    ${RESOURCES}
    )

target_link_libraries(ctqtool Qt5::Widgets)
