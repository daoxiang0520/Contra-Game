cmake_minimum_required(VERSION 3.16)
project(Contra VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)


set(PROJECT_SOURCES
    main.cpp
    mainwindow.h mainwindow.cpp
    startwidget.h startwidget.cpp
    animationbutton.h animationbutton.cpp
    levelwidget.h levelwidget.cpp
    settingwidget.h settingwidget.cpp
    blockwindow.h blockwindow.cpp
    tile.h tile.cpp
    sqlhandle.h sqlhandle.cpp
    player.h player.cpp
    bullet.h bullet.cpp
    mapdata.h mapdata.cpp
    gameview.h gameview.cpp
    enemy.h enemy.cpp

    gamedefs.h
    res.qrc
    Map.db

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Contra
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        globalaudio.h globalaudio.cpp
        globalsound.cpp globalsound.h
        soundplayer.cpp soundplayer.h
        bgmplayer.h bgmplayer.cpp
        image/stage/bullet.png image/stage/firebullet.png
        image/stage/bulletleft.png image/stage/bulletright.png image/stage/firebulletleft.png image/stage/firebulletright.png
        bgms/cure.mp3 bgms/injure.ogg
        resultwindow.h resultwindow.cpp
        multiplayerwidget.h multiplayerwidget.cpp
        onlinegamewidget.h networkmanager.h
        game.pb.h
        onlinegamewidget.cpp networkmanager.cpp game.pb.cc
        connectdialog.h connectdialog.cpp
    )
else()
    if(ANDROID)
        add_library(Contra SHARED ${PROJECT_SOURCES})
    else()
        add_executable(Contra ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(Contra PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
)
target_link_libraries(Contra PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Contra PRIVATE Qt${QT_VERSION_MAJOR}::Core)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Contra)
endif()

set_target_properties(Contra PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Contra
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
find_package(Qt6 COMPONENTS Sql REQUIRED)
target_link_libraries(Contra PRIVATE Qt6::Sql)
target_link_libraries(Contra PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_include_directories(Contra PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/3party/protobuf/include")
target_link_libraries(Contra PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libprotobuf.a")
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Contra)
endif()
