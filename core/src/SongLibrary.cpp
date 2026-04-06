#include "SongLibrary.hpp"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <algorithm>

using namespace std;


SongLibrary::SongLibrary() {
}

SongLibrary::~SongLibrary() {
}

bool SongLibrary::loadFromCSV(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open CSV file:" << filePath;
        return false;
    }
    
    QTextStream in(&file);
    
    // Skip header line
    if (!in.atEnd()) {
        in.readLine();
    }
    
    int loadedCount = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) continue;
        
        // Parse CSV line
        QStringList fields = line.split(',');
        if (fields.size() < 9) continue; // Need at least 9 fields
        
        try {
            // Extract fields
            int id = fields[0].toInt();
            QString title = fields[1];
            QString artistName = fields[2];
            QString albumName = fields[3];
            int duration = fields[4].toInt();
            QString genreName = fields[5];
            QString mood = fields[6];
            int rating = fields[7].toInt();
            int playCount = fields[8].toInt();
            
            QString filePath = "";
            bool isFavorite = false;
            
            if (fields.size() > 9) {
                filePath = fields[9];
            }
            if (fields.size() > 10) {
                isFavorite = (fields[10].toLower() == "true");
            }
            
            // Get or create artist
            auto artist = getOrCreateArtist(artistName);
            
            // Get or create album
            auto album = getOrCreateAlbum(albumName, artist);
            
            // Create song
            auto song = make_shared<Song>(id, title.toStdString(), artist, album, duration);
            song->setRating(rating);
            song->setFilePath(filePath.toStdString());
            song->setMood(mood.toStdString());
            song->setFavorite(isFavorite);
            
            // Set play count manually
            for (int i = 0; i < playCount; ++i) {
                song->incrementPlayCount();
            }
            
            // Add genre
            auto genre = getOrCreateGenre(genreName);
            song->addGenre(genre);
            
            // Add to library
            m_songs.push_back(song);
            album->addTrack(song);
            
            loadedCount++;
            
        } catch (...) {
            qDebug() << "Error parsing line:" << line;
            continue;
        }
    }
    
    file.close();
    
    qDebug() << "Loaded" << loadedCount << "songs from CSV";
    return loadedCount > 0;
}

vector<shared_ptr<Song>> SongLibrary::getAllSongs() const {
    return m_songs;
}

vector<shared_ptr<Song>> SongLibrary::searchByTitle(const QString& title) const {
    vector<shared_ptr<Song>> results;
    QString searchLower = title.toLower();
    
    for (const auto& song : m_songs) {
        QString songTitle = QString::fromStdString(song->getTitle()).toLower();
        if (songTitle.contains(searchLower)) {
            results.push_back(song);
        }
    }
    
    return results;
}

vector<shared_ptr<Song>> SongLibrary::searchByArtist(const QString& artistName) const {
    vector<shared_ptr<Song>> results;
    QString searchLower = artistName.toLower();
    
    for (const auto& song : m_songs) {
        if (song->getArtist()) {
            QString songArtist = QString::fromStdString(song->getArtist()->getName()).toLower();
            if (songArtist.contains(searchLower)) {
                results.push_back(song);
            }
        }
    }
    
    return results;
}

vector<shared_ptr<Song>> SongLibrary::filterByGenre(const QString& genre) const {
    vector<shared_ptr<Song>> results;
    
    for (const auto& song : m_songs) {
        if (song->hasGenre(genre.toStdString())) {
            results.push_back(song);
        }
    }
    
    return results;
}

vector<shared_ptr<Song>> SongLibrary::filterByMood(const QString& mood) const {
    vector<shared_ptr<Song>> results;
    QString moodLower = mood.toLower();
    
    for (const auto& song : m_songs) {
        QString songMood = QString::fromStdString(song->getMood()).toLower();
        if (songMood == moodLower) {
            results.push_back(song);
        }
    }
    
    return results;
}

shared_ptr<Song> SongLibrary::getSongById(int id) const {
    for (const auto& song : m_songs) {
        if (song->getId() == id) {
            return song;
        }
    }
    return nullptr;
}

shared_ptr<Artist> SongLibrary::getOrCreateArtist(const QString& name) {
    // Check if artist already exists
    for (const auto& artist : m_artists) {
        if (QString::fromStdString(artist->getName()) == name) {
            return artist;
        }
    }
    
    // Create new artist
    int id = m_artists.size() + 1;
    auto artist = make_shared<Artist>(id, name.toStdString());
    m_artists.push_back(artist);
    return artist;
}

shared_ptr<Album> SongLibrary::getOrCreateAlbum(const QString& title, shared_ptr<Artist> artist) {
    // Check if album already exists
    for (const auto& album : m_albums) {
        if (QString::fromStdString(album->getTitle()) == title &&
            album->getArtist() == artist) {
            return album;
        }
    }
    
    // Create new album
    int id = m_albums.size() + 1;
    auto album = make_shared<Album>(id, title.toStdString(), artist);
    m_albums.push_back(album);
    artist->addAlbum(album);
    return album;
}

shared_ptr<Genre> SongLibrary::getOrCreateGenre(const QString& name) {
    // Check if genre already exists
    for (const auto& genre : m_genres) {
        if (QString::fromStdString(genre->getName()) == name) {
            return genre;
        }
    }
    
    // Creating new genre
    int id = m_genres.size() + 1;
    auto genre = make_shared<Genre>(id, name.toStdString());
    m_genres.push_back(genre);
    return genre;
}