#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include "Song.hpp"
#include <string>
#include <vector>
#include <memory>

using namespace std;

class Playlist {
public:
    explicit Playlist(const string& name);
    ~Playlist();

    // ── Getters 
    string getName() const { return m_name; }
    vector<shared_ptr<Song>> getSongs() const { return m_songs; }
    int getSongCount()   const { return static_cast<int>(m_songs.size()); }
    int getTotalDuration() const;

    // ── Setters 
    void setName(const string& name) { m_name = name; }

    // ── Song management
    void addSong(shared_ptr<Song> song);
    void removeSong(int songId);
    void moveSong(int fromIndex, int toIndex);  
    bool containsSong(int songId) const;
    void clear();

    // ── Persistence 
   
    bool saveToFile(const string& path) const;

private:
    string m_name;
    vector<shared_ptr<Song>> m_songs;
};

#endif 