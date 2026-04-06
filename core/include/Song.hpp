#ifndef SONG_HPP
#define SONG_HPP

#include <string>
#include <memory>
#include <vector>

using namespace std;

// Forward declarations
class Artist;
class Album;
class Genre;


class Song {
public:

    Song(int id, const string& title, shared_ptr<Artist> artist,
         shared_ptr<Album> album, int duration);
    
    ~Song();
    
  
    Song(const Song& other);
    
    Song& operator=(const Song& other);
    

    Song(Song&& other) noexcept;
    
    Song& operator=(Song&& other) noexcept;
    
    
    int getId() const { 
        return m_id; 
    }

    string getTitle() const { 
        return m_title; 
    }

    shared_ptr<Artist> getArtist() const { 
        return m_artist; 
    }

    shared_ptr<Album> getAlbum() const { 
        return m_album; 
    }

    int getDuration() const { 
        return m_duration; 
    }


    int getRating() const { 
        return m_rating; 
    }

    int getPlayCount() const { 
        return m_playCount; 
    }

    string getFilePath() const { 
        return m_filePath; 
    }


    string getMood() const { 
        return m_mood; 
    }


    vector<shared_ptr<Genre>> getGenres() const { 
        return m_genres; 
    }


    bool isFavorite() const { 
        return m_isFavorite; 
    }

    
    // Setters
    void setTitle(const string& title) { 
        m_title = title; 
    }

    void setArtist(shared_ptr<Artist> artist) { 
        m_artist = artist; 
    }

    void setAlbum(shared_ptr<Album> album) { 
        m_album = album; 
    }

    void setDuration(int duration) { 
        m_duration = duration; 
    }

    void setRating(int rating);

    void setFilePath(const string& path) { 
        m_filePath = path; 
    }

    void setMood(const string& mood) { 
        m_mood = mood; 
    }

    void setFavorite(bool favorite) { 
        m_isFavorite = favorite; 
    }

    
    // Behavior methods
    void incrementPlayCount();
    void addGenre(shared_ptr<Genre> genre);
    void removeGenre(shared_ptr<Genre> genre);
    bool hasGenre(const string& genreName) const;
    
    
    // Utility methods
    string getFormattedDuration() const;
    double calculateSimilarity(const Song& other) const;
    
    
    // Operator overloading for comparison
    bool operator==(const Song& other) const;
    bool operator<(const Song& other) const; // For sorting
    

    // Friend functions for serialization
    friend ostream& operator<<(ostream& os, const Song& song);
    friend istream& operator>>(istream& is, Song& song);
    
private:
    int m_id;
    string m_title;
    shared_ptr<Artist> m_artist;
    shared_ptr<Album> m_album;
    int m_duration; 
    int m_rating; // 0-5 stars
    int m_playCount;
    string m_filePath;
    string m_mood; 
    vector<shared_ptr<Genre>> m_genres;
    bool m_isFavorite;
    
    // Helper methods
    void validateRating(int rating) const;
};


//Artist class represents a music artist

class Artist {
public:
    Artist(int id, const string& name);
    ~Artist();
    
    // Getters
    int getId() const { 
        return m_id; 
    }

    string getName() const { 
        return m_name; 
    }

    int getPlayCount() const { 
        return m_playCount; 
    }

    vector<shared_ptr<Album>> getDiscography() const { 
        return m_albums; 
    }
    
    // Setters
    void setName(const string& name) { 
        m_name = name; 
    }
    
    // Methods
    void addAlbum(shared_ptr<Album> album);
    void incrementPlayCount();
    vector<shared_ptr<Song>> getAllSongs() const;
    
private:
    int m_id;
    string m_name;
    int m_playCount;
    vector<shared_ptr<Album>> m_albums;
};



//Album class represents a music album

class Album {
public:
    Album(int id, const string& title, shared_ptr<Artist> artist);
    ~Album();
    
    // Getters
    int getId() const { 
        return m_id; 
    }
    
    string getTitle() const { 
        return m_title; 
    }

    shared_ptr<Artist> getArtist() const { 
        return m_artist; 
    }

    vector<shared_ptr<Song>> getTracks() const { 
        return m_tracks; 
    }

    int getReleaseYear() const { 
        return m_releaseYear; 
    }

    
    // Setters
    void setTitle(const string& title) { 
        m_title = title; 
    }

    void setReleaseYear(int year) { 
        m_releaseYear = year; 
    }
    
    // Methods
    void addTrack(shared_ptr<Song> song);
    int getTotalDuration() const;
    double getAverageRating() const;
    
private:
    int m_id;
    string m_title;
    shared_ptr<Artist> m_artist;
    vector<shared_ptr<Song>> m_tracks;
    int m_releaseYear;
};


//Genre class represents a music genre

class Genre {
public:
    Genre(int id, const string& name);
    ~Genre();
    
    // Getters
    int getId() const {
        return m_id; 
    }

    string getName() const { 
        return m_name; 
    }

    int getPlayCount() const { 
        return m_playCount; 
    }

    
    // Methods
    void addSong(shared_ptr<Song> song);
    void incrementPlayCount();
    vector<shared_ptr<Song>> getSongs() const { 
        return m_songs; 
    }
    
private:
    int m_id;
    string m_name;
    int m_playCount;
    vector<shared_ptr<Song>> m_songs;
};

#endif 
