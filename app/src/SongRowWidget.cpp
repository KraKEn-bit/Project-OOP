#include "SongRowWidget.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QMenu>
#include <QTimer>
#include <QRandomGenerator>
#include <cmath>


using namespace std;


SongRowWidget::SongRowWidget(QWidget *parent): QWidget(parent), m_songId(0), m_duration(0), m_rating(0), m_isPlaying(false)
    , m_isFavorite(false), m_opacity(1.0), m_hoverProgress(0.0), m_isHovered(false), m_hoveredStar(-1), m_waveformPhase(0)
{
    setFixedHeight(80);
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
    
    // Initializing waveform
    m_waveformHeights = {0.3, 0.6, 0.4};
    
    setupUI();
    createAnimations();
}

void SongRowWidget::setupUI() {

}

void SongRowWidget::createAnimations() {
    // Hover animation
    m_hoverAnimation = new QPropertyAnimation(this, "hoverProgress");
    m_hoverAnimation->setDuration(200);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Waveform animation
    m_waveformTimer = new QTimer(this);
    connect(m_waveformTimer, &QTimer::timeout, this, [this]() {
        m_waveformPhase = (m_waveformPhase + 1) % 100;
        for (size_t i = 0; i < m_waveformHeights.size(); ++i) {
            qreal phase = m_waveformPhase / 100.0 * 2.0 * M_PI + i * M_PI / 3.0;
            m_waveformHeights[i] = 0.3 + 0.4 * (0.5 + 0.5 * sin(phase));
        }
        update();
    });
}

void SongRowWidget::setSongInfo(const QString& title, const QString& artist,const QString& album, int duration, int rating) {
    m_title = title;
    m_artist = artist;
    m_album = album;
    m_duration = duration;
    m_rating = rating;
    update();
}

void SongRowWidget::setPlaying(bool playing) {
    m_isPlaying = playing;
    if (playing) {
        m_waveformTimer->start(80); 
    } 
    else {
        m_waveformTimer->stop();
    }
    update();
}

void SongRowWidget::setFavorite(bool favorite) {
    m_isFavorite = favorite;
    update();
}

void SongRowWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(m_opacity);
    
    // Background
    QColor bgColor = Qt::transparent;
    if (m_hoverProgress > 0.0) {
        bgColor = COLOR_BG_HOVER;
        bgColor.setAlphaF(m_hoverProgress * 0.8);
    }
    
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 12, 12);
    
    // Border for playing song
    if (m_isPlaying) {
        QPen borderPen(COLOR_PRIMARY, 2);
        painter.setPen(borderPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 12, 12);
    }
    
    // Layout positions
    int x = 15;
    int y = (height() - 60) / 2;
    
    // Play indicator / Index
    QRect playIndicatorRect(x, y, 40, 60);
    drawPlayIndicator(painter, playIndicatorRect);
    x += 55;
    
    // Album art
    QRect albumArtRect(x, y, 55, 55);
    drawAlbumArt(painter, albumArtRect);
    x += 70;
    
    // Song info
    painter.setPen(COLOR_TEXT_PRIMARY);
    QFont titleFont = painter.font();
    titleFont.setPixelSize(16);
    titleFont.setWeight(QFont::Medium);
    painter.setFont(titleFont);
    painter.drawText(x, y + 20, m_title);
    
    painter.setPen(COLOR_TEXT_SECONDARY);
    QFont artistFont = painter.font();
    artistFont.setPixelSize(13);
    painter.setFont(artistFont);
    QString artistAlbum = m_artist;

    if (!m_album.isEmpty()) {
        artistAlbum += " • " + m_album;
    }

    painter.drawText(x, y + 42, artistAlbum);
    
    // Rating stars (right side)
    int rightX = width() - 250;
    QRect starsRect(rightX, y + 15, 100, 30);
    drawRatingStars(painter, starsRect);
    rightX += 120;
    
    // Duration
    painter.setPen(COLOR_TEXT_SECONDARY);
    QFont durationFont = painter.font();
    durationFont.setPixelSize(14);
    durationFont.setFamily("Courier New");
    painter.setFont(durationFont);
    painter.drawText(rightX, y + 35, formatDuration(m_duration));
    rightX += 65;
    
    // Favorite button
    QRect favoriteRect(rightX, y + 10, 35, 35);
    QColor favBgColor = m_isFavorite ? QColor(COLOR_PRIMARY.red(), COLOR_PRIMARY.green(), COLOR_PRIMARY.blue(), 32) : Qt::transparent;

    painter.setBrush(favBgColor);

    if (m_isFavorite) {
        QPen favBorderPen(COLOR_PRIMARY, 2);
        painter.setPen(favBorderPen);
    } 
    else {
        painter.setPen(Qt::NoPen);
    }
    painter.drawEllipse(favoriteRect);
    
    // Drawing heart icon
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_isFavorite ? COLOR_PRIMARY : QColor("#a0a0a0"));
    QPointF heartCenter = favoriteRect.center();
    qreal heartSize = 10;
    QPainterPath heartPath;
    
    heartPath.moveTo(heartCenter.x(), heartCenter.y() + heartSize * 0.3);
    
    heartPath.cubicTo(heartCenter.x() - heartSize * 0.5, heartCenter.y() - heartSize * 0.3,heartCenter.x() - heartSize, heartCenter.y() + heartSize * 0.1,
                     heartCenter.x(), heartCenter.y() + heartSize * 0.7);


    heartPath.cubicTo(heartCenter.x() + heartSize, heartCenter.y() + heartSize * 0.1,heartCenter.x() + heartSize * 0.5, heartCenter.y() - heartSize * 0.3,
                     heartCenter.x(), heartCenter.y() + heartSize * 0.3);

    painter.drawPath(heartPath);
}

void SongRowWidget::drawPlayIndicator(QPainter& painter, const QRect& rect) {
    if (m_isPlaying) {
        // Animated waveform bars
        painter.setPen(Qt::NoPen);
        painter.setBrush(COLOR_PRIMARY);
        
        int barWidth = 4;
        int spacing = 3;
        int x = rect.center().x() - (3 * barWidth + 2 * spacing) / 2;
        int baseY = rect.center().y();
        
        for (size_t i = 0; i < 3; ++i) {
            int barHeight = static_cast<int>(30 * m_waveformHeights[i]);
            int y = baseY - barHeight / 2;
            painter.drawRoundedRect(x, y, barWidth, barHeight, 2, 2);
            x += barWidth + spacing;
        }
    } 
    else if (m_isHovered) {
        // Play button overlay
        painter.setBrush(COLOR_PRIMARY);
        painter.setPen(Qt::NoPen);
        QPointF center = rect.center();
        painter.drawEllipse(center, 17, 17);
        
        // Play triangle
        painter.setBrush(Qt::white);
        QPolygonF triangle;
        
        triangle << QPointF(center.x() - 5, center.y() - 8)<< QPointF(center.x() - 5, center.y() + 8)<< QPointF(center.x() + 8, center.y());

        painter.drawPolygon(triangle);
    }
}

void SongRowWidget::drawAlbumArt(QPainter& painter, const QRect& rect) {
    // Gradient background
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0.0, QColor("#0f3460"));
    gradient.setColorAt(1.0, QColor("#16213e"));
    
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, 8, 8);
    
    // Music note icon
    painter.setPen(Qt::NoPen);
    QColor iconColor("#ffffff");
    iconColor.setAlphaF(0.7);
    painter.setBrush(iconColor);
    
    QPointF center = rect.center();

    // Simple music note shape
    QRectF noteRect(center.x() - 6, center.y() - 10, 4, 15);
    painter.drawRoundedRect(noteRect, 2, 2);
    painter.drawEllipse(center.x() - 8, center.y() + 3, 8, 6);
}

void SongRowWidget::drawRatingStars(QPainter& painter, const QRect& rect) {
    int starSize = 18;
    int spacing = 4;
    int x = rect.x();
    int y = rect.center().y() - starSize / 2;
    
    for (int i = 0; i < 5; ++i) {
        bool filled = i < m_rating;
        bool hovered = i == m_hoveredStar;
        
        QColor starColor = filled ? COLOR_STAR : QColor("#5a5a5a");
        
        if (hovered) {
            starColor = starColor.lighter(130);
        }
        
        painter.setBrush(starColor);
        painter.setPen(Qt::NoPen);
        
        // Drawing star shape
        QPolygonF star;
        qreal angle = -M_PI / 2.0;
        qreal radius = starSize / 2.0;
        qreal cx = x + starSize / 2.0;
        qreal cy = y + starSize / 2.0;
        
        for (int j = 0; j < 10; ++j) {
            qreal r = (j % 2 == 0) ? radius : radius * 0.4;
            star << QPointF(cx + r * cos(angle), cy + r * sin(angle));
            angle += M_PI / 5.0;
        }
        
        painter.drawPolygon(star);
        x += starSize + spacing;
    }
}

QString SongRowWidget::formatDuration(int seconds) const {
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(minutes).arg(secs, 2, 10, QChar('0'));
}

void SongRowWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QWidget::mousePressEvent(event);
}

void SongRowWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();
        emit playRequested();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void SongRowWidget::enterEvent(QEnterEvent *event) {
    m_isHovered = true;
    m_hoverAnimation->stop();
    m_hoverAnimation->setStartValue(m_hoverProgress);
    m_hoverAnimation->setEndValue(1.0);
    m_hoverAnimation->start();
    QWidget::enterEvent(event);
}

void SongRowWidget::leaveEvent(QEvent *event) {
    m_isHovered = false;
    m_hoveredStar = -1;
    m_hoverAnimation->stop();
    m_hoverAnimation->setStartValue(m_hoverProgress);
    m_hoverAnimation->setEndValue(0.0);
    m_hoverAnimation->start();
    QWidget::leaveEvent(event);
}

void SongRowWidget::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu(this);
    menu.setStyleSheet(
        "QMenu {"
        "   background-color: #252d3a;"
        "   border: 1px solid #3d4758;"
        "   border-radius: 8px;"
        "   padding: 5px;"
        "}"
        "QMenu::item {"
        "   color: #ffffff;"
        "   padding: 8px 20px;"
        "   border-radius: 4px;"
        "}"
        "QMenu::item:selected {"
        "   background-color: #e94560;"
        "}"
    );
    
    QAction* playAction = menu.addAction("▶ Play Now");
    QAction* addToPlaylistAction = menu.addAction("➕ Add to Playlist");
    QAction* addToQueueAction = menu.addAction("📋 Add to Queue");
    menu.addSeparator();
    
    
    QAction* viewArtistAction = menu.addAction("👤 View Artist");
    QAction* viewAlbumAction = menu.addAction("💿 View Album");
    menu.addSeparator();
    
    QAction* infoAction = menu.addAction("ℹ Song Info");
    
    connect(playAction, &QAction::triggered, this, &SongRowWidget::playRequested);
    connect(addToPlaylistAction, &QAction::triggered, this, &SongRowWidget::addToPlaylistRequested);
    
    menu.exec(event->globalPos());
}
