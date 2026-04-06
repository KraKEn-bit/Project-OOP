#ifndef RECOMMENDER_HPP
#define RECOMMENDER_HPP

#include <vector>
#include <memory>
#include <map>
#include <string>
#include <algorithm>
#include "Song.hpp"
#include "SongLibrary.hpp"
#include "UserProfile.hpp"

using namespace std;


class Recommender {
public:
    explicit Recommender(SongLibrary* library, UserProfile* userProfile);
    ~Recommender();
    
    // Main recommendation method
    vector<shared_ptr<Song>> getRecommendations(int count = 10);
    
    // Specific recommendation strategies
    vector<shared_ptr<Song>> getContentBasedRecommendations(int count);
    vector<shared_ptr<Song>> getMoodBasedRecommendations(const string& mood, int count);
    vector<shared_ptr<Song>> getGenreBasedRecommendations(const string& genre, int count);
    vector<shared_ptr<Song>> getArtistBasedRecommendations(const string& artist, int count);
    vector<shared_ptr<Song>> getSimilarSongs(shared_ptr<Song> song, int count);
    
    // Discovery recommendations
    vector<shared_ptr<Song>> getDiscoverWeekly(int count);
    vector<shared_ptr<Song>> getPopularSongs(int count);
    vector<shared_ptr<Song>> getNewReleases(int count);
    
    // Playlist generators
    vector<shared_ptr<Song>> generatePlaylist(const string& theme, int count);
    vector<shared_ptr<Song>> generateWorkoutPlaylist(int count);
    vector<shared_ptr<Song>> generateStudyPlaylist(int count);
    vector<shared_ptr<Song>> generatePartyPlaylist(int count);
    
    // Configuration
    void setStrategy(const string& strategy);
    void updateWeights(double genreWeight, double moodWeight, double ratingWeight);
    
private:
    SongLibrary* m_library;
    UserProfile* m_userProfile;
    
    // Weights for hybrid recommendation
    double m_genreWeight;
    double m_moodWeight;
    double m_ratingWeight;
    double m_popularityWeight;
    
    string m_strategy; // "content", "collaborative", "hybrid"
    
    // Helper methods
    double calculateSongScore(shared_ptr<Song> song);
    double calculateGenreSimilarity(shared_ptr<Song> song1, shared_ptr<Song> song2);
    double calculateMoodSimilarity(const string& mood1, const string& mood2);
    vector<string> getUserTopGenres() const;
    vector<string> getUserTopMoods() const;
    
    // Filtering methods
    vector<shared_ptr<Song>> filterByGenre(const vector<shared_ptr<Song>>& songs, const string& genre);
    
    vector<shared_ptr<Song>> filterByMood(const vector<shared_ptr<Song>>& songs, const string& mood);
    
    vector<shared_ptr<Song>> filterByRating(const vector<shared_ptr<Song>>& songs, int minRating);
    
    vector<shared_ptr<Song>> excludeListenedSongs(const vector<shared_ptr<Song>>& songs);
    
    // Sorting methods
    void sortByScore(vector<shared_ptr<Song>>& songs);
    void sortByPopularity(vector<shared_ptr<Song>>& songs);
    void sortByRating(vector<shared_ptr<Song>>& songs);
    
    // Scoring algorithms
    struct SongScore {
        shared_ptr<Song> song;
        double score;
        
        bool operator<(const SongScore& other) const {
            return score > other.score; // Descending order
        }
    };
    
    vector<SongScore> calculateScores(const vector<shared_ptr<Song>>& songs);
};

#endif 
