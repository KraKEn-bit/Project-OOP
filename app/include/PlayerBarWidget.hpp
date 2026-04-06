#ifndef PLAYERBARWIDGET_HPP
#define PLAYERBARWIDGET_HPP

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QPropertyAnimation>
#include <QTimer>

using namespace std;

// Forward declaration
class AppController;


class PlayerBarWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal discRotation READ discRotation WRITE setDiscRotation)
    
public:
    explicit PlayerBarWidget(QWidget *parent = nullptr);
    
    void setController(AppController* controller);
    void setCurrentSong(const QString& title, const QString& artist, const QString& album);
    void setPlaying(bool playing);
    void setProgress(qreal progress);
    void setDuration(int seconds);
    void setVolume(int volume);
    
    qreal discRotation() const { return m_discRotation; }
    void setDiscRotation(qreal rotation) { m_discRotation = rotation; update(); }
    
signals:
    void playPauseClicked();
    void nextClicked();
    void previousClicked();
    void shuffleClicked();
    void repeatClicked();
    void seekRequested(qreal position);
    void volumeChanged(int volume);
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private slots:
    void onPlayPauseClicked();
    void onProgressSliderMoved(int value);
    void onVolumeSliderMoved(int value);
    
private:
    void setupUI();
    void createAnimations();
    void drawAlbumDisc(QPainter& painter, const QRect& rect);
    void drawPlayButton(QPainter& painter, const QRect& rect);
    QString formatTime(int seconds) const;
    
    // UI Components
    QHBoxLayout* m_mainLayout;
    
    // Now playing section
    QWidget* m_nowPlayingWidget;
    QLabel* m_discLabel;
    QLabel* m_titleLabel;
    QLabel* m_artistLabel;
    QPushButton* m_favoriteBtn;
    
    // Controls section
    QWidget* m_controlsWidget;
    QHBoxLayout* m_controlButtonsLayout;
    QPushButton* m_shuffleBtn;
    QPushButton* m_previousBtn;
    QPushButton* m_playPauseBtn;
    QPushButton* m_nextBtn;
    QPushButton* m_repeatBtn;
    
    // Progress section
    QWidget* m_progressWidget;
    QLabel* m_currentTimeLabel;
    QSlider* m_progressSlider;
    QLabel* m_totalTimeLabel;
    
    // Volume section
    QWidget* m_volumeWidget;
    QPushButton* m_volumeBtn;
    QSlider* m_volumeSlider;
    QLabel* m_volumeLabel;
    
    // Data
    AppController* m_controller;
    QString m_currentTitle;
    QString m_currentArtist;
    QString m_currentAlbum;
    bool m_isPlaying;
    qreal m_progress;
    int m_duration;
    int m_volume;
    bool m_isShuffle;
    bool m_isRepeat;
    
    // Animations
    QPropertyAnimation* m_discRotationAnim;
    QPropertyAnimation* m_playButtonPulseAnim;
    qreal m_discRotation;
    qreal m_playButtonScale;
    
    // Colors
    const QColor COLOR_BG = QColor("#16213e");
    const QColor COLOR_PRIMARY = QColor("#e94560");
    const QColor COLOR_TEXT_PRIMARY = QColor("#ffffff");
    const QColor COLOR_TEXT_SECONDARY = QColor("#a0a0a0");
};


class ModernSlider : public QSlider {
    Q_OBJECT
    
public:
    explicit ModernSlider(Qt::Orientation orientation, QWidget *parent = nullptr);
    
    void setAccentColor(const QColor& color) { 
        m_accentColor = color;
        update(); 
    }
    
protected:
    void paintEvent(QPaintEvent *event) override;
    
private:
    QColor m_accentColor;
};


class CircularButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    
public:
    explicit CircularButton(const QString& icon, int size, QWidget *parent = nullptr);
    
    void setIcon(const QString& icon) { 
        m_iconText = icon; 
        update(); 
    }
    void setActive(bool active) { 
        m_isActive = active; 
        update(); 
    }
    void setPrimary(bool primary) { 
        m_isPrimary = primary; 
        update(); 
    }
    
    qreal scale() const { 
        return m_scale; 
    }
    void setScale(qreal scale) { 
        m_scale = scale; 
        update(); 
    }
    
protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    
private:
    QString m_iconText;
    int m_size;
    bool m_isActive;
    bool m_isPrimary;
    bool m_isHovered;
    bool m_isPressed;
    qreal m_scale;
    QPropertyAnimation* m_scaleAnimation;
};

#endif // PLAYERBARWIDGET_HPP
