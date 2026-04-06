#ifndef PLAYLISTPANELWIDGET_HPP
#define PLAYLISTPANELWIDGET_HPP

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QPainter>
#include <QString>
#include <QVector>
#include <QInputDialog>

class PlaylistPanelWidget : public QWidget {
    Q_OBJECT

public:
    explicit PlaylistPanelWidget(QWidget *parent = nullptr);

    void addPlaylist(const QString& name);
    void removePlaylist(const QString& name);

signals:
    void playlistSelected(const QString& name);
    void createPlaylistRequested(const QString& name);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onCreateClicked();
    void onPlaylistClicked(const QString& name);

private:
    void setupUI();
    void rebuildList();

    QVBoxLayout* m_outerLayout;
    QWidget*     m_listContainer;
    QVBoxLayout* m_listLayout;
    QScrollArea* m_scrollArea;
    QString      m_selectedPlaylist;

    QVector<QString> m_playlists;

    // Colors
    const QColor C_BG      = QColor("#1e2736");
    const QColor C_PRIMARY = QColor("#e94560");
    const QColor C_TEXT    = QColor("#ffffff");
    const QColor C_SUBTEXT = QColor("#a0a0a0");
    const QColor C_ITEM    = QColor("#252d3a");
    const QColor C_HOVER   = QColor("#2d3748");
};

#endif