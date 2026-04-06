#ifndef SONGROWWIDGET_HPP
#define SONGROWWIDGET_HPP

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QMouseEvent>

using namespace std;


 //SongRowWidget
 

class SongRowWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
    Q_PROPERTY(qreal hoverProgress READ hoverProgress WRITE setHoverProgress)
    
public:
    explicit SongRowWidget(QWidget *parent = nullptr);
    
    void setSongInfo(const QString& title, const QString& artist, const QString& album, int duration, int rating);
    void setPlaying(bool playing);
    void setFavorite(bool favorite);
    
    void setSongId(int id) { 
        m_songId = id; 
    }
    
    int getSongId() const { 
        return m_songId; 
    }

    bool isPlaying() const { 
        return m_isPlaying; 
    }

    bool isFavorite() const { 
        return m_isFavorite; 
    }
    
    qreal opacity() const { 
        return m_opacity; 
    }

    void setOpacity(qreal opacity) { 
        m_opacity = opacity; 
        update(); 
    }
    
    qreal hoverProgress() const { 
        return m_hoverProgress; 
    }

    void setHoverProgress(qreal progress) { 
        m_hoverProgress = progress; 
        update(); 
    }
    
signals:
    void clicked();
    void doubleClicked();
    void playRequested();
    void ratingChanged(int newRating);
    void favoriteToggled();
    void addToPlaylistRequested();
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    
private:
    void setupUI();
    void createAnimations();
    void drawPlayIndicator(QPainter& painter, const QRect& rect);
    void drawAlbumArt(QPainter& painter, const QRect& rect);
    void drawRatingStars(QPainter& painter, const QRect& rect);
    QString formatDuration(int seconds) const;
    
    // Song data
    int m_songId;
    QString m_title;
    QString m_artist;
    QString m_album;
    int m_duration;
    int m_rating;
    bool m_isPlaying;
    bool m_isFavorite;
    
    // UI state
    qreal m_opacity;
    qreal m_hoverProgress;
    bool m_isHovered;
    int m_hoveredStar;
    
    // Animations
    QPropertyAnimation* m_hoverAnimation;
    QTimer* m_waveformTimer;
    vector<qreal> m_waveformHeights;
    int m_waveformPhase;
    
    // Colors
    const QColor COLOR_BG_HOVER = QColor("#2d3748");
    const QColor COLOR_PRIMARY = QColor("#e94560");
    const QColor COLOR_TEXT_PRIMARY = QColor("#ffffff");
    const QColor COLOR_TEXT_SECONDARY = QColor("#a0a0a0");
    const QColor COLOR_STAR = QColor("#ffc107");
};

#endif 
