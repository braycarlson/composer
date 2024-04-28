#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QListWidget>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPointer>
#include <QTimer>


class Playlist : public QListWidget {
    Q_OBJECT

 public:
    explicit Playlist(QWidget* parent = nullptr);

    void addSong(const QString&, QJsonObject&);
    void createPlaylist();
    void deletePlaylist(QListWidgetItem*);
    QJsonObject getPlaylist(const QString&) const;
    QString getPlaylistFilename(const QString&) const;
    void renamePlaylist(QListWidgetItem*);
    void savePlaylist(const QString&, const QJsonObject&);
    void setFirstPlaylist();

 protected:
    void mousePressEvent(QMouseEvent*) override;

 public slots:
    void onBrowseForSong(const QString&);
    void onDoubleClicked(const QVariantMap&);
    void onGetSongs();
    void onHandlePlaylistSelected();
    void onLoadPlaylist();
    void onDeleteSong(const QString&);
    void onSelectPlaylist(QListWidgetItem*);
    void onShowContextMenu(const QPoint&);
    void onShowEmptyContextMenu();
    void onUpdatePlaylistUI(const QString&);
    void onUpdateSong(const QVariantMap&);

 signals:
    void playlistNameSelected(const QString&);
    void playlistSelected(const QString&);
    void playlistLoaded(const QJsonObject&);
    void playlistUpdated(const QString&);
    void rightClicked();
    void songAdded(const QJsonObject&);
    void songDeleted(const QJsonObject&);
    void songSelected(const QVariantMap&, const QMap<QString, QJsonObject>&);

 private:
    QMediaPlayer* player;
    QMap<QString, QJsonObject> data;
    QMap<QString, QString> mapping;
};

#endif // PLAYLIST_H
