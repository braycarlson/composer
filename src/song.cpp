#include "include/song.h"


SongDialog::SongDialog(const QVariantMap& metadata, QWidget* parent) : QDialog(parent), metadata(metadata) {
    this->setWindowTitle("Edit Song");

    titleEdit = new QLineEdit(metadata["title"].toString());
    artistEdit = new QLineEdit(metadata["artist"].toString());
    albumEdit = new QLineEdit(metadata["album"].toString());

    QFormLayout* layout = new QFormLayout(this);

    layout->addRow("Title:", titleEdit);
    layout->addRow("Artist:", artistEdit);
    layout->addRow("Album:", albumEdit);

    QDialogButtonBox* group = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(group);

    this->connect(group, &QDialogButtonBox::accepted, this, &SongDialog::accept);
    this->connect(group, &QDialogButtonBox::rejected, this, &SongDialog::reject);
}

QVariantMap SongDialog::updateMetadata() const {
    QVariantMap update;

    update["uid"] = metadata["uid"];
    update["path"] = metadata["path"];
    update["title"] = titleEdit->text();
    update["artist"] = artistEdit->text();
    update["album"] = albumEdit->text();
    update["duration"] = metadata["duration"];
    return update;
}
