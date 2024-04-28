#include "include/table.h"


Table::Table(QWidget* parent) : QTableWidget(parent) {
    this->setColumnCount(4);

    QStringList header = {"Title", "Artist", "Album", "Duration"};
    this->setHorizontalHeaderLabels(header);

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
    this->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->verticalHeader()->setVisible(false);
    this->horizontalHeader()->setHighlightSections(false);
    this->setSortingEnabled(true);

    this->setContextMenuPolicy(Qt::CustomContextMenu);

    this->connect(this, &Table::itemDoubleClicked, this, &Table::onItemDoubleClicked);
    this->connect(this, &QTableWidget::customContextMenuRequested, this, &Table::showContextMenu);
    this->connect(this->horizontalHeader(), &QHeaderView::sectionClicked, this, &Table::onPlaylistSorted);
}

void Table::onSongAdded(const QJsonObject& song) {
    emit this->songAdded(song);

    int current = this->findIndex(this->uid);
    if (current == -1) return;

    QString add = song.value("uid").toString();
    int added = this->findIndex(add);

    QString previous, next;

    if (added < current) {
        if (added == current - 1) {
            previous = add;
        }

        if (added < current - 1) {
            previous = this->findUID(current - 1);
        }
    }
    else if (added > current) {
        if (added == current + 1) {
            next = add;
        }

        if (added > current + 1) {
            next = this->findUID(current + 1);
        }
    }

    QVariantMap metadata;

    if (!previous.isEmpty()) {
        metadata = this->getMetadata(previous);
        emit this->previousSongRequested(metadata);
    }

    if (!next.isEmpty()) {
        metadata = this->getMetadata(next);
        emit this->nextSongRequested(metadata);
    }

    if (this->uid != QString("")) {
        int index = this->findIndex(this->uid);

        this->clearSelection();
        this->setCurrentCell(index, 0);
        this->selectRow(index);
        this->scrollToItem(this->item(index, 0), QAbstractItemView::PositionAtCenter);
    }
}

QString Table::findUID(int index) {
    int count = this->rowCount();

    if (index >= 0 && index < count) {
        return this->item(index, 0)->data(Qt::UserRole).toString();
    }

    return QString();
}

int Table::findIndex(const QString& uid) {
    int count = this->rowCount();

    for (int i = 0; i < count; ++i) {
        if (this->item(i, 0)->data(Qt::UserRole).toString() == uid) {
            return i;
        }
    }

    return -1;
}

void Table::editSong() {
    int row = this->currentRow();
    if (row == -1) return;

    QVariantMap metadata;
    metadata["uid"] = item(row, 0)->data(Qt::UserRole).toString();
    metadata["path"] = item(row, 0)->data(Qt::UserRole + 1).toString();
    metadata["title"] = item(row, 0)->text();
    metadata["artist"] = item(row, 1)->text();
    metadata["album"] = item(row, 2)->text();
    metadata["duration"] = item(row, 3)->text();

    SongDialog dialog(metadata, this);

    if (dialog.exec() == QDialog::Accepted) {
        QVariantMap update = dialog.updateMetadata();

        item(row, 0)->setText(update["title"].toString());
        item(row, 1)->setText(update["artist"].toString());
        item(row, 2)->setText(update["album"].toString());

        emit this->songUpdated(update);

        if (item(row, 0)->data(Qt::UserRole).toString() == this->uid) {
            emit this->songEdited(update);
        }
    }

    if (this->uid != QString("")) {
        int index = this->findIndex(this->uid);

        this->clearSelection();
        this->setCurrentCell(index, 0);
        this->selectRow(index);
        this->scrollToItem(this->item(index, 0), QAbstractItemView::PositionAtCenter);
    }
}

void Table::onLoadPlaylist(const QJsonObject& playlist) {
    this->clearContents();
    this->setRowCount(0);

    this->setSortingEnabled(false);

    this->playlist = playlist;

    QJsonArray songs = playlist["songs"].toArray();

    foreach (const QJsonValue& song, songs) {
        QJsonObject object = song.toObject();

        QVariantMap metadata;
        metadata.insert("uid", object["uid"].toString());
        metadata.insert("path", object["path"].toString());
        metadata.insert("title", object["title"].toString());
        metadata.insert("artist", object["artist"].toString());
        metadata.insert("album", object["album"].toString());
        metadata.insert("duration", object["duration"].toString());

        int row = this->rowCount();
        this->insertRow(row);

        QTableWidgetItem* item = new QTableWidgetItem(metadata.value("title").toString());

        this->setItem(row, 0, item);
        this->setItem(row, 1, new QTableWidgetItem(metadata.value("artist").toString()));
        this->setItem(row, 2, new QTableWidgetItem(metadata.value("album").toString()));
        this->setItem(row, 3, new QTableWidgetItem(metadata.value("duration").toString()));

        item->setData(Qt::UserRole, metadata.value("uid").toString());
        item->setData(Qt::UserRole + 1, metadata.value("path").toString());

    }

    this->setSortingEnabled(true);
    emit this->playlistLoaded(playlist);
}

void Table::mousePressEvent(QMouseEvent* event) {
    QModelIndex index = this->indexAt(event->pos());

    if (!index.isValid()) {
        this->clearSelection();
        this->setCurrentCell(-1, -1);
    }

    QTableWidget::mousePressEvent(event);
}

void Table::onItemDoubleClicked(QTableWidgetItem* song) {
    if (!song) return;
    int row = song->row();

    QVariantMap metadata;
    metadata["index"] = row;
    metadata["uid"] = item(row, 0)->data(Qt::UserRole).toString();
    metadata["path"] = item(row, 0)->data(Qt::UserRole + 1).toString();
    metadata["title"] = item(row, 0)->text();
    metadata["artist"] = item(row, 1)->text();
    metadata["album"] = item(row, 2)->text();
    metadata["duration"] = item(row, 3)->text();

    emit this->doubleClicked(metadata);
}

void Table::onPlaylistSorted() {
    emit this->playlistSorted();
}

void Table::onPlaylistIndexChanged(const QString& uid) {
    for (int row = 0; row < this->rowCount(); ++row) {
        if (this->item(row, 0)->data(Qt::UserRole).toString() == uid) {
            this->uid = uid;

            QString previous, next;
            QVariantMap metadata;

            if (row == 0) {
                previous = this->item(this->rowCount() - 1, 0)->data(Qt::UserRole).toString();
            } else {
                previous = this->item(row - 1, 0)->data(Qt::UserRole).toString();
            }

            if (row == this->rowCount() - 1) {
                next = this->item(0, 0)->data(Qt::UserRole).toString();
            } else {
                next = this->item(row + 1, 0)->data(Qt::UserRole).toString();
            }

            metadata = this->getMetadata(previous);
            emit this->previousSongRequested(metadata);

            metadata = this->getMetadata(next);
            emit this->nextSongRequested(metadata);

            this->clearSelection();
            this->selectRow(row);
            this->scrollToItem(this->item(row, 0), QAbstractItemView::PositionAtCenter);

            return;
        }
    }
}

QVariantMap Table::getMetadata(const QString& uid) const {
    QVariantMap metadata;

    for (int row = 0; row < this->rowCount(); ++row) {
        if (this->item(row, 0)->data(Qt::UserRole).toString() == uid) {
            metadata["uid"] = uid;
            metadata["path"] = this->item(row, 0)->data(Qt::UserRole + 1).toString();
            metadata["title"] = this->item(row, 0)->text();
            metadata["artist"] = this->item(row, 1)->text();
            metadata["album"] = this->item(row, 2)->text();
            metadata["duration"] = this->item(row, 3)->text();

            break;
        }
    }

    return metadata;
}

void Table::playSong() {
    int row = this->currentRow();
    if (row == -1) return;

    QVariantMap metadata;
    metadata["index"] = row;
    metadata["uid"] = item(row, 0)->data(Qt::UserRole).toString();
    metadata["path"] = item(row, 0)->data(Qt::UserRole + 1).toString();
    metadata["title"] = item(row, 0)->text();
    metadata["artist"] = item(row, 1)->text();
    metadata["album"] = item(row, 2)->text();
    metadata["duration"] = item(row, 3)->text();

    emit this->playSongRequested(metadata);
}

void Table::removeSong() {
    int row = this->currentRow();
    if (row == -1) return;

    QString uid = this->item(row, 0)->data(Qt::UserRole).toString();

    int current = this->findIndex(this->uid);
    QString previous, next;

    if (this->rowCount() > 1) {
        if (row < current) {
            previous = row == 0 ? this->findUID(this->rowCount() - 2) : this->findUID(row - 1);
        } else if (row > current) {
            next = row == this->rowCount() - 1 ? this->findUID(0) : this->findUID(row);
        } else {
            previous = row - 1 < 0 ? this->findUID(this->rowCount() - 2) : this->findUID(row - 1);
            next = row == this->rowCount() - 1 ? this->findUID(0) : this->findUID(row);
        }
    }

    this->removeRow(row);

    emit this->songRemoved(uid);
    emit this->songDeleted(uid);

    QVariantMap metadata;

    if (!previous.isEmpty()) {
        metadata = this->getMetadata(previous);
        emit this->previousSongRequested(metadata);
    }

    if (!next.isEmpty()) {
        metadata = this->getMetadata(next);
        emit this->nextSongRequested(metadata);
    }

    emit this->playlistSorted();
}

void Table::onPlaylistSelected(const QString& name) {
    QTableWidgetItem* item = this->itemAt(this->position);

    if (!item) {
        if (!name.isEmpty()) {
            QMenu menu(this);
            QAction* createAction = new QAction(tr("Add song to playlist"), &menu);

            this->connect(createAction, &QAction::triggered, this, [this, name]() {
                emit this->addSongRequested(name);
            });

            menu.addAction(createAction);
            menu.exec(viewport()->mapToGlobal(this->position));
        }
    } else {
        QMenu menu(this);
        menu.addAction("Play song", this, &Table::playSong);
        menu.addAction("Edit song", this, &Table::editSong);
        menu.addAction("Remove song", this, &Table::removeSong);
        menu.exec(viewport()->mapToGlobal(this->position));
    }
}

void Table::sendMetadata() {
    int row = this->currentRow();
    if (row == -1) return;

    QVariantMap metadata;
    metadata["index"] = row;
    metadata["uid"] = item(row, 0)->data(Qt::UserRole).toString();
    metadata["path"] = item(row, 0)->data(Qt::UserRole + 1).toString();
    metadata["title"] = item(row, 0)->text();
    metadata["artist"] = item(row, 1)->text();
    metadata["album"] = item(row, 2)->text();
    metadata["duration"] = item(row, 3)->text();

    emit this->playSongRequested(metadata);
    emit this->itemSelected(metadata);
}

void Table::showContextMenu(const QPoint& pos) {
    this->position = pos;
    emit this->requestPlaylist();
}
