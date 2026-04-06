#include "Recommender.hpp"
#include <algorithm>
#include <cmath>

using namespace std;



Recommender::Recommender(SongLibrary* library, UserProfile* userProfile)
    : m_library(library)
    , m_userProfile(userProfile)
    , m_genreWeight(0.40)
    , m_moodWeight(0.30)
    , m_ratingWeight(0.20)
    , m_popularityWeight(0.10)
    , m_strategy("hybrid")
{}

Recommender::~Recommender() = default;



vector<shared_ptr<Song>> Recommender::getRecommendations(int count) {
    return getContentBasedRecommendations(count);
}



vector<shared_ptr<Song>> Recommender::getContentBasedRecommendations(int count) {
    auto all    = m_library->getAllSongs();
    auto scored = calculateScores(all);
    sort(scored.begin(), scored.end());

    vector<shared_ptr<Song>> result;
    result.reserve(min(count, (int)scored.size()));
    for (int i = 0; i < min(count, (int)scored.size()); ++i)
        result.push_back(scored[i].song);
    return result;
}


vector<shared_ptr<Song>> Recommender::getMoodBasedRecommendations(const string& mood, int count) {
    auto songs = m_library->filterByMood(QString::fromStdString(mood));
    sortByRating(songs);
    if ((int)songs.size() > count) songs.resize(count);
    return songs;
}

//Genre-based

vector<shared_ptr<Song>> Recommender::getGenreBasedRecommendations(const string& genre, int count) {
    auto songs = m_library->filterByGenre(QString::fromStdString(genre));
    sortByRating(songs);
    if ((int)songs.size() > count) songs.resize(count);
    return songs;
}

//  Artist-based

vector<shared_ptr<Song>> Recommender::getArtistBasedRecommendations(const string& artist, int count) {
    auto songs = m_library->searchByArtist(QString::fromStdString(artist));
    sortByRating(songs);
    if ((int)songs.size() > count) songs.resize(count);
    return songs;
}

// Similar songs

vector<shared_ptr<Song>> Recommender::getSimilarSongs(shared_ptr<Song> song, int count) {
    if (!song) return {};

    auto all = m_library->getAllSongs();
    vector<pair<double, shared_ptr<Song>>> scored;
    scored.reserve(all.size());

    for (const auto& s : all) {
        if (s->getId() == song->getId()) continue;
        scored.push_back({ song->calculateSimilarity(*s), s });
    }

    sort(scored.begin(), scored.end(),
         [](const auto& a, const auto& b){ return a.first > b.first; });

    vector<shared_ptr<Song>> result;
    for (int i = 0; i < min(count, (int)scored.size()); ++i)
        result.push_back(scored[i].second);
    return result;
}

// Discovery

vector<shared_ptr<Song>> Recommender::getDiscoverWeekly(int count) {
    auto all     = m_library->getAllSongs();
    auto unheard = excludeListenedSongs(all);
    auto scored  = calculateScores(unheard);
    sort(scored.begin(), scored.end());

    vector<shared_ptr<Song>> result;
    for (int i = 0; i < min(count, (int)scored.size()); ++i)
        result.push_back(scored[i].song);
    return result;
}

vector<shared_ptr<Song>> Recommender::getPopularSongs(int count) {
    auto songs = m_library->getAllSongs();
    sortByPopularity(songs);
    if ((int)songs.size() > count) songs.resize(count);
    return songs;
}

vector<shared_ptr<Song>> Recommender::getNewReleases(int count) {

    auto songs = m_library->getAllSongs();
    sort(songs.begin(), songs.end(),
         [](const auto& a, const auto& b){ return a->getId() > b->getId(); });
    if ((int)songs.size() > count) songs.resize(count);
    return songs;
}

// Playlist generators

vector<shared_ptr<Song>> Recommender::generatePlaylist(const string& theme, int count) {
    if (theme == "workout") return generateWorkoutPlaylist(count);
    if (theme == "study")   return generateStudyPlaylist(count);
    if (theme == "party")   return generatePartyPlaylist(count);
    return getRecommendations(count);
}

vector<shared_ptr<Song>> Recommender::generateWorkoutPlaylist(int count) {
    auto energetic = getMoodBasedRecommendations("energetic", count);
    auto epic      = getMoodBasedRecommendations("epic",      count);
    for (const auto& s : epic) {
        if (!any_of(energetic.begin(), energetic.end(),
                    [&s](const auto& e){ return e->getId() == s->getId(); }))
            energetic.push_back(s);
    }
    sortByRating(energetic);
    if ((int)energetic.size() > count) energetic.resize(count);
    return energetic;
}

vector<shared_ptr<Song>> Recommender::generateStudyPlaylist(int count) {
    auto calm  = getMoodBasedRecommendations("calm",  count);
    auto focus = getMoodBasedRecommendations("focus", count);
    for (const auto& s : focus) {
        if (!any_of(calm.begin(), calm.end(),
                    [&s](const auto& c){ return c->getId() == s->getId(); }))
            calm.push_back(s);
    }
    sortByRating(calm);
    if ((int)calm.size() > count) calm.resize(count);
    return calm;
}

vector<shared_ptr<Song>> Recommender::generatePartyPlaylist(int count) {
    auto result = getMoodBasedRecommendations("energetic", count);
    for (const string& mood : { "happy", "dramatic" }) {
        auto extra = getMoodBasedRecommendations(mood, count);
        for (const auto& s : extra) {
            if (!any_of(result.begin(), result.end(),
                        [&s](const auto& r){ return r->getId() == s->getId(); }))
                result.push_back(s);
        }
    }
    sortByRating(result);
    if ((int)result.size() > count) result.resize(count);
    return result;
}


void Recommender::setStrategy(const string& strategy) { m_strategy = strategy; }

void Recommender::updateWeights(double genreWeight, double moodWeight, double ratingWeight) {
    m_genreWeight   = genreWeight;
    m_moodWeight    = moodWeight;
    m_ratingWeight  = ratingWeight;
}



double Recommender::calculateSongScore(shared_ptr<Song> song) {
    if (!song) return 0.0;

    double score = 0.0;

    score += m_ratingWeight * (song->getRating() / 5.0);


    score += m_popularityWeight * min(1.0, song->getPlayCount() / 50.0);

    if (m_userProfile) {
        // Genre component – reward songs in the user's top listened genres
        auto topGenres = m_userProfile->getTopGenres(5);
        for (const auto& genre : song->getGenres()) {
            if (any_of(topGenres.begin(), topGenres.end(),
                       [&](const string& g){ return g == genre->getName(); })) {
                score += m_genreWeight;
                break;
            }
        }

        auto topMoods = m_userProfile->getTopMoods(3);
        if (any_of(topMoods.begin(), topMoods.end(),
                   [&](const string& m){ return m == song->getMood(); }))
            score += m_moodWeight;
    } else {

        score += m_genreWeight * (song->getRating() / 5.0);
    }

    return score;
}

double Recommender::calculateGenreSimilarity(shared_ptr<Song> s1, shared_ptr<Song> s2) {
    if (!s1 || !s2) return 0.0;
    return s1->calculateSimilarity(*s2);
}

double Recommender::calculateMoodSimilarity(const string& mood1, const string& mood2) {
    return (mood1 == mood2) ? 1.0 : 0.0;
}

vector<string> Recommender::getUserTopGenres() const {
    return m_userProfile ? m_userProfile->getTopGenres(5) : vector<string>{};
}

vector<string> Recommender::getUserTopMoods() const {
    return m_userProfile ? m_userProfile->getTopMoods(3) : vector<string>{};
}

vector<shared_ptr<Song>> Recommender::filterByGenre(
        const vector<shared_ptr<Song>>& songs, const string& genre) {
    vector<shared_ptr<Song>> result;
    for (const auto& s : songs)
        if (s->hasGenre(genre)) result.push_back(s);
    return result;
}

vector<shared_ptr<Song>> Recommender::filterByMood(
        const vector<shared_ptr<Song>>& songs, const string& mood) {
    vector<shared_ptr<Song>> result;
    for (const auto& s : songs)
        if (s->getMood() == mood) result.push_back(s);
    return result;
}

vector<shared_ptr<Song>> Recommender::filterByRating(
        const vector<shared_ptr<Song>>& songs, int minRating) {
    vector<shared_ptr<Song>> result;
    for (const auto& s : songs)
        if (s->getRating() >= minRating) result.push_back(s);
    return result;
}

vector<shared_ptr<Song>> Recommender::excludeListenedSongs(
        const vector<shared_ptr<Song>>& songs) {
    if (!m_userProfile) return songs;
    vector<shared_ptr<Song>> result;
    for (const auto& s : songs)
        if (!m_userProfile->hasListened(s->getId())) result.push_back(s);
    return result;
}

void Recommender::sortByScore(vector<shared_ptr<Song>>& songs) {
    sort(songs.begin(), songs.end(), [this](const auto& a, const auto& b){
        return calculateSongScore(a) > calculateSongScore(b);
    });
}

void Recommender::sortByPopularity(vector<shared_ptr<Song>>& songs) {
    sort(songs.begin(), songs.end(), [](const auto& a, const auto& b){
        return a->getPlayCount() > b->getPlayCount();
    });
}

void Recommender::sortByRating(vector<shared_ptr<Song>>& songs) {
    sort(songs.begin(), songs.end(), [](const auto& a, const auto& b){
        return a->getRating() > b->getRating();
    });
}

vector<Recommender::SongScore> Recommender::calculateScores(
        const vector<shared_ptr<Song>>& songs) {
    vector<SongScore> scores;
    scores.reserve(songs.size());
    for (const auto& s : songs)
        scores.push_back({ s, calculateSongScore(s) });
    return scores;
}