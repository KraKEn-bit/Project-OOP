#include "Song.hpp"
#include <algorithm>
#include <sstream>
#include <cmath>


using namespace std;



// Song Implementation
Song::Song(int id, const string& title, shared_ptr<Artist> artist,shared_ptr<Album> album, int duration)
    : m_id(id)
    , m_title(title)
    , m_artist(artist)
    , m_album(album)
    , m_duration(duration)
    , m_rating(0)
    , m_playCount(0)
    , m_filePath("")
    , m_mood("neutral")
    , m_isFavorite(false)
{
}

Song::~Song() = default;

Song::Song(const Song& other)
    : m_id(other.m_id)
    , m_title(other.m_title)
    , m_artist(other.m_artist)
    , m_album(other.m_album)
    , m_duration(other.m_duration)
    , m_rating(other.m_rating)
    , m_playCount(other.m_playCount)
    , m_filePath(other.m_filePath)
    , m_mood(other.m_mood)
    , m_genres(other.m_genres)
    , m_isFavorite(other.m_isFavorite)
{}

Song& Song::operator=(const Song& other) {
    if (this != &other) {
        m_id = other.m_id;
        m_title = other.m_title;
        m_artist = other.m_artist;
        m_album = other.m_album;
        m_duration = other.m_duration;
        m_rating = other.m_rating;
        m_playCount = other.m_playCount;
        m_filePath = other.m_filePath;
        m_mood = other.m_mood;
        m_genres = other.m_genres;
        m_isFavorite = other.m_isFavorite;
    }
    return *this;
}

Song::Song(Song&& other) noexcept
    : m_id(other.m_id)
    , m_title(move(other.m_title))
    , m_artist(move(other.m_artist))
    , m_album(move(other.m_album))
    , m_duration(other.m_duration)
    , m_rating(other.m_rating)
    , m_playCount(other.m_playCount)
    , m_filePath(move(other.m_filePath))
    , m_mood(move(other.m_mood))
    , m_genres(move(other.m_genres))
    , m_isFavorite(other.m_isFavorite)
{}

Song& Song::operator=(Song&& other) noexcept {
    if (this != &other) {
        m_id = other.m_id;
        m_title = move(other.m_title);
        m_artist = move(other.m_artist);
        m_album = move(other.m_album);
        m_duration = other.m_duration;
        m_rating = other.m_rating;
        m_playCount = other.m_playCount;
        m_filePath = move(other.m_filePath);
        m_mood = move(other.m_mood);
        m_genres = move(other.m_genres);
        m_isFavorite = other.m_isFavorite;
    }
    return *this;
}

void Song::setRating(int rating) {
    validateRating(rating);
    m_rating = rating;
}

void Song::incrementPlayCount() {
    m_playCount++;
    if (m_artist) {
        m_artist->incrementPlayCount();
    }
}

void Song::addGenre(shared_ptr<Genre> genre) {
    if (genre && !hasGenre(genre->getName())) {
        m_genres.push_back(genre);
        genre->addSong(make_shared<Song>(*this));
    }
}

void Song::removeGenre(shared_ptr<Genre> genre) {
    m_genres.erase(
        remove_if(m_genres.begin(), m_genres.end(),
                      [&genre](const shared_ptr<Genre>& g) {
                          return g->getId() == genre->getId();
                      }),
        m_genres.end()
    );
}

bool Song::hasGenre(const string& genreName) const {
    return any_of(m_genres.begin(), m_genres.end(),
                      [&genreName](const shared_ptr<Genre>& g) {
                          return g->getName() == genreName;
                      });
}

string Song::getFormattedDuration() const {
    int hours = m_duration / 3600;
    int minutes = (m_duration % 3600) / 60;
    int seconds = m_duration % 60;
    
    ostringstream oss;
    if (hours > 0) {
        oss << hours << ":" << (minutes < 10 ? "0" : "") << minutes << ":"<< (seconds < 10 ? "0" : "") << seconds;
    } 
    else {
        oss << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
    }
    return oss.str();
}

double Song::calculateSimilarity(const Song& other) const {
    double similarity = 0.0;
    int factors = 0;
    
    // Genre similarity
    int commonGenres = 0;
    for (const auto& genre : m_genres) {
        if (other.hasGenre(genre->getName())) {
            commonGenres++;
        }
    }
    if (!m_genres.empty() && !other.m_genres.empty()) {
        similarity += static_cast<double>(commonGenres) / 
                     max(m_genres.size(), other.m_genres.size());
        factors++;
    }
    
    // Mood similarity
    if (m_mood == other.m_mood) {
        similarity += 1.0;
        factors++;
    }
    
    // Artist similarity
    if (m_artist && other.m_artist && m_artist->getId() == other.m_artist->getId()) {
        similarity += 1.0;
        factors++;
    }
    
    // Duration similarity 
    if (abs(m_duration - other.m_duration) <= 30) {
        similarity += 0.5;
        factors++;
    }
    
    return factors > 0 ? similarity / factors : 0.0;
}

bool Song::operator==(const Song& other) const {
    return m_id == other.m_id;
}

bool Song::operator<(const Song& other) const {
    return m_title < other.m_title;
}

void Song::validateRating(int rating) const {
    if (rating < 0 || rating > 5) {
        throw invalid_argument("Rating must be between 0 and 5");
    }
}

ostream& operator<<(ostream& os, const Song& song) {
    os << song.m_id << ","
       << song.m_title << ","
       << (song.m_artist ? song.m_artist->getName() : "Unknown") << ","
       << (song.m_album ? song.m_album->getTitle() : "Unknown") << ","
       << song.m_duration << ","
       << song.m_rating << ","
       << song.m_playCount;
    return os;
}


// Artist Implementation

Artist::Artist(int id, const string& name)
    : m_id(id)
    , m_name(name)
    , m_playCount(0) {}

Artist::~Artist() = default;

void Artist::addAlbum(shared_ptr<Album> album) {
    if (album) {
        m_albums.push_back(album);
    }
}

void Artist::incrementPlayCount() {
    m_playCount++;
}

vector<shared_ptr<Song>> Artist::getAllSongs() const {
    vector<shared_ptr<Song>> allSongs;
    for (const auto& album : m_albums) {
        auto tracks = album->getTracks();
        allSongs.insert(allSongs.end(), tracks.begin(), tracks.end());
    }
    return allSongs;
}


// Album Implementation

Album::Album(int id, const string& title, shared_ptr<Artist> artist)
    : m_id(id)
    , m_title(title)
    , m_artist(artist)
    , m_releaseYear(2024) {}

Album::~Album() = default;

void Album::addTrack(shared_ptr<Song> song) {
    if (song) {
        m_tracks.push_back(song);
    }
}

int Album::getTotalDuration() const {
    int total = 0;
    for (const auto& track : m_tracks) {
        total += track->getDuration();
    }
    return total;
}

double Album::getAverageRating() const {
    if (m_tracks.empty()) return 0.0;
    
    int totalRating = 0;
    for (const auto& track : m_tracks) {
        totalRating += track->getRating();
    }
    return static_cast<double>(totalRating) / m_tracks.size();
}




// Genre Implementation

Genre::Genre(int id, const string& name)
    : m_id(id)
    , m_name(name)
    , m_playCount(0) {}

Genre::~Genre() = default;

void Genre::addSong(shared_ptr<Song> song) {
    if (song) {
        m_songs.push_back(song);
    }
}

void Genre::incrementPlayCount() {
    m_playCount++;
}
