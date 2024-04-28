#ifndef TABLE_H
#define TABLE_H

#include <QDialog>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonObject>
#include <QMenu>
#include <QMouseEvent>
#include <QString>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QUrl>
#include <QVariantMap>

#include <include/song.h>


class Table : public QTableWidget {
    Q_OBJECT

 public:
    explicit Table(QWidget* parent = nullptr);

    void addSong(const QJsonObject&);
    void editSong();
    int findIndex(const QString&);
    QString findUID(int);
    QString getUID();
    QVariantMap getMetadata(const QString& uid) const;
    void mousePressEvent(QMouseEvent*);
    void playSong();
    void removeSong();
    void sendMetadata();
    void showContextMenu(const QPoint&);

 signals:
    void addSongRequested(const QString&);
    void doubleClicked(const QVariantMap&);
    void itemSelected(const QVariantMap&);
    void playSongRequested(const QVariantMap&);
    void playlistLoaded(const QJsonObject&);
    void playlistSorted();
    void previousSongRequested(const QVariantMap&);
    void nextSongRequested(const QVariantMap&);
    void requestPlaylist();
    void songAdded(const QJsonObject&);
    void songDeleted(const QString&);
    void songRemoved(const QString&);
    void songEdited(const QVariantMap&);
    void songUpdated(const QVariantMap&);

 public slots:
    void onItemDoubleClicked(QTableWidgetItem*);
    void onLoadPlaylist(const QJsonObject&);
    void onPlaylistIndexChanged(const QString&);
    void onPlaylistSelected(const QString&);
    void onPlaylistSorted();
    void onSongAdded(const QJsonObject&);

 private:
    QPoint position;
    QString uid;
    QJsonObject playlist;
};

#endif // TABLE_H
