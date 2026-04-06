#include "UserProfile.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

UserProfile::UserProfile(const string& username)
    : m_username(username)
    , m_maxHistorySize(100)
{}

UserProfile::~UserProfile() = default;

// ── Favorites 

void UserProfile::addFavoriteSong(shared_ptr<Song> song) {
    if (song && !isFavorite(song->getId()))
        m_favoriteSongs.push_back(song);
}

void UserProfile::removeFavoriteSong(int songId) {
    m_favoriteSongs.erase(
        remove_if(m_favoriteSongs.begin(), m_favoriteSongs.end(),
                  [songId](const shared_ptr<Song>& s){ return s->getId() == songId; }),
        m_favoriteSongs.end());
}

bool UserProfile::isFavorite(int songId) const {
    return any_of(m_favoriteSongs.begin(), m_favoriteSongs.end(),
                  [songId](const shared_ptr<Song>& s){ return s->getId() == songId; });
}

// ── History

void UserProfile::addToHistory(shared_ptr<Song> song) {
    if (!song) return;
    if ((int)m_listeningHistory.size() >= m_maxHistorySize)
        m_listeningHistory.erase(m_listeningHistory.begin());
    m_listeningHistory.push_back(song);
}

void UserProfile::clearHistory() {
    m_listeningHistory.clear();
}

bool UserProfile::hasListened(int songId) const {
    return any_of(m_listeningHistory.begin(), m_listeningHistory.end(),
                  [songId](const shared_ptr<Song>& s){ return s->getId() == songId; });
}

// ── Play recording 

void UserProfile::recordPlay(shared_ptr<Song> song) {
    if (!song) return;
    addToHistory(song);

    for (const auto& genre : song->getGenres())
        m_genrePlayCounts[genre->getName()]++;

    m_moodPlayCounts[song->getMood()]++;
}

// ── Top genres / moods 

vector<string> UserProfile::getTopGenres(int count) const {
    vector<pair<string,int>> sorted(m_genrePlayCounts.begin(), m_genrePlayCounts.end());
    sort(sorted.begin(), sorted.end(),
         [](const auto& a, const auto& b){ return a.second > b.second; });

    vector<string> result;
    for (int i = 0; i < min(count, (int)sorted.size()); ++i)
        result.push_back(sorted[i].first);
    return result;
}

vector<string> UserProfile::getTopMoods(int count) const {
    vector<pair<string,int>> sorted(m_moodPlayCounts.begin(), m_moodPlayCounts.end());
    sort(sorted.begin(), sorted.end(),
         [](const auto& a, const auto& b){ return a.second > b.second; });

    vector<string> result;
    for (int i = 0; i < min(count, (int)sorted.size()); ++i)
        result.push_back(sorted[i].first);
    return result;
}

// ── Ratings 

void UserProfile::rateSong(int songId, int rating) {
    if (rating >= 0 && rating <= 5)
        m_songRatings[songId] = rating;
}

int UserProfile::getSongRating(int songId) const {
    auto it = m_songRatings.find(songId);
    return (it != m_songRatings.end()) ? it->second : 0;
}

// ── Persistence 

bool UserProfile::saveToFile(const string& path) const {
    ofstream file(path);
    if (!file.is_open()) return false;

    file << "username:" << m_username << "\n";

    for (const auto& [genre, count] : m_genrePlayCounts)
        file << "genre:" << genre << ":" << count << "\n";

    for (const auto& [mood, count] : m_moodPlayCounts)
        file << "mood:" << mood << ":" << count << "\n";

    for (const auto& [id, rating] : m_songRatings)
        file << "rating:" << id << ":" << rating << "\n";

    for (const auto& song : m_favoriteSongs)
        file << "favorite:" << song->getId() << "\n";

    return true;
}

bool UserProfile::loadFromFile(const string& path) {
    ifstream file(path);
    if (!file.is_open()) return false;

    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string key;
        getline(ss, key, ':');

        if (key == "username") {
            getline(ss, m_username);
        } else if (key == "genre") {
            string genre, countStr;
            getline(ss, genre, ':');
            getline(ss, countStr);
            if (!countStr.empty()) m_genrePlayCounts[genre] = stoi(countStr);
        } else if (key == "mood") {
            string mood, countStr;
            getline(ss, mood, ':');
            getline(ss, countStr);
            if (!countStr.empty()) m_moodPlayCounts[mood] = stoi(countStr);
        } else if (key == "rating") {
            string idStr, ratingStr;
            getline(ss, idStr, ':');
            getline(ss, ratingStr);
            if (!idStr.empty() && !ratingStr.empty())
                m_songRatings[stoi(idStr)] = stoi(ratingStr);
        }
    }
    return true;
}