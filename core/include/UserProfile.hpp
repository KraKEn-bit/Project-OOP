#ifndef USERPROFILE_HPP
#define USERPROFILE_HPP

#include "Song.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

using namespace std;

class Playlist;

class UserProfile {
public:
    explicit UserProfile(const string& username = "Guest");
    ~UserProfile();

    // Getters
    string getUsername() const { return m_username; }
    vector<shared_ptr<Song>> getFavoriteSongs() const { return m_favoriteSongs; }
    vector<shared_ptr<Song>> getListeningHistory() const { return m_listeningHistory; }
    map<string, int> getGenrePlayCounts() const { return m_genrePlayCounts; }
    map<string, int> getMoodPlayCounts()  const { return m_moodPlayCounts;  }

    // Favorites
    void addFavoriteSong(shared_ptr<Song> song);
    void removeFavoriteSong(int songId);
    bool isFavorite(int songId) const;

    // History
    void addToHistory(shared_ptr<Song> song);
    void clearHistory();
    bool hasListened(int songId) const;

    // Play recording 
    void recordPlay(shared_ptr<Song> song);

    // Top genres / moods derived from play history
    vector<string> getTopGenres(int count = 3) const;
    vector<string> getTopMoods (int count = 3) const;

    // Per-song ratings
    void rateSong(int songId, int rating);
    int  getSongRating(int songId) const;

    // Persistence
    bool saveToFile(const string& path) const;
    bool loadFromFile(const string& path);

private:
    string m_username;
    vector<shared_ptr<Song>> m_favoriteSongs;
    vector<shared_ptr<Song>> m_listeningHistory;
    map<string, int> m_genrePlayCounts;
    map<string, int> m_moodPlayCounts;
    map<int, int>    m_songRatings;   // songId -> rating
    int m_maxHistorySize;
};

#endif 