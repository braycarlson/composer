#include "include/playlist.h"


Playlist::Playlist(QWidget* parent) : QListWidget(parent) {
    this->player = new QMediaPlayer(this);
    this->onLoadPlaylist();

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->setMinimumWidth(100);
    this->setMaximumWidth(400);

    this->connect(this, &QListWidget::itemClicked, this, &Playlist::onSelectPlaylist);
    this->connect(this, &Playlist::playlistUpdated, this, &Playlist::onUpdatePlaylistUI);
    this->connect(this, &Playlist::rightClicked, this, &Playlist::onShowEmptyContextMenu);
    this->connect(this, &QListWidget::customContextMenuRequested, this, &Playlist::onShowContextMenu);
    this->connect(this, &QListWidget::itemSelectionChanged, this, &Playlist::onGetSongs);
}

void Playlist::addSong(const QString& name, QJsonObject& song) {
    QString update;

    foreach (const QString& filename, data.keys()) {
        QJsonObject playlist = data.value(filename);

        if (playlist["name"].toString() == name) {
            update = filename;
            break;
        }
    }

    if (!update.isEmpty()) {
        QJsonObject& playlist = data[update];
        QJsonArray songs = playlist["songs"].toArray();

        QString uid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        song["uid"] = uid;

        songs.append(song);
        playlist["songs"] = songs;

        this->savePlaylist(update, playlist);
        emit this->playlistUpdated(name);
    }
}

void Playlist::onBrowseForSong(const QString& playlist) {
    QString path = QFileDialog::getOpenFileName(
        this,
        tr("Select song"),
        "music",
        tr("Audio files (*.mp3 *.wav *.flac)")
    );

    if (!path.isEmpty()) {
        this->player->setMedia(QMediaContent());
        this->player->setMedia(QUrl::fromLocalFile(path));

        this->disconnect(this->player, &QMediaPlayer::mediaStatusChanged, nullptr, nullptr);

        this->connect(this->player, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::LoadedMedia) {
                QString title = this->player->metaData(QMediaMetaData::Title).toString();
                QString artist = this->player->metaData(QMediaMetaData::AlbumArtist).toString();
                QString album = this->player->metaData(QMediaMetaData::AlbumTitle).toString();
                qint64 duration = this->player->duration();

                int seconds = (duration / 1000) % 60;
                int minutes = (duration / (1000 * 60)) % 60;

                QJsonObject song;
                song["title"] = title.isEmpty() ? QFileInfo(path).baseName() : title;
                song["artist"] = artist.isEmpty() ? "Unknown" : artist;
                song["album"] = album.isEmpty() ? "Unknown" : album;
                song["path"] = path;
                song["duration"] = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));

                this->addSong(playlist, song);
                emit this->songAdded(song);

                this->disconnect(this->player, &QMediaPlayer::mediaStatusChanged, this, nullptr);
            }
        });
    }
}

void Playlist::createPlaylist() {
    bool ok;

    QString name = QInputDialog::getText(
        this,
        tr("New Playlist"),
        tr("Playlist name:"),
        QLineEdit::Normal,
        QString(),
        &ok
    );

    if (ok && !name.isEmpty()) {
        if (this->findItems(name, Qt::MatchExactly).isEmpty()) {
            QJsonObject update;
            update["name"] = name;
            update["songs"] = QJsonArray();

            QString filename = QUuid::createUuid().toString(QUuid::WithoutBraces) + ".json";
            this->savePlaylist(filename, update);
            this->onLoadPlaylist();

            QList<QListWidgetItem*> items = this->findItems(name, Qt::MatchExactly);

            if (!items.isEmpty()) {
                this->setCurrentItem(items.first());
                this->onUpdatePlaylistUI(name);
            }
        } else {
            QMessageBox::critical(
                this,
                tr("Error"),
                tr("A playlist with the name '%1' exists. Please choose a different name.").arg(name)
            );
        }
    }
}

void Playlist::deletePlaylist(QListWidgetItem* item) {
    QString name = item->text();
    int row = this->row(item);

    if (this->mapping.contains(name)) {
        QString filename = this->mapping[name];
        this->data.remove(filename);

        QFile::remove("playlist/" + filename);
        this->mapping.remove(name);

        delete item;

        int count = this->count();

        if (count > 0) {
            if (row >= count) {
                row = count - 1;
            }

            this->setCurrentRow(row);

            QListWidgetItem* next = this->item(row);

            if (next) {
                name = next->text();
                this->onUpdatePlaylistUI(name);
            }
        }
    }
}

QString Playlist::getPlaylistFilename(const QString& name) const {
    return this->mapping.value(name, QString());
}

QJsonObject Playlist::getPlaylist(const QString& name) const {
    QString filename = this->getPlaylistFilename(name);
    return this->data.value(filename, QJsonObject());
}

void Playlist::onGetSongs() {
    QListWidgetItem* item = this->currentItem();

    if (item) {
        QString name = item->text();
        QJsonObject playlist = this->getPlaylist(name);

        if (!playlist.isEmpty()) {
            emit this->playlistLoaded(playlist);
        } else {
            emit this->playlistLoaded(QJsonObject());
        }
    } else {
        emit this->playlistLoaded(QJsonObject());
    }
}

void Playlist::onHandlePlaylistSelected() {
    int index = this->currentRow();
    QString name;

    if (index >= 0) {
        QListWidgetItem* item = this->item(index);

        if (item) {
            name = item->text();
        }
    }

    emit this->playlistNameSelected(name);
}

void Playlist::onLoadPlaylist() {
    this->clear();

    this->data.clear();
    this->mapping.clear();

    QDir directory("playlist");
    QStringList files = directory.entryList(QStringList() << "*.json", QDir::Files);
    QMap<QString, QString> playlists;

    foreach (const QString& filename, files) {
        QFile file(directory.absoluteFilePath(filename));

        if (file.open(QIODevice::ReadOnly)) {
            QByteArray content = file.readAll();
            QJsonDocument document = QJsonDocument::fromJson(content);

            if (!document.isNull() && document.isObject()) {
                QJsonObject object = document.object();
                QString name = object["name"].toString();
                playlists[name] = filename;
            }
        }
    }

    QList<QString> names = playlists.keys();

    std::sort(names.begin(), names.end(), [](const QString& a, const QString& b) {
        return QString::compare(a, b, Qt::CaseInsensitive) < 0;
    });

    foreach (const QString& name, names) {
        QString filename = playlists[name];
        QFile file(directory.absoluteFilePath(filename));

        if (file.open(QIODevice::ReadOnly)) {
            QByteArray content = file.readAll();
            QJsonDocument document = QJsonDocument::fromJson(content);
            QJsonObject object = document.object();

            this->addItem(name);
            this->data[filename] = object;
            this->mapping[name] = filename;
        }
    }
}

void Playlist::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton && itemAt(event->pos()) == nullptr) {
        emit this->rightClicked();
    } else {
        QListWidget::mousePressEvent(event);
    }
}

void Playlist::onDoubleClicked(const QVariantMap& metadata) {
    emit this->songSelected(metadata, this->data);
}

void Playlist::renamePlaylist(QListWidgetItem* item) {
    bool ok;
    QString previous = item->text();

    QString name = QInputDialog::getText(
        this,
        tr("Rename playlist"),
        tr("Playlist name:"),
        QLineEdit::Normal,
        previous,
        &ok
    );

    if (ok && !name.isEmpty() && this->mapping.contains(previous)) {
        if (this->mapping.contains(name) && name != this->mapping[previous]) {
            QMessageBox::critical(
                this,
                tr("Error"),
                tr("A playlist with the name '%1' exists. Please choose a different name.").arg(name)
            );

            return;
        }

        QString filename = this->mapping[previous];
        QJsonObject& playlist = data[filename];
        playlist["name"] = name;
        this->savePlaylist(filename, playlist);

        this->mapping.remove(previous);
        this->mapping[name] = filename;

        item->setText(name);
        emit this->playlistUpdated(name);
    }
}

void Playlist::savePlaylist(const QString& filename, const QJsonObject& playlist) {
    QFile file("playlist/" + filename);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(playlist).toJson());
    }
}

void Playlist::setFirstPlaylist() {
    if (this->count() > 0) {
        this->setCurrentRow(0);
    }
}

void Playlist::onSelectPlaylist(QListWidgetItem* item) {
    if (item) {
        QString name = item->text();
        emit this->playlistSelected(name);
    }
}

void Playlist::onShowContextMenu(const QPoint& pos) {
    QListWidgetItem* item = this->itemAt(pos);

    if (item) {
        this->setCurrentItem(item);
        QString name = item->text();
        this->onUpdatePlaylistUI(name);
    } else {
        return;
    }

    QMenu menu;

    bool hasPlaylists = !this->count() == 0;

    QAction* addSongAction = new QAction(tr("Add song to playlist"), &menu);
    QAction* renameAction = new QAction(tr("Rename playlist"), &menu);
    QAction* deleteAction = new QAction(tr("Delete playlist"), &menu);

    addSongAction->setEnabled(hasPlaylists);
    renameAction->setEnabled(hasPlaylists);
    deleteAction->setEnabled(hasPlaylists);

    menu.addAction(addSongAction);
    menu.addAction(renameAction);
    menu.addAction(deleteAction);

    this->connect(addSongAction, &QAction::triggered, this, [this, pos]() {
        QListWidgetItem* item = this->itemAt(pos);

        if (item) {
            this->onBrowseForSong(item->text());
        }
    });

    this->connect(renameAction, &QAction::triggered, this, [this, pos]() {
        QListWidgetItem* item = this->itemAt(pos);

        if (item) {
            this->renamePlaylist(item);
        }
    });

    this->connect(deleteAction, &QAction::triggered, this, [this, pos]() {
        QListWidgetItem* item = this->itemAt(pos);

        if (item) {
            this->deletePlaylist(item);
        }
    });

    menu.exec(this->mapToGlobal(pos));
}

void Playlist::onShowEmptyContextMenu() {
    QMenu menu;

    QAction* createAction = new QAction(tr("Create playlist"), &menu);
    this->connect(createAction, &QAction::triggered, this, &Playlist::createPlaylist);

    menu.addAction(createAction);
    menu.exec(QCursor::pos());
}

void Playlist::onUpdatePlaylistUI(const QString& name) {
    this->clear();
    this->onLoadPlaylist();

    for (int i = 0; i < this->count(); ++i) {
        QListWidgetItem* item = this->item(i);

        if (item->text() == name) {
            this->setCurrentItem(item);
            break;
        }
    }

    emit this->playlistSelected(name);
}

void Playlist::onDeleteSong(const QString& uid) {
    QListWidgetItem* item = this->currentItem();
    if (!item) return;

    QString current = item->text();
    if (current.isEmpty()) return;

    QString filename = this->getPlaylistFilename(current);
    if (filename.isEmpty()) return;

    QJsonObject playlist = this->getPlaylist(current);
    QJsonArray songs = playlist["songs"].toArray();

    QJsonArray update;

    for (const QJsonValue &value : songs) {
        QJsonObject song = value.toObject();

        if (song["uid"].toString() != uid) {
            update.append(song);
        }
    }

    playlist["songs"] = update;
    this->savePlaylist(filename, playlist);
    this->onUpdatePlaylistUI(current);
}

void Playlist::onUpdateSong(const QVariantMap& metadata) {
    QListWidgetItem* item = this->currentItem();
    if (!item) return;

    QString current = item->text();
    if (current.isEmpty()) return;

    QString filename = this->getPlaylistFilename(current);

    if (filename.isEmpty()) {
        return;
    }

    QJsonObject playlist = this->getPlaylist(current);
    QJsonArray songs = playlist["songs"].toArray();
    bool isUpdated = false;

    for (int i = 0; i < songs.size(); ++i) {
        QJsonObject song = songs[i].toObject();

        if (song["uid"].toString() == metadata["uid"].toString()) {
            song["title"] = metadata["title"].toString();
            song["artist"] = metadata["artist"].toString();
            song["album"] = metadata["album"].toString();

            songs[i] = song;
            isUpdated = true;
            break;
        }
    }

    if (isUpdated) {
        playlist["songs"] = songs;
        this->savePlaylist(filename, playlist);
        this->onUpdatePlaylistUI(current);
    }
}
