#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "Song.hpp"
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPainter>
#include <QTimer>
#include <memory>
#include <vector>

using namespace std;

class AppController;
class PlayerBarWidget;
class PlaylistPanelWidget;
class RecommendationPanelWidget;
class SongRowWidget;


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setController(AppController* controller);
    void showWelcomeScreen();
    void showLibrary();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onTabChanged(int index);
    void onSongsLoaded();
    void onPlaylistSelected(const QString& name);
    void onMoodFilterSelected(const QString& mood);
    void onSongPlayRequested(shared_ptr<Song> song);

private:
    void setupUI();
    void setupCentralWidget();
    void setupTopBar();
    void setupContentArea();
    void setupSidebars();
    void setupPlayerBar();
    void applyStyles();
    void createAnimations();
    void updateSongList();
    void clearSongList();
    void showSongVector(const vector<shared_ptr<Song>>& songs);
    void animateParticles();

    // UI Components
    QWidget*      m_centralWidget;
    QHBoxLayout*  m_mainLayout;

    PlaylistPanelWidget*       m_playlistPanel;
    RecommendationPanelWidget* m_recommendationPanel;

    QWidget*      m_contentWidget;
    QVBoxLayout*  m_contentLayout;

    QWidget*      m_topBar;
    QLabel*       m_onlineIndicator;
    QLineEdit*    m_searchBar;
    QPushButton*  m_settingsBtn;

    QWidget*      m_contentArea;
    QScrollArea*  m_songScrollArea;
    QWidget*      m_songListContainer;
    QVBoxLayout*  m_songListLayout;

    QWidget*      m_tabWidget;
    QHBoxLayout*  m_tabLayout;
    vector<QPushButton*> m_tabs;
    int           m_currentTab;

    PlayerBarWidget* m_playerBar;
    AppController*   m_controller;

    QTimer*           m_particleTimer;
    vector<QPointF>   m_particlePositions;
    vector<qreal>     m_particleOffsets;

    QString m_currentFilter;
    QString m_currentMood;

    const QColor COLOR_BG_DARK   = QColor("#1a1a2e");
    const QColor COLOR_BG_MEDIUM = QColor("#16213e");
    const QColor COLOR_BG_LIGHT  = QColor("#0f3460");
    const QColor COLOR_PRIMARY   = QColor("#e94560");
    const QColor COLOR_CARD_BG   = QColor("#252d3a");
    const QColor COLOR_HOVER_BG  = QColor("#2d3748");
    const QColor COLOR_TEXT_PRIMARY   = QColor("#ffffff");
    const QColor COLOR_TEXT_SECONDARY = QColor("#a0a0a0");
};


class CardWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit CardWidget(QWidget *parent = nullptr);

    void setCornerRadius(int radius) { m_cornerRadius = radius; update(); }
    void setCardColor(const QColor& color) { m_cardColor = color; update(); }
    void enableHoverEffect(bool enable) { m_hoverEnabled = enable; }

    qreal opacity() const { return m_opacity; }
    void  setOpacity(qreal opacity) { m_opacity = opacity; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    int   m_cornerRadius;
    QColor m_cardColor;
    bool  m_hoverEnabled;
    bool  m_isHovered;
    qreal m_opacity;
    QPropertyAnimation* m_hoverAnimation;
};


class TabButton : public QPushButton {
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)

public:
    explicit TabButton(const QString& text, QWidget *parent = nullptr);

    void setActive(bool active);
    bool isActive() const { return m_isActive; }

    qreal scale() const { return m_scale; }
    void  setScale(qreal scale) { m_scale = scale; update(); }

protected:
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    bool  m_isActive;
    bool  m_isHovered;
    QPropertyAnimation* m_animation;
    qreal m_scale;
};

#endif // MAINWINDOW_HPP