#ifndef RECOMMENDATIONPANELWIDGET_HPP
#define RECOMMENDATIONPANELWIDGET_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QPainter>
#include <QString>
#include <QVector>

class RecommendationPanelWidget : public QWidget {
    Q_OBJECT

public:
    explicit RecommendationPanelWidget(QWidget *parent = nullptr);

    void setRecommendedSongs(const QVector<QPair<QString,QString>>& songs); // title, artist

signals:
    void moodSelected(const QString& mood);
    void playlistThemeRequested(const QString& theme); // "study", "workout", "party"

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onMoodClicked(const QString& mood);

private:
    void setupUI();
    void buildMoodButtons();
    void buildThemeButtons();

    QVBoxLayout* m_outerLayout;
    QWidget*     m_moodGrid;
    QWidget*     m_themeContainer;
    QWidget*     m_songListContainer;
    QVBoxLayout* m_songListLayout;

    QString m_activeMood;

    struct MoodEntry { QString mood; QString emoji; };
    const QVector<MoodEntry> MOODS = {
        { "calm",      "🌊" },
        { "happy",     "☀️" },
        { "sad",       "🌧" },
        { "energetic", "⚡" },
        { "romantic",  "💖" },
        { "dramatic",  "🎭" },
        { "epic",      "🔥" },
        { "focus",     "🎯" },
    };

    // Colors
    const QColor C_BG      = QColor("#16213e");
    const QColor C_PRIMARY = QColor("#e94560");
    const QColor C_TEXT    = QColor("#ffffff");
    const QColor C_SUBTEXT = QColor("#a0a0a0");
    const QColor C_CARD    = QColor("#252d3a");
};

#endif