#include "include/control.h"


QIcon loadIcon(const QString& path, const QColor& color) {
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QIcon();
    }

    QString svg = file.readAll();
    file.close();

    int index = svg.indexOf('>') + 1;
    QString style = QString("<style type=\"text/css\">path{fill:%1;}</style>").arg(color.name());
    svg.insert(index, style);

    QSvgRenderer renderer(svg.toUtf8());
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    renderer.render(&painter);
    painter.end();

    return QIcon(pixmap);
}

QPixmap loadPixmap(const QString& path, const QColor& color, const QSize& size = QSize(24, 24)) {
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QPixmap();
    }

    QString svg = file.readAll();
    QString style = QString("<style type=\"text/css\">path{fill:%1;}</style>").arg(color.name());
    svg.insert(svg.indexOf(">") + 1, style);

    QSvgRenderer renderer(svg.toUtf8());
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    renderer.render(&painter);
    painter.end();

    return pixmap;
}

Control::Control(QWidget* parent) : QWidget(parent) {
    this->setAttribute(Qt::WA_StyledBackground, true);

    player = new QMediaPlayer(this);
    playlist = new QMediaPlaylist(player);
    player->setPlaylist(playlist);

    QColor color = this->palette().color(QPalette::ButtonText);

    QHBoxLayout* layout = new QHBoxLayout(this);

    QHBoxLayout* left = new QHBoxLayout();

    iconLabel = new QLabel(this);

    QSize labelSize(48, 48);
    iconLabel->setPixmap(loadPixmap("icon/photo.svg", color, labelSize));

    left->addWidget(iconLabel);

    QVBoxLayout* labelLayout = new QVBoxLayout();

    titleLabel = new QLabel("-");
    artistLabel = new QLabel("-");

    int width = 75;
    titleLabel->setFixedWidth(width);
    artistLabel->setFixedWidth(width);

    titleLabel->setWordWrap(false);
    artistLabel->setWordWrap(false);

    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    artistLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    labelLayout->addWidget(titleLabel);
    labelLayout->addWidget(artistLabel);
    labelLayout->setSpacing(0);

    QHBoxLayout* volumeLayout = new QHBoxLayout();

    left->addLayout(labelLayout);

    elapsedLabel = new QLabel("0:00", this);
    durationLabel = new QLabel("0:00", this);

    QHBoxLayout* control = new QHBoxLayout();

    shuffleButton = new QPushButton();
    shuffleButton->setIcon(loadIcon("icon/shuffle.svg", color));
    shuffleButton->setIconSize(QSize(24, 24));
    shuffleButton->setToolTip("Toggle shuffle");

    prevButton = new QPushButton();
    prevButton->setIcon(loadIcon("icon/skip_previous.svg", color));
    prevButton->setIconSize(QSize(24, 24));
    prevButton->setToolTip("Play previous song");

    playPauseButton = new QPushButton();
    playPauseButton->setIcon(loadIcon("icon/play.svg", color));
    playPauseButton->setIconSize(QSize(24, 24));
    playPauseButton->setToolTip("Play/Pause");

    nextButton = new QPushButton();
    nextButton->setIcon(loadIcon("icon/skip_next.svg", color));
    nextButton->setIconSize(QSize(24, 24));
    nextButton->setToolTip("Play next song");

    repeatButton = new QPushButton();
    repeatButton->setIcon(loadIcon("icon/repeat.svg", color));
    repeatButton->setIconSize(QSize(24, 24));
    repeatButton->setToolTip("Toggle repeat");

    volumeButton = new QPushButton();
    volumeButton->setFixedSize(24, 24);
    volumeButton->setStyleSheet("QPushButton { background-color: transparent; border: none; }");
    volumeButton->setFlat(true);
    volumeButton->setIcon(loadIcon("icon/volume_on.svg", color));
    volumeButton->setIconSize(QSize(20, 20));
    volumeButton->setToolTip("Mute/Unmute volume");

    control->addWidget(shuffleButton);
    control->addWidget(prevButton);
    control->addWidget(playPauseButton);
    control->addWidget(nextButton);
    control->addWidget(repeatButton);

    progressSlider = new Slider(Qt::Horizontal, this);
    progressSlider->setRange(0, 100);
    progressSlider->setValue(0);
    progressSlider->setMinimumWidth(250);

    QHBoxLayout* progressLayout = new QHBoxLayout();
    progressLayout->addWidget(elapsedLabel);
    progressLayout->addWidget(progressSlider, 1);
    progressLayout->addWidget(durationLabel);

    QVBoxLayout* center = new QVBoxLayout();
    center->addLayout(control);
    center->addLayout(progressLayout);

    QVBoxLayout* right = new QVBoxLayout();
    QSlider* volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setMaximumWidth(175);

    volumeLayout->addWidget(volumeButton);
    volumeLayout->addWidget(volumeSlider);

    visualizeButton = new QPushButton();
    visualizeButton->setIcon(loadIcon("icon/eq.svg", color));
    visualizeButton->setIconSize(QSize(24, 24));
    visualizeButton->setToolTip("Open visualizer");

    right->addWidget(visualizeButton);
    right->addLayout(volumeLayout);

    layout->addLayout(left);
    layout->addStretch();
    layout->addLayout(center);
    layout->addStretch();
    layout->addLayout(right);

    this->setLayout(layout);

    this->connect(playPauseButton, &QPushButton::clicked, this, &Control::onTogglePlayPause);
    this->connect(nextButton, &QPushButton::clicked, this, &Control::onPlayNext);
    this->connect(prevButton, &QPushButton::clicked, this, &Control::onPlayPrevious);
    this->connect(shuffleButton, &QPushButton::clicked, this, &Control::onToggleShuffle);
    this->connect(repeatButton, &QPushButton::clicked, this, &Control::onToggleRepeat);
    this->connect(visualizeButton, &QPushButton::clicked, this, &Control::onVisualizeClicked);
    this->connect(volumeButton, &QPushButton::clicked, this, &Control::onToggleMute);
    this->connect(volumeSlider, &QSlider::valueChanged, this, &Control::onSetVolume);
    this->connect(progressSlider, &QSlider::sliderMoved, this, &Control::onSeek);
    this->connect(playlist, &QMediaPlaylist::currentIndexChanged, this, &Control::onPlaylistIndexChanged);
    this->connect(player, &QMediaPlayer::durationChanged, this, &Control::onUpdateDuration);
    this->connect(player, &QMediaPlayer::positionChanged, this, &Control::onUpdateElapsedTime);
    this->connect(player, &QMediaPlayer::durationChanged, progressSlider, &QSlider::setMaximum);
    this->connect(player, &QMediaPlayer::positionChanged, progressSlider, &QSlider::setValue);
    this->connect(player, &QMediaPlayer::stateChanged, this, &Control::onUpdatePlayPauseButton);
    this->connect(player, &QMediaPlayer::stateChanged, this, &Control::onPlayerStateChanged);
    this->connect(progressSlider, &QSlider::sliderPressed, this, &Control::onSliderPressed);
    this->connect(progressSlider, &QSlider::sliderReleased, this, &Control::onSliderReleased);
    this->connect(progressSlider, &QSlider::sliderMoved, this, &Control::onSeek);
    this->connect(progressSlider, &Slider::seek, this, &Control::onDirectSeek);
}

void Control::queueAddSongToPlaylist(const QJsonObject& metadata) {
    Action action;
    action.type = ActionType::Add;
    action.metadata = metadata;
    this->queue.enqueue(action);
}

void Control::queueRemoveSongFromPlaylist(const QString& uid) {
    QString current = this->getUID();

    Action action;
    action.type = ActionType::Remove;
    action.uid = uid;
    this->queue.enqueue(action);

    if (uid == current) {
        this->player->stop();
        this->processQueue();

        this->resetLabel();
    }
}

void Control::processQueue() {
    if (!this->queue.isEmpty()) {
        while (!this->queue.isEmpty()) {
            const Action& action = this->queue.dequeue();

            switch (action.type) {
                case ActionType::Add:
                    this->onAddSongToPlaylist(action.metadata);
                    break;
                case ActionType::Remove:
                    this->onRemoveSongFromPlaylist(action.uid);
                    break;
            }
        }
    }
}

void Control::onAddSongToPlaylist(const QJsonObject& metadata) {
    QString uid = metadata.value("uid").toString();
    QString path = metadata.value("path").toString();

    this->playlist->addMedia(QUrl::fromLocalFile(path));

    int index = this->playlist->mediaCount() - 1;
    this->original[uid] = index;
}

void Control::onRemoveSongFromPlaylist(const QString& uid) {
    if (!this->original.contains(uid)) return;

    int index = this->original.value(uid);
    this->playlist->removeMedia(index);

    QMap<QString, int> update;

    for (auto it = this->original.cbegin(); it != this->original.cend(); ++it) {
        if (it.key() != uid) {
            update.insert(
                it.key(),
                it.value() > index ? it.value() - 1 : it.value()
            );
        }
    }

    this->original = update;
}

void Control::onStorePlaylist(const QJsonObject& playlist) {
    this->current = playlist;
}

void Control::onLoadPlaylist(const QJsonObject& playlist) {
    QString name = playlist["name"].toString();

    if (this->name != name) {
        this->name = name;

        this->playlist->clear();
        this->original.clear();

        int index = 0;

        QJsonArray songs = playlist["songs"].toArray();

        for (const QJsonValue& song : songs) {
            QJsonObject track = song.toObject();
            QString path = track["path"].toString();
            QString uid = track["uid"].toString();

            this->playlist->addMedia(QUrl::fromLocalFile(path));
            this->original[uid] = index;

            index++;
        }
    }
}

QString Control::getUID() {
    int index = this->playlist->currentIndex();

    for (auto it = this->original.begin(); it != this->original.end(); ++it) {
        if (it.value() == index) {
            QString uid = it.key();
            this->onGetCurrentSong(uid);

            return uid;
        }
    }

    QString uid = QString();
    this->onGetCurrentSong(uid);

    return uid;
}

void Control::onPlaylistSorted() {
    QString uid = this->getUID();
    emit this->songPlaying(uid);
}

void Control::onGetCurrentSong(const QString& uid) {
    this->uid = uid;
}

void Control::onGetNextSong(const QVariantMap& metadata) {
    this->next = metadata;
}

void Control::onGetPreviousSong(const QVariantMap& metadata) {
    this->previous = metadata;
}

void Control::onPlaySong(const QVariantMap& metadata) {
    this->processQueue();

    this->player->stop();
    this->onLoadPlaylist(this->current);

    QString uid = metadata.value("uid").toString();
    int index = this->original.value(uid, -1);

    this->playlist->setCurrentIndex(index);
    this->player->play();

    this->onUpdateSong(metadata);
}

void Control::onPlaylistIndexChanged(int) {
    QString uid = this->getUID();
    emit this->songChanged(uid);
}

void Control::onPlayNext() {
    this->processQueue();

    this->player->stop();

    if (this->isRepeat || this->playlist->mediaCount() == 1) {
        int index = this->original.value(this->uid);
        this->playlist->setCurrentIndex(index);

        this->player->setPosition(0);
    }

    if (this->isShuffle) {
        QList<QString> keys = this->original.keys();
        QString uid;

        do {
            int random = QRandomGenerator::global()->bounded(keys.size());
            uid = keys.at(random);
        } while (this->original.size() > 1 && uid == this->getUID());

        int index = this->original.value(uid);
        this->playlist->setCurrentIndex(index);
    } else if (!this->isRepeat) {
        uid = this->next.value("uid").toString();

        int index = this->original.value(uid);
        this->playlist->setCurrentIndex(index);

    }

    this->player->play();
    this->onUpdateSong(this->next);
    this->nextTrackRequested();
}

void Control::onPlayPrevious() {
    this->processQueue();

    this->player->stop();

    if (this->isRepeat || this->playlist->mediaCount() == 1) {
        int index = this->original.value(this->uid);
        this->playlist->setCurrentIndex(index);

        this->player->setPosition(0);
    }

    if (this->isShuffle) {
        QList<QString> keys = this->original.keys();
        QString uid;

        do {
            int random = QRandomGenerator::global()->bounded(keys.size());
            uid = keys.at(random);
        } while (this->original.size() > 1 && uid == this->getUID());

        int index = this->original.value(uid);
        this->playlist->setCurrentIndex(index);
    } else if (!this->isRepeat) {
        uid = this->previous.value("uid").toString();

        int index = this->original.value(uid);
        this->playlist->setCurrentIndex(index);
    }

    this->player->play();
    this->onUpdateSong(this->previous);
    this->previousTrackRequested();
}

void Control::onPlayerStateChanged(QMediaPlayer::State state) {
    if (state == QMediaPlayer::StoppedState || state == QMediaPlayer::PausedState) {
        this->processQueue();
    }

    if (state == QMediaPlayer::StoppedState) {
        emit this->songFinished();
    }
}

QMediaPlayer* Control::getPlayer() {
    return this->player;
}

void Control::onDirectSeek(int value) {
    qint64 position = static_cast<qint64>(
        (static_cast<double>(value) / progressSlider->maximum()) * player->duration()
    );

    this->player->setPosition(position);
}

void Control::onSliderPressed() {
    this->isPlaying = (player->state() == QMediaPlayer::PlayingState);

    if (this->isPlaying) {
        this->player->pause();
    }
}

void Control::onSliderReleased() {
    qint64 newPosition = static_cast<qint64>(
        (static_cast<double>(progressSlider->value()) / progressSlider->maximum()) * player->duration()
    );

    this->player->setPosition(newPosition);

    if (this->isPlaying) {
        this->player->play();
    }
}

void Control::onSeek(int seconds) {
    this->player->setPosition(seconds);
}

void Control::onSetVolume(int value) {
    this->player->setVolume(value);

    if (value > 0 && this->isMuted) {
        this->player->setMuted(false);
        this->isMuted = false;

        QColor color = this->palette().color(QPalette::ButtonText);
        this->volumeButton->setIcon(loadIcon("icon/volume_on.svg", color));
    }
}

void Control::onToggleMute() {
    this->isMuted = !this->isMuted;
    this->player->setMuted(this->isMuted);

    if (!this->isMuted) {
        QColor color = this->palette().color(QPalette::ButtonText);
        this->volumeButton->setIcon(loadIcon("icon/volume_on.svg", color));
    } else {
        QColor color = this->palette().color(QPalette::BrightText);
        this->volumeButton->setIcon(loadIcon("icon/volume_off.svg", color));
    }
}

void Control::onTogglePlayPause() {
    if (this->player->state() == QMediaPlayer::PlayingState) {
        this->player->pause();
    } else {
        this->player->play();
    }
}

void Control::onToggleShuffle() {
    this->isShuffle = !this->isShuffle;

    if (this->isShuffle) {
        this->playlist->setPlaybackMode(QMediaPlaylist::Random);

        QColor color = this->palette().color(QPalette::BrightText);
        this->shuffleButton->setIcon(loadIcon("icon/shuffle.svg", color));

        color = this->palette().color(QPalette::ButtonText);
        this->repeatButton->setIcon(loadIcon("icon/repeat.svg", color));
    } else {
        this->playlist->setPlaybackMode(QMediaPlaylist::Sequential);

        QColor color = this->palette().color(QPalette::ButtonText);
        this->shuffleButton->setIcon(loadIcon("icon/shuffle.svg", color));
    }

    this->isRepeat = false;
}

void Control::onToggleRepeat() {
    this->isRepeat = !this->isRepeat;

    if (this->isRepeat) {
        this->playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);

        QColor color = this->palette().color(QPalette::BrightText);
        this->repeatButton->setIcon(loadIcon("icon/repeat.svg", color));

        color = this->palette().color(QPalette::ButtonText);
        this->shuffleButton->setIcon(loadIcon("icon/shuffle.svg", color));
    } else {
        this->playlist->setPlaybackMode(QMediaPlaylist::Sequential);

        QColor color = this->palette().color(QPalette::ButtonText);
        this->repeatButton->setIcon(loadIcon("icon/repeat.svg", color));
    }

    this->isShuffle = false;
}

void Control::onUpdateDuration(qint64 duration) {
    int seconds = (duration / 1000) % 60;
    int minutes = (duration / (1000 * 60)) % 60;

    this->durationLabel->setText(
        QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
    );
}

void Control::onUpdateElapsedTime(qint64 position) {
    int seconds = (position / 1000) % 60;
    int minutes = (position / (1000 * 60)) % 60;

    this->elapsedLabel->setText(
        QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'))
    );
}

void Control::onUpdatePlayPauseButton(QMediaPlayer::State state) {
    if (state == QMediaPlayer::PlayingState) {
        QColor color = this->palette().color(QPalette::ButtonText);
        this->playPauseButton->setIcon(loadIcon("icon/pause.svg", color));
    } else {
        QColor color = this->palette().color(QPalette::ButtonText);
        playPauseButton->setIcon(loadIcon("icon/play.svg", color));
    }
}

void Control::resetLabel() {
    this->titleLabel->setText("-");
    this->artistLabel->setText("-");
}

void Control::onUpdateSong(const QVariantMap& metadata) {
    QString title = metadata.value("title").toString();
    QString artist = metadata.value("artist").toString();

    QFontMetrics metricsTitle(this->titleLabel->font());
    QString truncatedTitle = metricsTitle.elidedText(title, Qt::ElideRight, this->titleLabel->width());
    this->titleLabel->setText(truncatedTitle);

    QFontMetrics metricsArtist(this->artistLabel->font());
    QString truncatedArtist = metricsArtist.elidedText(artist, Qt::ElideRight, this->artistLabel->width());
    this->artistLabel->setText(truncatedArtist);
}

void Control::updateVisualizeIcon(bool state) {
    this->isVisualizing = state;

    if (state == 0) {
        QColor color = this->palette().color(QPalette::ButtonText);
        visualizeButton->setIcon(loadIcon("icon/eq.svg", color));
    } else {
        QColor color = this->palette().color(QPalette::BrightText);
        visualizeButton->setIcon(loadIcon("icon/eq.svg", color));
    }
}

void Control::onVisualizeClicked() {
    emit this->visualizeClicked();
}

void Control::updateTheme() {
    QColor off = this->palette().color(QPalette::ButtonText);
    QColor on = this->palette().color(QPalette::BrightText);

    if (player->state() == QMediaPlayer::PlayingState) {
        playPauseButton->setIcon(loadIcon("icon/pause.svg", off));
    } else {
        playPauseButton->setIcon(loadIcon("icon/play.svg", off));
    }

    if (isShuffle) {
        shuffleButton->setIcon(loadIcon("icon/shuffle.svg", on));
    } else {
        shuffleButton->setIcon(loadIcon("icon/shuffle.svg", off));
    }

    if (isRepeat) {
        repeatButton->setIcon(loadIcon("icon/repeat.svg", on));
    } else {
        repeatButton->setIcon(loadIcon("icon/repeat.svg", off));
    }

    if (isVisualizing) {
        visualizeButton->setIcon(loadIcon("icon/eq.svg", on));
    } else {
        visualizeButton->setIcon(loadIcon("icon/eq.svg", off));
    }

    if (isMuted) {
        volumeButton->setIcon(loadIcon("icon/volume_off.svg", on));
    } else {
        volumeButton->setIcon(loadIcon("icon/volume_on.svg", off));
    }

    QSize labelSize(48, 48);
    this->iconLabel->setPixmap(loadPixmap("icon/photo.svg", off, labelSize));

    this->prevButton->setIcon(loadIcon("icon/skip_previous.svg", off));
    this->nextButton->setIcon(loadIcon("icon/skip_next.svg", off));
}
