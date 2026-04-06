#include "PlayerBarWidget.hpp"
#include "AppController.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QGraphicsDropShadowEffect>
#include <cmath>

using namespace std;


PlayerBarWidget::PlayerBarWidget(QWidget *parent): QWidget(parent), m_controller(nullptr), m_isPlaying(false), m_progress(0.0)
    , m_duration(0), m_volume(50), m_isShuffle(false), m_isRepeat(false), m_discRotation(0.0), m_playButtonScale(1.0)
{       
    setFixedHeight(100);
    setupUI();
    createAnimations();
}

void PlayerBarWidget::setupUI() {
    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 12, 20, 12);
    m_mainLayout->setSpacing(20);
    

    // Now playing section:
    m_nowPlayingWidget = new QWidget();
    QHBoxLayout* nowPlayingLayout = new QHBoxLayout(m_nowPlayingWidget);
    nowPlayingLayout->setContentsMargins(0, 0, 0, 0);
    nowPlayingLayout->setSpacing(15);
    
    m_discLabel = new QLabel("🎵");
    m_discLabel->setFixedSize(75, 75);
    m_discLabel->setAlignment(Qt::AlignCenter);
    m_discLabel->setStyleSheet(
        "QLabel {"
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "       stop:0 #0f3460, stop:1 #16213e);"
        "   border-radius: 8px;"
        "   font-size: 32px;"
        "}"
    );    

    nowPlayingLayout->addWidget(m_discLabel);
    
    QWidget* songInfoWidget = new QWidget();
    QVBoxLayout* songInfoLayout = new QVBoxLayout(songInfoWidget);
    songInfoLayout->setContentsMargins(0, 0, 0, 0);
    songInfoLayout->setSpacing(5);
    

    m_titleLabel = new QLabel("Select a song to play");
    m_titleLabel->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold;");
    songInfoLayout->addWidget(m_titleLabel);
    

    m_artistLabel = new QLabel("—");
    m_artistLabel->setStyleSheet("color: #a0a0a0; font-size: 13px;");
    songInfoLayout->addWidget(m_artistLabel);
    

    nowPlayingLayout->addWidget(songInfoWidget);
    

    m_favoriteBtn = new CircularButton("🤍", 32);
    nowPlayingLayout->addWidget(m_favoriteBtn);
    

    m_nowPlayingWidget->setFixedWidth(300);
    m_mainLayout->addWidget(m_nowPlayingWidget);
    

    // Controls section
    m_controlsWidget = new QWidget();
    QVBoxLayout* controlsLayout = new QVBoxLayout(m_controlsWidget);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    controlsLayout->setSpacing(10);
    

    // Control buttons
    m_controlButtonsLayout = new QHBoxLayout();
    m_controlButtonsLayout->setSpacing(15);
    m_controlButtonsLayout->setAlignment(Qt::AlignCenter);
    

    m_shuffleBtn = new CircularButton("🔀", 32);
    connect(m_shuffleBtn, &QPushButton::clicked, this, &PlayerBarWidget::shuffleClicked);
    m_controlButtonsLayout->addWidget(m_shuffleBtn);
    
    
    m_previousBtn = new CircularButton("⏮", 40);
    connect(m_previousBtn, &QPushButton::clicked, this, &PlayerBarWidget::previousClicked);
    m_controlButtonsLayout->addWidget(m_previousBtn);
    

    m_playPauseBtn = new CircularButton("▶", 56);
    static_cast<CircularButton*>(m_playPauseBtn)->setPrimary(true);
    connect(m_playPauseBtn, &QPushButton::clicked, this, &PlayerBarWidget::onPlayPauseClicked);
    m_controlButtonsLayout->addWidget(m_playPauseBtn);
    
    m_nextBtn = new CircularButton("⏭", 40);
    connect(m_nextBtn, &QPushButton::clicked, this, &PlayerBarWidget::nextClicked);
    m_controlButtonsLayout->addWidget(m_nextBtn);
    
    m_repeatBtn = new CircularButton("🔁", 32);
    connect(m_repeatBtn, &QPushButton::clicked, this, &PlayerBarWidget::repeatClicked);
    m_controlButtonsLayout->addWidget(m_repeatBtn);
    
    controlsLayout->addLayout(m_controlButtonsLayout);
    
    // Progress bar
    m_progressWidget = new QWidget();
    QHBoxLayout* progressLayout = new QHBoxLayout(m_progressWidget);
    progressLayout->setContentsMargins(0, 0, 0, 0);
    progressLayout->setSpacing(10);
    
    m_currentTimeLabel = new QLabel("0:00");
    m_currentTimeLabel->setStyleSheet("color: #a0a0a0; font-size: 11px; font-family: 'Courier New';");
    m_currentTimeLabel->setFixedWidth(45);
    m_currentTimeLabel->setAlignment(Qt::AlignRight);
    progressLayout->addWidget(m_currentTimeLabel);
    
    m_progressSlider = new ModernSlider(Qt::Horizontal);
    m_progressSlider->setRange(0, 1000);
    m_progressSlider->setValue(0);
    static_cast<ModernSlider*>(m_progressSlider)->setAccentColor(COLOR_PRIMARY);
    connect(m_progressSlider, &QSlider::sliderMoved, this, &PlayerBarWidget::onProgressSliderMoved);
    progressLayout->addWidget(m_progressSlider, 1);
    
    m_totalTimeLabel = new QLabel("3:45");
    m_totalTimeLabel->setStyleSheet("color: #a0a0a0; font-size: 11px; font-family: 'Courier New';");
    m_totalTimeLabel->setFixedWidth(45);
    progressLayout->addWidget(m_totalTimeLabel);
    
    controlsLayout->addWidget(m_progressWidget);
    
    m_mainLayout->addWidget(m_controlsWidget, 1);
    
    // Volume section
    m_volumeWidget = new QWidget();
    QHBoxLayout* volumeLayout = new QHBoxLayout(m_volumeWidget);
    volumeLayout->setContentsMargins(0, 0, 0, 0);
    volumeLayout->setSpacing(10);
    
    m_volumeBtn = new CircularButton("🔊", 32);
    volumeLayout->addWidget(m_volumeBtn);
    
    m_volumeSlider = new ModernSlider(Qt::Horizontal);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(50);
    static_cast<ModernSlider*>(m_volumeSlider)->setAccentColor(QColor("#00d9a5"));
    connect(m_volumeSlider, &QSlider::sliderMoved, this, &PlayerBarWidget::onVolumeSliderMoved);
    volumeLayout->addWidget(m_volumeSlider);
    
    m_volumeLabel = new QLabel("50%");
    m_volumeLabel->setStyleSheet("color: #a0a0a0; font-size: 11px;");
    m_volumeLabel->setFixedWidth(35);
    volumeLayout->addWidget(m_volumeLabel);
    
    m_volumeWidget->setFixedWidth(220);
    m_mainLayout->addWidget(m_volumeWidget);
}

void PlayerBarWidget::createAnimations() {
    // Disc rotation animation
    m_discRotationAnim = new QPropertyAnimation(this, "discRotation");
    m_discRotationAnim->setDuration(20000);
    m_discRotationAnim->setStartValue(0.0);
    m_discRotationAnim->setEndValue(360.0);
    m_discRotationAnim->setLoopCount(-1);
    m_discRotationAnim->setEasingCurve(QEasingCurve::Linear);
}

void PlayerBarWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    

    QColor bgColor = COLOR_BG;
    bgColor.setAlphaF(0.95);
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());
    

    QLinearGradient topGlow(0, 0, width(), 0);
    topGlow.setColorAt(0.0, QColor(COLOR_PRIMARY.red(), COLOR_PRIMARY.green(), 
                                   COLOR_PRIMARY.blue(), 64));
    topGlow.setColorAt(0.5, COLOR_PRIMARY);
    topGlow.setColorAt(1.0, QColor(COLOR_PRIMARY.red(), COLOR_PRIMARY.green(), 
                                   COLOR_PRIMARY.blue(), 64));
    painter.setBrush(topGlow);
    painter.drawRect(0, 0, width(), 2);
}

void PlayerBarWidget::setController(AppController* controller) {
    m_controller = controller;
}

void PlayerBarWidget::setCurrentSong(const QString& title, const QString& artist, const QString& album) {
    m_currentTitle = title;
    m_currentArtist = artist;
    m_currentAlbum = album;
    
    m_titleLabel->setText(title);
    m_artistLabel->setText(artist + (album.isEmpty() ? "" : " • " + album));
}

void PlayerBarWidget::setPlaying(bool playing) {
    m_isPlaying = playing;
    
    CircularButton* playBtn = static_cast<CircularButton*>(m_playPauseBtn);
    playBtn->setIcon(playing ? "⏸" : "▶");
    
    if (playing) {
        m_discRotationAnim->start();
    } else {
        m_discRotationAnim->pause();
    }
}

void PlayerBarWidget::setProgress(qreal progress) {
    m_progress = progress;
    m_progressSlider->setValue(static_cast<int>(progress * 1000));
    
    int currentSeconds = static_cast<int>(progress * m_duration);
    m_currentTimeLabel->setText(formatTime(currentSeconds));
}

void PlayerBarWidget::setDuration(int seconds) {
    m_duration = seconds;
    m_totalTimeLabel->setText(formatTime(seconds));
}

void PlayerBarWidget::setVolume(int volume) {
    m_volume = volume;
    m_volumeSlider->setValue(volume);
    m_volumeLabel->setText(QString::number(volume) + "%");
    
    // Update volume icon
    CircularButton* volBtn = static_cast<CircularButton*>(m_volumeBtn);
    if (volume == 0) {
        volBtn->setIcon("🔇");
    } else if (volume < 50) {
        volBtn->setIcon("🔉");
    } else {
        volBtn->setIcon("🔊");
    }
}

QString PlayerBarWidget::formatTime(int seconds) const {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    if (hours > 0) {
        return QString("%1:%2:%3")
            .arg(hours)
            .arg(minutes, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'));
    } else {
        return QString("%1:%2")
            .arg(minutes)
            .arg(secs, 2, 10, QChar('0'));
    }
}

void PlayerBarWidget::onPlayPauseClicked() {
    setPlaying(!m_isPlaying);
    emit playPauseClicked();
}

void PlayerBarWidget::onProgressSliderMoved(int value) {
    qreal position = value / 1000.0;
    emit seekRequested(position);
}

void PlayerBarWidget::onVolumeSliderMoved(int value) {
    setVolume(value);
    emit volumeChanged(value);
}



// ModernSlider Implementation
ModernSlider::ModernSlider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent)
    , m_accentColor(QColor("#e94560"))
{
    setStyleSheet("background: transparent;");
}

void ModernSlider::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    int sliderHeight = 5;
    int handleSize = 14;
    
    // Track background
    QRect trackRect(0, (height() - sliderHeight) / 2, width(), sliderHeight);
    painter.setBrush(QColor("#2d3748"));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(trackRect, sliderHeight / 2, sliderHeight / 2);
    
    // Filled portion
    qreal progress = static_cast<qreal>(value() - minimum()) / (maximum() - minimum());
    QRect filledRect(0, (height() - sliderHeight) / 2, 
                     static_cast<int>(width() * progress), sliderHeight);
    
    QLinearGradient fillGradient(filledRect.topLeft(), filledRect.topRight());
    fillGradient.setColorAt(0.0, m_accentColor);
    fillGradient.setColorAt(1.0, m_accentColor.darker(110));
    painter.setBrush(fillGradient);
    painter.drawRoundedRect(filledRect, sliderHeight / 2, sliderHeight / 2);
    
    // Handle
    if (isSliderDown() || underMouse()) {
        int handleX = static_cast<int>(progress * (width() - handleSize));
        QRect handleRect(handleX, (height() - handleSize) / 2, handleSize, handleSize);
        
        painter.setBrush(Qt::white);
        painter.setPen(QPen(m_accentColor, 2));
        painter.drawEllipse(handleRect);
    }
}




// CircularButton Implementation
CircularButton::CircularButton(const QString& icon, int size, QWidget *parent)
    : QPushButton(parent)
    , m_iconText(icon)
    , m_size(size)
    , m_isActive(false)
    , m_isPrimary(false)
    , m_isHovered(false)
    , m_isPressed(false)
    , m_scale(1.0)
{
    setFixedSize(size, size);
    setCursor(Qt::PointingHandCursor);
    setStyleSheet("background: transparent; border: none;");
    
    m_scaleAnimation = new QPropertyAnimation(this, "scale");
    m_scaleAnimation->setDuration(150);
    m_scaleAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void CircularButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Scale transformation
    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(m_scale, m_scale);
    painter.translate(-width() / 2.0, -height() / 2.0);
    
    // Background
    QColor bgColor;
    if (m_isPrimary) {
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0.0, QColor("#e94560"));
        gradient.setColorAt(1.0, QColor("#d32f4f"));
        painter.setBrush(gradient);
    } else if (m_isActive) {
        bgColor = QColor("#e94560");
        painter.setBrush(bgColor);
    } else {
        bgColor = QColor("#2d3748");
        if (m_isHovered) {
            bgColor = bgColor.lighter(120);
        }
        painter.setBrush(bgColor);
    }
    
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect());
    
    // Active indicator dot
    if (m_isActive && !m_isPrimary) {
        painter.setBrush(Qt::white);
        int dotSize = 4;
        QRect dotRect(width() / 2 - dotSize / 2, height() - 8, dotSize, dotSize);
        painter.drawEllipse(dotRect);
    }
    
    // Icon
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setPixelSize(m_size / 2);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, m_iconText);
}

void CircularButton::enterEvent(QEnterEvent *event) {
    m_isHovered = true;
    m_scaleAnimation->setStartValue(m_scale);
    m_scaleAnimation->setEndValue(1.05);
    m_scaleAnimation->start();
    update();
    QPushButton::enterEvent(event);
}

void CircularButton::leaveEvent(QEvent *event) {
    m_isHovered = false;
    m_scaleAnimation->setStartValue(m_scale);
    m_scaleAnimation->setEndValue(1.0);
    m_scaleAnimation->start();
    update();
    QPushButton::leaveEvent(event);
}

void CircularButton::mousePressEvent(QMouseEvent *event) {
    m_isPressed = true;
    setScale(0.95);
    QPushButton::mousePressEvent(event);
}

void CircularButton::mouseReleaseEvent(QMouseEvent *event) {
    m_isPressed = false;
    setScale(m_isHovered ? 1.05 : 1.0);
    QPushButton::mouseReleaseEvent(event);
}
