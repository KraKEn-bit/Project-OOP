#include "PlaylistPanelWidget.hpp"
#include <QPainter>
#include <QPainterPath>
#include <QInputDialog>

PlaylistPanelWidget::PlaylistPanelWidget(QWidget *parent)
    : QWidget(parent)
{
    setFixedWidth(250);
    m_playlists = { "Favorites", "Recently Played", "Study Focus", "Workout Mix" };
    setupUI();
}

void PlaylistPanelWidget::setupUI() {
    m_outerLayout = new QVBoxLayout(this);
    m_outerLayout->setContentsMargins(12, 20, 12, 12);
    m_outerLayout->setSpacing(14);

    // Header
    QLabel* title = new QLabel("📂  Playlists");
    title->setStyleSheet(
        "color: #ffffff;"
        "font-size: 16px;"
        "font-weight: bold;"
        "background: transparent;"
    );
    m_outerLayout->addWidget(title);

    //  playlist button
    QPushButton* createBtn = new QPushButton("＋  New Playlist");
    createBtn->setFixedHeight(40);
    createBtn->setCursor(Qt::PointingHandCursor);
    createBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #e94560;"
        "   color: #ffffff;"
        "   border: none;"
        "   border-radius: 20px;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover { background-color: #d32f4f; }"
        "QPushButton:pressed { background-color: #b71c1c; }"
    );
    connect(createBtn, &QPushButton::clicked, this, &PlaylistPanelWidget::onCreateClicked);
    m_outerLayout->addWidget(createBtn);

    // Divider label
    QLabel* divider = new QLabel("YOUR PLAYLISTS");
    divider->setStyleSheet("color: #a0a0a0; font-size: 10px; letter-spacing: 1px; background: transparent;");
    m_outerLayout->addWidget(divider);

    // Scroll area for playlist list
    m_scrollArea = new QScrollArea();
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical { background: rgba(45,55,72,0.3); width: 6px; border-radius: 3px; }"
        "QScrollBar::handle:vertical { background: #e94560; border-radius: 3px; min-height: 20px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
    );

    m_listContainer = new QWidget();
    m_listContainer->setStyleSheet("background: transparent;");
    m_listLayout = new QVBoxLayout(m_listContainer);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->setSpacing(6);

    m_scrollArea->setWidget(m_listContainer);
    m_outerLayout->addWidget(m_scrollArea, 1);

    rebuildList();
}

void PlaylistPanelWidget::rebuildList() {

    QLayoutItem* item;
    while ((item = m_listLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (const QString& name : m_playlists) {
        QPushButton* btn = new QPushButton("  🎵  " + name);
        btn->setFixedHeight(44);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setCheckable(true);
        btn->setChecked(name == m_selectedPlaylist);

        QString style =
            "QPushButton {"
            "   background-color: " + QString(name == m_selectedPlaylist ? "#e94560" : "#252d3a") + ";"
            "   color: #ffffff;"
            "   border: none;"
            "   border-radius: 10px;"
            "   text-align: left;"
            "   padding-left: 8px;"
            "   font-size: 13px;"
            "}"
            "QPushButton:hover { background-color: " + QString(name == m_selectedPlaylist ? "#d32f4f" : "#2d3748") + "; }"
            "QPushButton:pressed { background-color: #e94560; }";
        btn->setStyleSheet(style);

        connect(btn, &QPushButton::clicked, this, [this, name]() {
            onPlaylistClicked(name);
        });

        m_listLayout->addWidget(btn);
    }
    m_listLayout->addStretch();
}

void PlaylistPanelWidget::onCreateClicked() {
    bool ok = false;
    QString name = QInputDialog::getText(
        this, "New Playlist", "Playlist name:", QLineEdit::Normal, "", &ok
    );
    if (ok && !name.trimmed().isEmpty()) {
        addPlaylist(name.trimmed());
        emit createPlaylistRequested(name.trimmed());
    }
}

void PlaylistPanelWidget::onPlaylistClicked(const QString& name) {
    m_selectedPlaylist = name;
    rebuildList();
    emit playlistSelected(name);
}

void PlaylistPanelWidget::addPlaylist(const QString& name) {
    if (!m_playlists.contains(name)) {
        m_playlists.append(name);
        rebuildList();
    }
}

void PlaylistPanelWidget::removePlaylist(const QString& name) {
    m_playlists.removeAll(name);
    if (m_selectedPlaylist == name) m_selectedPlaylist.clear();
    rebuildList();
}

void PlaylistPanelWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(C_BG);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(255, 255, 255, 18), 1));
    painter.drawLine(width() - 1, 0, width() - 1, height());

    QWidget::paintEvent(event);
}