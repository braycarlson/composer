#ifndef SONGDIALOG_H
#define SONGDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QVariantMap>


class SongDialog : public QDialog {
    Q_OBJECT

 public:
    explicit SongDialog(const QVariantMap&, QWidget* parent = nullptr);

    QVariantMap updateMetadata() const;

 private:
    QVariantMap metadata;
    QLineEdit* titleEdit;
    QLineEdit* artistEdit;
    QLineEdit* albumEdit;
};

#endif // SONGDIALOG_H
