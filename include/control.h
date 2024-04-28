#ifndef CONTROL_H
#define CONTROL_H

#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QFile>
#include <QGuiApplication>
#include <QHeaderView>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QQueue>
#include <QRandomGenerator>
#include <QScreen>
#include <QSlider>
#include <QSvgRenderer>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include "include/slider.h"


enum class ActionType {
    Add,
    Remove
};


struct Action {
    ActionType type;
    QJsonObject metadata;
    QString uid;
    QJsonObject songs;
};


class Control : public QWidget {
    Q_OBJECT

 public:
    explicit Control(QWidget* parent = nullptr);

    QMediaPlayer* getPlayer();
    QString getUID();
    void loadPlaylist(const QJsonArray&);
    void processQueue();
    void queueAddSongToPlaylist(const QJsonObject&);
    void queueRemoveSongFromPlaylist(const QString&);
    void resetLabel();
    void updateTheme();
    void updateVisualizeIcon(bool);

 public slots:
    void onAddSongToPlaylist(const QJsonObject&);
    void onDirectSeek(int);
    void onGetCurrentSong(const QString&);
    void onGetNextSong(const QVariantMap&);
    void onGetPreviousSong(const QVariantMap&);
    void onLoadPlaylist(const QJsonObject&);
    void onPlaylistIndexChanged(int);
    void onPlaylistSorted();
    void onPlayNext();
    void onPlayPrevious();
    void onPlaySong(const QVariantMap&);
    void onPlayerStateChanged(QMediaPlayer::State);
    void onRemoveSongFromPlaylist(const QString&);
    void onSeek(int);
    void onSetVolume(int);
    void onSliderPressed();
    void onSliderReleased();
    void onStorePlaylist(const QJsonObject&);
    void onToggleMute();
    void onTogglePlayPause();
    void onToggleRepeat();
    void onToggleShuffle();
    void onUpdateDuration(qint64);
    void onUpdateElapsedTime(qint64);
    void onUpdatePlayPauseButton(QMediaPlayer::State);
    void onUpdateSong(const QVariantMap&);
    void onVisualizeClicked();

 signals:
    void metadataChanged(const QVariantMap&);
    void metadataRequested();
    void nextTrackRequested();
    void playlistIndexChanged(const QString&);
    void previousTrackRequested();
    void queueProcessed(const QString&);
    void songChanged(const QString&);
    void songPlaying(const QString&);
    void songFinished();
    void visualizeClicked();

 private:
    QMediaPlayer* player;
    QMediaPlaylist* playlist;
    QJsonObject current;

    QVariantMap previous;
    QString uid;
    QVariantMap next;
    QString name;
    QMap<QString, int> original;
    QQueue<Action> queue;

    QLabel* iconLabel;
    QPushButton* playPauseButton;
    QPushButton* stopButton;
    QPushButton* nextButton;
    QPushButton* prevButton;
    QPushButton* shuffleButton;
    QPushButton* repeatButton;
    QPushButton* visualizeButton;
    QPushButton* volumeButton;

    QSlider* volumeSlider;
    Slider* progressSlider;

    QLabel* titleLabel;
    QLabel* artistLabel;
    QLabel* elapsedLabel;
    QLabel* durationLabel;

    bool isMuted = false;
    bool isPlaying = false;
    bool isRepeat = false;
    bool isShuffle = false;
    bool isVisualizing = false;
};

#endif // CONTROL_H
