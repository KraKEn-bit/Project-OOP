#include "AppController.hpp"
#include <QDebug>
#include <algorithm>

using namespace std;

AppController::AppController(QObject *parent)
    : QObject(parent)
    , m_library(make_unique<SongLibrary>())
    , m_userProfile(make_unique<UserProfile>("Guest"))
    , m_recommender(nullptr)   // created after library is loaded
{}

AppController::~AppController() {
    saveUserProfile();
}

// Library

bool AppController::loadSongsFromCSV(const QString& path) {
    qDebug() << "Loading songs from:" << path;

    bool success = m_library->loadFromCSV(path);

    if (success) {
        int count = m_library->getSongCount();
        qDebug() << "Successfully loaded" << count << "songs";


        m_recommender = make_unique<Recommender>(m_library.get(), m_userProfile.get());


        loadUserProfile();

        emit songsLoaded();
        emit songCountChanged(count);
    } else {
        qDebug() << "Failed to load songs from CSV";
    }

    return success;
}

int AppController::getSongCount() const {
    return m_library->getSongCount();
}

vector<shared_ptr<Song>> AppController::getAllSongs() const {
    return m_library->getAllSongs();
}

vector<shared_ptr<Song>> AppController::searchSongs(const QString& query) const {
    if (query.isEmpty()) return getAllSongs();

    auto titleResults  = m_library->searchByTitle(query);
    auto artistResults = m_library->searchByArtist(query);

    // Merge without duplicates
    vector<shared_ptr<Song>> combined = titleResults;
    for (const auto& song : artistResults) {
        bool exists = any_of(combined.begin(), combined.end(),
            [&song](const shared_ptr<Song>& s){ return s->getId() == song->getId(); });
        if (!exists) combined.push_back(song);
    }
    return combined;
}

vector<shared_ptr<Song>> AppController::filterByMood(const QString& mood) const {
    return m_library->filterByMood(mood);
}

shared_ptr<Song> AppController::getSongById(int id) const {
    return m_library->getSongById(id);
}

// Playback events

void AppController::onSongPlayed(shared_ptr<Song> song) {
    if (!song) return;
    m_userProfile->recordPlay(song);
    song->incrementPlayCount();
    emit recommendationsReady();
}

void AppController::rateSong(int songId, int rating) {
    auto song = getSongById(songId);
    if (song) {
        song->setRating(rating);
        m_userProfile->rateSong(songId, rating);
    }
}

// Recommendations

vector<shared_ptr<Song>> AppController::getRecommendations(int count) {
    if (!m_recommender) return {};
    return m_recommender->getRecommendations(count);
}

vector<shared_ptr<Song>> AppController::getMoodRecommendations(
        const string& mood, int count) {
    if (!m_recommender) return filterByMood(QString::fromStdString(mood));
    return m_recommender->getMoodBasedRecommendations(mood, count);
}

vector<shared_ptr<Song>> AppController::getThemePlaylist(
        const string& theme, int count) {
    if (!m_recommender) return {};
    return m_recommender->generatePlaylist(theme, count);
}

vector<shared_ptr<Song>> AppController::getSimilarSongs(
        shared_ptr<Song> song, int count) {
    if (!m_recommender) return {};
    return m_recommender->getSimilarSongs(song, count);
}

// Playlists

void AppController::createPlaylist(const string& name) {

    for (const auto& pl : m_playlists)
        if (pl->getName() == name) return;
    m_playlists.push_back(make_unique<Playlist>(name));
}

void AppController::deletePlaylist(const string& name) {
    m_playlists.erase(
        remove_if(m_playlists.begin(), m_playlists.end(),
            [&name](const unique_ptr<Playlist>& pl){ return pl->getName() == name; }),
        m_playlists.end());
}

void AppController::addSongToPlaylist(const string& playlistName, shared_ptr<Song> song) {
    for (auto& pl : m_playlists) {
        if (pl->getName() == playlistName) {
            pl->addSong(song);
            return;
        }
    }
}

void AppController::removeSongFromPlaylist(const string& playlistName, int songId) {
    for (auto& pl : m_playlists) {
        if (pl->getName() == playlistName) {
            pl->removeSong(songId);
            return;
        }
    }
}

vector<string> AppController::getPlaylistNames() const {
    vector<string> names;
    for (const auto& pl : m_playlists)
        names.push_back(pl->getName());
    return names;
}

vector<shared_ptr<Song>> AppController::getPlaylistSongs(const string& name) const {
    for (const auto& pl : m_playlists)
        if (pl->getName() == name) return pl->getSongs();
    return {};
}

// User profile

bool AppController::saveUserProfile(const string& path) {
    return m_userProfile->saveToFile(path);
}

bool AppController::loadUserProfile(const string& path) {
    return m_userProfile->loadFromFile(path);
}