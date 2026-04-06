#include "Playlist.hpp"
#include <algorithm>
#include <fstream>
#include <stdexcept>

using namespace std;

Playlist::Playlist(const string& name)
    : m_name(name)
{}

Playlist::~Playlist() = default;

int Playlist::getTotalDuration() const {
    int total = 0;
    for (const auto& song : m_songs)
        total += song->getDuration();
    return total;
}

void Playlist::addSong(shared_ptr<Song> song) {
    if (song && !containsSong(song->getId()))
        m_songs.push_back(song);
}

void Playlist::removeSong(int songId) {
    m_songs.erase(
        remove_if(m_songs.begin(), m_songs.end(),
                  [songId](const shared_ptr<Song>& s){ return s->getId() == songId; }),
        m_songs.end());
}

void Playlist::moveSong(int fromIndex, int toIndex) {
    int sz = static_cast<int>(m_songs.size());
    if (fromIndex < 0 || fromIndex >= sz || toIndex < 0 || toIndex >= sz) return;
    if (fromIndex == toIndex) return;

    auto song = m_songs[fromIndex];
    m_songs.erase(m_songs.begin() + fromIndex);
    m_songs.insert(m_songs.begin() + toIndex, song);
}

bool Playlist::containsSong(int songId) const {
    return any_of(m_songs.begin(), m_songs.end(),
                  [songId](const shared_ptr<Song>& s){ return s->getId() == songId; });
}

void Playlist::clear() {
    m_songs.clear();
}

bool Playlist::saveToFile(const string& path) const {
    ofstream file(path);
    if (!file.is_open()) return false;

    file << "playlist:" << m_name << "\n";
    for (const auto& song : m_songs)
        file << "song:" << song->getId() << "\n";

    return true;
}