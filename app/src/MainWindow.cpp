#include "MainWindow.hpp"
#include "AppController.hpp"
#include "PlayerBarWidget.hpp"
#include "PlaylistPanelWidget.hpp"
#include "RecommendationPanelWidget.hpp"
#include "SongRowWidget.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsDropShadowEffect>
#include <QRandomGenerator>
#include <cmath>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_controller(nullptr)
    , m_currentTab(0)
{
    setWindowTitle("PocketTrack - Your Music Companion");
    resize(1280, 800);
    setMinimumSize(1000, 600);

    // Particle system
    m_particleTimer = new QTimer(this);
    connect(m_particleTimer, &QTimer::timeout, this, &MainWindow::animateParticles);

    for (int i = 0; i < 20; ++i) {
        qreal x = QRandomGenerator::global()->bounded(1280);
        qreal y = QRandomGenerator::global()->bounded(800);
        m_particlePositions.push_back(QPointF(x, y));
        m_particleOffsets.push_back(QRandomGenerator::global()->bounded(100) / 100.0);
    }

    setupUI();
    applyStyles();
    createAnimations();

    m_particleTimer->start(50);
}

MainWindow::~MainWindow() {}

// UI setup

void MainWindow::setupUI() {
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // Outer vertical layout: [content row] + [player bar]
    QVBoxLayout* outerVertical = new QVBoxLayout(m_centralWidget);
    outerVertical->setContentsMargins(0, 0, 0, 0);
    outerVertical->setSpacing(0);

    // Inner horizontal layout: [left panel] + [content] + [right panel]
    m_mainLayout = new QHBoxLayout();
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);


    m_playlistPanel = new PlaylistPanelWidget();
    connect(m_playlistPanel, &PlaylistPanelWidget::playlistSelected,
            this, &MainWindow::onPlaylistSelected);
    connect(m_playlistPanel, &PlaylistPanelWidget::createPlaylistRequested,
            this, [this](const QString& name) {
                if (m_controller)
                    m_controller->createPlaylist(name.toStdString());
            });

    m_recommendationPanel = new RecommendationPanelWidget();
    connect(m_recommendationPanel, &RecommendationPanelWidget::moodSelected,
            this, &MainWindow::onMoodFilterSelected);
    connect(m_recommendationPanel, &RecommendationPanelWidget::playlistThemeRequested,
            this, [this](const QString& theme) {
                if (!m_controller) return;
                auto songs = m_controller->getThemePlaylist(theme.toStdString(), 20);

                m_currentFilter.clear();
                m_currentMood.clear();

                clearSongList();
                showSongVector(songs);
            });

    // Content widget (centre column)
    m_contentWidget = new QWidget();
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(20, 20, 20, 20);
    m_contentLayout->setSpacing(20);

    setupTopBar();
    setupContentArea();

    // LEFT | CENTRE | RIGHT
    m_mainLayout->addWidget(m_playlistPanel);
    m_mainLayout->addWidget(m_contentWidget, 1);
    m_mainLayout->addWidget(m_recommendationPanel);

    QWidget* contentRow = new QWidget();
    contentRow->setLayout(m_mainLayout);

    // Player bar
    m_playerBar = new PlayerBarWidget();
    m_playerBar->setFixedHeight(100);

    outerVertical->addWidget(contentRow, 1);
    outerVertical->addWidget(m_playerBar);
}

void MainWindow::setupCentralWidget() {}

void MainWindow::setupTopBar() {
    m_topBar = new CardWidget();
    m_topBar->setFixedHeight(70);

    QHBoxLayout* topLayout = new QHBoxLayout(m_topBar);
    topLayout->setContentsMargins(15, 15, 15, 15);
    topLayout->setSpacing(15);

    // Online indicator
    m_onlineIndicator = new QLabel();
    m_onlineIndicator->setFixedSize(45, 45);
    m_onlineIndicator->setAlignment(Qt::AlignCenter);
    m_onlineIndicator->setText("🌐");
    m_onlineIndicator->setStyleSheet(
        "QLabel { background-color: #00d9a5; border-radius: 22px; font-size: 24px; }"
    );

    QGraphicsOpacityEffect* pulseEffect = new QGraphicsOpacityEffect(m_onlineIndicator);
    m_onlineIndicator->setGraphicsEffect(pulseEffect);
    QPropertyAnimation* pulseAnim = new QPropertyAnimation(pulseEffect, "opacity");
    pulseAnim->setDuration(2000);
    pulseAnim->setStartValue(0.5);
    pulseAnim->setEndValue(1.0);
    pulseAnim->setEasingCurve(QEasingCurve::InOutSine);
    pulseAnim->setLoopCount(-1);
    pulseAnim->start(QAbstractAnimation::DeleteWhenStopped);
    topLayout->addWidget(m_onlineIndicator);

    // Search bar
    m_searchBar = new QLineEdit();
    m_searchBar->setPlaceholderText("Search songs, artists, or albums...");
    m_searchBar->setStyleSheet(
        "QLineEdit {"
        "   background-color: rgba(45, 55, 72, 0.5);"
        "   border: none; border-radius: 22px;"
        "   padding: 12px 20px; color: #ffffff; font-size: 14px;"
        "}"
        "QLineEdit::placeholder { color: #a0a0a0; }"
    );
    connect(m_searchBar, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    topLayout->addWidget(m_searchBar, 1);

    QPushButton* gridViewBtn = new QPushButton("📊");
    gridViewBtn->setFixedSize(45, 45);
    gridViewBtn->setStyleSheet(
        "QPushButton { background-color: #2d3748; border: none; border-radius: 22px;"
        "              color: #ffffff; font-size: 20px; }"
        "QPushButton:hover { background-color: #3d4758; }"
    );
    topLayout->addWidget(gridViewBtn);

    m_settingsBtn = new QPushButton("⚙️");
    m_settingsBtn->setFixedSize(45, 45);
    m_settingsBtn->setStyleSheet(
        "QPushButton { background-color: #2d3748; border: none; border-radius: 22px;"
        "              color: #ffffff; font-size: 20px; }"
        "QPushButton:hover { background-color: #3d4758; }"
    );
    topLayout->addWidget(m_settingsBtn);

    m_contentLayout->addWidget(m_topBar);
}

void MainWindow::setupContentArea() {
    m_contentArea = new CardWidget();

    QVBoxLayout* contentAreaLayout = new QVBoxLayout(m_contentArea);
    contentAreaLayout->setContentsMargins(20, 20, 20, 20);
    contentAreaLayout->setSpacing(15);

    // Header + tabs row
    QHBoxLayout* headerLayout = new QHBoxLayout();

    QLabel* libraryLabel = new QLabel("Your Library");
    libraryLabel->setStyleSheet(
        "QLabel { color: #ffffff; font-size: 28px; font-weight: bold; }"
    );
    headerLayout->addWidget(libraryLabel);
    headerLayout->addStretch();

    m_tabWidget = new QWidget();
    m_tabLayout = new QHBoxLayout(m_tabWidget);
    m_tabLayout->setSpacing(10);
    m_tabLayout->setContentsMargins(0, 0, 0, 0);

    QStringList tabNames = { "All Songs", "Artists", "Albums", "Genres" };
    for (int i = 0; i < tabNames.size(); ++i) {
        TabButton* tab = new TabButton(tabNames[i]);

        // FIX: first tab starts active
        if (i == 0) tab->setActive(true);

        connect(tab, &QPushButton::clicked, this, [this, i]() {
            onTabChanged(i);
        });

        m_tabs.push_back(tab);
        m_tabLayout->addWidget(tab);
    }

    headerLayout->addWidget(m_tabWidget);
    contentAreaLayout->addLayout(headerLayout);

    // Scroll area
    m_songScrollArea = new QScrollArea();
    m_songScrollArea->setWidgetResizable(true);
    m_songScrollArea->setFrameShape(QFrame::NoFrame);
    m_songScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_songScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_songListContainer = new QWidget();
    m_songListLayout = new QVBoxLayout(m_songListContainer);
    m_songListLayout->setSpacing(8);
    m_songListLayout->setContentsMargins(0, 0, 0, 0);
    m_songListLayout->addStretch();

    m_songScrollArea->setWidget(m_songListContainer);
    m_songScrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical { background: rgba(45,55,72,0.3); width: 8px; border-radius: 4px; }"
        "QScrollBar::handle:vertical { background: #e94560; border-radius: 4px; min-height: 20px; }"
        "QScrollBar::handle:vertical:hover { background: #d32f4f; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
    );

    contentAreaLayout->addWidget(m_songScrollArea);
    m_contentLayout->addWidget(m_contentArea, 1);
}

void MainWindow::setupSidebars() {}   // kept for API compatibility
void MainWindow::setupPlayerBar()  {}  // kept for API compatibility

// ── Slot: song play requested ─────────────────────────────────────────────────

void MainWindow::onSongPlayRequested(shared_ptr<Song> song) {
    if (!song || !m_playerBar) return;

    QString artistName = song->getArtist()
        ? QString::fromStdString(song->getArtist()->getName()) : "Unknown";
    QString albumTitle = song->getAlbum()
        ? QString::fromStdString(song->getAlbum()->getTitle()) : "";

    m_playerBar->setCurrentSong(
        QString::fromStdString(song->getTitle()), artistName, albumTitle);
    m_playerBar->setDuration(song->getDuration());
    m_playerBar->setPlaying(true);


    for (int i = 0; i < m_songListLayout->count(); ++i) {
        auto* row = qobject_cast<SongRowWidget*>(m_songListLayout->itemAt(i)->widget());
        if (row)
            row->setPlaying(row->getSongId() == song->getId());
    }


    if (m_controller) {
        m_controller->onSongPlayed(song);


        auto recs = m_controller->getSimilarSongs(song, 5);
        QVector<QPair<QString, QString>> recPairs;
        for (const auto& s : recs) {
            recPairs.append({
                QString::fromStdString(s->getTitle()),
                s->getArtist()
                    ? QString::fromStdString(s->getArtist()->getName()) : "Unknown"
            });
        }
        m_recommendationPanel->setRecommendedSongs(recPairs);
    }
}

// Song list helpers

void MainWindow::clearSongList() {
    QLayoutItem* child;
    while ((child = m_songListLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

void MainWindow::showSongVector(const vector<shared_ptr<Song>>& songs) {
    for (int i = 0; i < (int)songs.size(); ++i) {
        const auto& song = songs[i];

        SongRowWidget* songRow = new SongRowWidget();
        songRow->setSongInfo(
            QString::fromStdString(song->getTitle()),
            song->getArtist()
                ? QString::fromStdString(song->getArtist()->getName()) : "Unknown",
            song->getAlbum()
                ? QString::fromStdString(song->getAlbum()->getTitle()) : "",
            song->getDuration(),
            song->getRating()
        );
        songRow->setSongId(song->getId());
        songRow->setFavorite(song->isFavorite());

        connect(songRow, &SongRowWidget::playRequested, this, [this, song]() {
            onSongPlayRequested(song);
        });
        connect(songRow, &SongRowWidget::doubleClicked, this, [this, song]() {
            onSongPlayRequested(song);
        });

        m_songListLayout->insertWidget(m_songListLayout->count() - 1, songRow);

        // Staggered fade-in
        songRow->setOpacity(0.0);
        QPropertyAnimation* fadeIn = new QPropertyAnimation(songRow, "opacity");
        fadeIn->setDuration(300);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->setEasingCurve(QEasingCurve::OutCubic);
        QTimer::singleShot(i * 30, [fadeIn]() { fadeIn->start(QAbstractAnimation::DeleteWhenStopped); });
    }
    m_songListLayout->addStretch();
}

void MainWindow::updateSongList() {
    if (!m_controller) return;

    clearSongList();

    vector<shared_ptr<Song>> songs;
    if (!m_currentFilter.isEmpty()) {
        songs = m_controller->searchSongs(m_currentFilter);
    } else if (!m_currentMood.isEmpty()) {
        songs = m_controller->filterByMood(m_currentMood);
    } else {
        songs = m_controller->getAllSongs();
    }

    showSongVector(songs);   // FIX: removed arbitrary 50-song cap
}

// Slots

void MainWindow::onSearchTextChanged(const QString& text) {
    m_currentFilter = text;
    m_currentMood.clear();
    updateSongList();
}

void MainWindow::onTabChanged(int index) {

    // for (int i = 0; i < (int)m_tabs.size(); ++i)
    //     m_tabs[i]->setActive(i == index);

    m_currentTab = index;
    updateSongList();
}

void MainWindow::onSongsLoaded()                         { updateSongList(); }
void MainWindow::onPlaylistSelected(const QString& name) { updateSongList(); }

void MainWindow::onMoodFilterSelected(const QString& mood) {
    m_currentMood   = mood;
    m_currentFilter.clear();
    updateSongList();
}

void MainWindow::showWelcomeScreen() {}
void MainWindow::showLibrary()       { updateSongList(); }

// Controller

void MainWindow::setController(AppController* controller) {
    m_controller = controller;
    if (m_playerBar) m_playerBar->setController(controller);
    connect(m_controller, &AppController::songsLoaded,
            this, &MainWindow::onSongsLoaded);
}

// Painting / Animation
void MainWindow::applyStyles() {
    setStyleSheet(
        "QMainWindow {"
        "   background: qlineargradient(x1:0,y1:0,x2:0,y2:1,"
        "       stop:0 #1a1a2e, stop:0.5 #16213e, stop:1 #0f3460);"
        "}"
    );
}

void MainWindow::createAnimations() {

    QPropertyAnimation* leftSlide  = new QPropertyAnimation(m_playlistPanel, "pos");
    leftSlide->setDuration(600);
    leftSlide->setEasingCurve(QEasingCurve::OutCubic);

    QPropertyAnimation* rightSlide = new QPropertyAnimation(m_recommendationPanel, "pos");
    rightSlide->setDuration(600);
    rightSlide->setEasingCurve(QEasingCurve::OutCubic);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QMainWindow::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0.0, COLOR_BG_DARK);
    gradient.setColorAt(0.5, COLOR_BG_MEDIUM);
    gradient.setColorAt(1.0, COLOR_BG_LIGHT);
    painter.fillRect(rect(), gradient);

    painter.setPen(Qt::NoPen);
    for (size_t i = 0; i < m_particlePositions.size(); ++i) {
        QColor c = COLOR_PRIMARY;
        c.setAlphaF(0.3 + 0.3 * sin(m_particleOffsets[i]));
        painter.setBrush(c);
        painter.drawEllipse(m_particlePositions[i], 2, 2);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
}

void MainWindow::animateParticles() {
    for (size_t i = 0; i < m_particlePositions.size(); ++i) {
        m_particleOffsets[i]      += 0.05;
        m_particlePositions[i].ry() -= 1.0;

        if (m_particlePositions[i].y() < -20) {
            m_particlePositions[i].setY(height() + 20);
            m_particlePositions[i].setX(QRandomGenerator::global()->bounded(width()));
        }
    }
    update();
}


// CardWidget

CardWidget::CardWidget(QWidget *parent)
    : QWidget(parent)
    , m_cornerRadius(15)
    , m_cardColor(QColor("#252d3a"))
    , m_hoverEnabled(false)
    , m_isHovered(false)
    , m_opacity(1.0)
{
    setAttribute(Qt::WA_TranslucentBackground);
    m_hoverAnimation = new QPropertyAnimation(this, "opacity");
    m_hoverAnimation->setDuration(200);
    m_hoverAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

void CardWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(m_opacity);

    QColor bgColor = m_cardColor;
    if (m_isHovered && m_hoverEnabled) bgColor = bgColor.lighter(110);

    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), m_cornerRadius, m_cornerRadius);
}

void CardWidget::enterEvent(QEnterEvent *event) {
    if (m_hoverEnabled) { m_isHovered = true; update(); }
    QWidget::enterEvent(event);
}

void CardWidget::leaveEvent(QEvent *event) {
    if (m_hoverEnabled) { m_isHovered = false; update(); }
    QWidget::leaveEvent(event);
}


// TabButton

TabButton::TabButton(const QString& text, QWidget *parent)
    : QPushButton(text, parent)
    , m_isActive(false)
    , m_isHovered(false)
    , m_scale(1.0)
{
    setFixedSize(100, 40);
    setCursor(Qt::PointingHandCursor);
    m_animation = new QPropertyAnimation(this, "scale");
    m_animation->setDuration(150);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
}

void TabButton::setActive(bool active) {
    m_isActive = active;
    update();
}

void TabButton::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor bgColor = m_isActive ? QColor("#e94560") : QColor("#0f3460");
    if (m_isHovered && !m_isActive) bgColor = bgColor.lighter(120);

    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 20, 20);

    painter.setPen(QColor("#ffffff"));
    QFont font = painter.font();
    font.setPixelSize(13);
    font.setBold(m_isActive);
    painter.setFont(font);
    painter.drawText(rect(), Qt::AlignCenter, text());
}

void TabButton::enterEvent(QEnterEvent *event) {
    m_isHovered = true; update(); QPushButton::enterEvent(event);
}
void TabButton::leaveEvent(QEvent *event) {
    m_isHovered = false; update(); QPushButton::leaveEvent(event);
}