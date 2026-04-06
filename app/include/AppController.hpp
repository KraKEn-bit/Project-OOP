#ifndef APPCONTROLLER_HPP
#define APPCONTROLLER_HPP

#include <QObject>
#include <QString>
#include <QVector>
#include "SongLibrary.hpp"
#include "UserProfile.hpp"
#include "Recommender.hpp"
#include "Playlist.hpp"
#include <memory>

using namespace std;

class AppController : public QObject {
    Q_OBJECT

public:
    explicit AppController(QObject *parent = nullptr);
    ~AppController();

    //  Library
    bool loadSongsFromCSV(const QString& path);
    int  getSongCount() const;

    vector<shared_ptr<Song>> getAllSongs()            const;
    vector<shared_ptr<Song>> searchSongs(const QString& query) const;
    vector<shared_ptr<Song>> filterByMood(const QString& mood) const;
    shared_ptr<Song>         getSongById(int id)      const;

    // ─Playback events
    void onSongPlayed(shared_ptr<Song> song);   // records play + updates recommender
    void rateSong(int songId, int rating);

    //  Recommendations
    vector<shared_ptr<Song>> getRecommendations(int count = 10);
    vector<shared_ptr<Song>> getMoodRecommendations(const string& mood, int count = 8);
    vector<shared_ptr<Song>> getThemePlaylist(const string& theme, int count = 20);
    vector<shared_ptr<Song>> getSimilarSongs(shared_ptr<Song> song, int count = 8);

    // Playlists
    void   createPlaylist(const string& name);
    void   deletePlaylist(const string& name);
    void   addSongToPlaylist(const string& playlistName, shared_ptr<Song> song);
    void   removeSongFromPlaylist(const string& playlistName, int songId);
    vector<string>           getPlaylistNames() const;
    vector<shared_ptr<Song>> getPlaylistSongs(const string& name) const;

    //  User profile
    UserProfile* getUserProfile() { return m_userProfile.get(); }
    bool saveUserProfile(const string& path = "data/user_profile.txt");
    bool loadUserProfile(const string& path = "data/user_profile.txt");

signals:
    void songsLoaded();
    void songCountChanged(int count);
    void recommendationsReady();

private:
    unique_ptr<SongLibrary>  m_library;
    unique_ptr<UserProfile>  m_userProfile;
    unique_ptr<Recommender>  m_recommender;
    vector<unique_ptr<Playlist>> m_playlists;
};

#endif