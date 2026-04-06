cmake_minimum_required(VERSION 3.16)
project(PocketTrack VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    Network
)

# ── Core library ──────────────────────────────────────────────────────────────
set(CORE_SOURCES
    core/src/Song.cpp
    core/src/SongLibrary.cpp
    core/src/UserProfile.cpp
    core/src/Playlist.cpp
    core/src/Recommender.cpp
)

set(CORE_HEADERS
    core/include/Song.hpp
    core/include/SongLibrary.hpp
    core/include/UserProfile.hpp
    core/include/Playlist.hpp
    core/include/Recommender.hpp
)

add_library(PocketTrackCore STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(PocketTrackCore PUBLIC core/include)
target_link_libraries(PocketTrackCore PUBLIC Qt6::Core)

# ── GUI application ───────────────────────────────────────────────────────────
set(APP_SOURCES
    app/src/main.cpp
    app/src/MainWindow.cpp
    app/src/SongRowWidget.cpp
    app/src/PlayerBarWidget.cpp
    app/src/AppController.cpp
    app/src/PlaylistPanelWidget.cpp
    app/src/RecommendationPanelWidget.cpp
)

set(APP_HEADERS
    app/include/MainWindow.hpp
    app/include/SongRowWidget.hpp
    app/include/PlayerBarWidget.hpp
    app/include/AppController.hpp
    app/include/PlaylistPanelWidget.hpp
    app/include/RecommendationPanelWidget.hpp
)

add_executable(PocketTrack ${APP_SOURCES} ${APP_HEADERS})
target_include_directories(PocketTrack PRIVATE app/include)
target_link_libraries(PocketTrack PRIVATE
    PocketTrackCore
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::Network
)