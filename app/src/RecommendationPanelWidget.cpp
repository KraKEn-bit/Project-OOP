#include "RecommendationPanelWidget.hpp"
#include <QPainter>
#include <QPainterPath>

RecommendationPanelWidget::RecommendationPanelWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedWidth(280);
    setupUI();
}

void RecommendationPanelWidget::setupUI() {
    m_outerLayout = new QVBoxLayout(this);
    m_outerLayout->setContentsMargins(12, 20, 12, 12);
    m_outerLayout->setSpacing(14);


    QLabel* title = new QLabel("✨  Discover");
    title->setStyleSheet(
        "color: #ffffff; font-size: 16px; font-weight: bold; background: transparent;"
    );
    m_outerLayout->addWidget(title);

    // Mood sectio
    QLabel* moodLabel = new QLabel("FILTER BY MOOD");
    moodLabel->setStyleSheet(
        "color: #a0a0a0; font-size: 10px; letter-spacing: 1px; background: transparent;"
    );
    m_outerLayout->addWidget(moodLabel);

    buildMoodButtons();

    QLabel* divider = new QLabel("AUTO PLAYLISTS");
    divider->setStyleSheet(
        "color: #a0a0a0; font-size: 10px; letter-spacing: 1px; background: transparent;"
    );
    m_outerLayout->addWidget(divider);

    buildThemeButtons();

    QLabel* recLabel = new QLabel("RECOMMENDED");
    recLabel->setStyleSheet(
        "color: #a0a0a0; font-size: 10px; letter-spacing: 1px; background: transparent;"
    );
    m_outerLayout->addWidget(recLabel);

    m_songListContainer = new QWidget();
    m_songListContainer->setStyleSheet("background: transparent;");
    m_songListLayout = new QVBoxLayout(m_songListContainer);
    m_songListLayout->setContentsMargins(0, 0, 0, 0);
    m_songListLayout->setSpacing(6);


    QStringList placeholders = {
        "🎵  Play a song to get suggestions",
        "💡  Rate songs to improve picks"
    };
    for (const QString& text : placeholders) {
        QLabel* lbl = new QLabel(text);
        lbl->setStyleSheet(
            "color: #a0a0a0; font-size: 12px;"
            "background-color: #252d3a; border-radius: 8px; padding: 8px;"
        );
        lbl->setWordWrap(true);
        m_songListLayout->addWidget(lbl);
    }
    m_songListLayout->addStretch();
    m_outerLayout->addWidget(m_songListContainer, 1);
}

void RecommendationPanelWidget::buildMoodButtons() {
    m_moodGrid = new QWidget();
    m_moodGrid->setStyleSheet("background: transparent;");
    QGridLayout* grid = new QGridLayout(m_moodGrid);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(8);

    for (int i = 0; i < MOODS.size(); ++i) {
        const auto& entry = MOODS[i];
        QPushButton* btn = new QPushButton(entry.emoji + "  " + entry.mood);
        btn->setFixedHeight(38);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setCheckable(true);
        btn->setStyleSheet(
            "QPushButton {"
            "   background-color: #252d3a;"
            "   color: #ffffff;"
            "   border: none;"
            "   border-radius: 10px;"
            "   font-size: 12px;"
            "}"
            "QPushButton:hover { background-color: #2d3748; }"
            "QPushButton:checked { background-color: #e94560; }"
        );
        connect(btn, &QPushButton::clicked, this, [this, mood = entry.mood]() {
            onMoodClicked(mood);
        });
        grid->addWidget(btn, i / 2, i % 2);
    }

    m_outerLayout->addWidget(m_moodGrid);
}

void RecommendationPanelWidget::buildThemeButtons() {
    m_themeContainer = new QWidget();
    m_themeContainer->setStyleSheet("background: transparent;");
    QVBoxLayout* layout = new QVBoxLayout(m_themeContainer);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);

    struct ThemeEntry { QString theme; QString label; QString emoji; };
    QVector<ThemeEntry> themes = {
        { "study",   "Study Focus",  "📚" },
        { "workout", "Workout Mix",  "💪" },
        { "party",   "Party Mode",   "🎉" },
    };

    for (const auto& t : themes) {
        QPushButton* btn = new QPushButton(t.emoji + "  " + t.label);
        btn->setFixedHeight(40);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(
            "QPushButton {"
            "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
            "       stop:0 #0f3460, stop:1 #16213e);"
            "   color: #ffffff;"
            "   border: 1px solid #e9456040;"
            "   border-radius: 10px;"
            "   font-size: 13px;"
            "   text-align: left;"
            "   padding-left: 12px;"
            "}"
            "QPushButton:hover { border-color: #e94560; background-color: #1a2744; }"
            "QPushButton:pressed { background-color: #e94560; }"
        );
        connect(btn, &QPushButton::clicked, this, [this, theme = t.theme]() {
            emit playlistThemeRequested(theme);
        });
        layout->addWidget(btn);
    }

    m_outerLayout->addWidget(m_themeContainer);
}

void RecommendationPanelWidget::onMoodClicked(const QString& mood) {

    m_activeMood = (m_activeMood == mood) ? "" : mood;


    QGridLayout* grid = qobject_cast<QGridLayout*>(m_moodGrid->layout());
    if (grid) {
        for (int i = 0; i < grid->count(); ++i) {
            QWidget* w = grid->itemAt(i)->widget();
            if (QPushButton* btn = qobject_cast<QPushButton*>(w)) {
                bool shouldCheck = btn->text().contains(mood) && !m_activeMood.isEmpty();
                btn->setChecked(shouldCheck);
            }
        }
    }

    emit moodSelected(m_activeMood);
}

void RecommendationPanelWidget::setRecommendedSongs(
        const QVector<QPair<QString, QString>>& songs) {

    QLayoutItem* item;
    while ((item = m_songListLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if (songs.isEmpty()) {
        QLabel* lbl = new QLabel("🎵  Play a song to get suggestions");
        lbl->setStyleSheet(
            "color: #a0a0a0; font-size: 12px;"
            "background-color: #252d3a; border-radius: 8px; padding: 8px;"
        );
        m_songListLayout->addWidget(lbl);
    } else {
        for (const auto& [title, artist] : songs) {
            QWidget* card = new QWidget();
            card->setStyleSheet(
                "background-color: #252d3a; border-radius: 8px;"
            );
            QVBoxLayout* cl = new QVBoxLayout(card);
            cl->setContentsMargins(10, 6, 10, 6);
            cl->setSpacing(2);

            QLabel* titleLbl = new QLabel(title);
            titleLbl->setStyleSheet("color: #ffffff; font-size: 13px; font-weight: bold; background: transparent;");
            titleLbl->setWordWrap(true);

            QLabel* artistLbl = new QLabel(artist);
            artistLbl->setStyleSheet("color: #a0a0a0; font-size: 11px; background: transparent;");

            cl->addWidget(titleLbl);
            cl->addWidget(artistLbl);
            m_songListLayout->addWidget(card);
        }
    }

    m_songListLayout->addStretch();
}

void RecommendationPanelWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(C_BG);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    painter.setPen(QPen(QColor(255, 255, 255, 18), 1));
    painter.drawLine(0, 0, 0, height());

    QWidget::paintEvent(event);
}