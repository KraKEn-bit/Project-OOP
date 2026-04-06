#ifndef SONGLIBRARY_HPP
#define SONGLIBRARY_HPP

#include "Song.hpp"
#include <vector>
#include <memory>
#include <string>
#include <QString>

using namespace std;


//SongLibrary - Manages collection of songs

class SongLibrary {
public:
    SongLibrary();
    ~SongLibrary();
    
    // Loading songs from CSV file
    bool loadFromCSV(const QString& filePath);
    
    // Get all songs
    vector<shared_ptr<Song>> getAllSongs() const;
    
    // Searching songs
    vector<shared_ptr<Song>> searchByTitle(const QString& title) const;
    vector<shared_ptr<Song>> searchByArtist(const QString& artistName) const;
    vector<shared_ptr<Song>> filterByGenre(const QString& genre) const;
    vector<shared_ptr<Song>> filterByMood(const QString& mood) const;
    
    // Getting song by ID
    shared_ptr<Song> getSongById(int id) const;
    
    // Statistics
    int getSongCount() const { return m_songs.size(); }
    
private:
    vector<shared_ptr<Song>> m_songs;
    vector<shared_ptr<Artist>> m_artists;
    vector<shared_ptr<Album>> m_albums;
    vector<shared_ptr<Genre>> m_genres;
    
    // Helper methods
    shared_ptr<Artist> getOrCreateArtist(const QString& name);
    shared_ptr<Album> getOrCreateAlbum(const QString& title, shared_ptr<Artist> artist);
    shared_ptr<Genre> getOrCreateGenre(const QString& name);
};

#endif 