#ifndef THEME_H
#define THEME_H

#include <QApplication>
#include <QColor>
#include <QFile>
#include <QMap>
#include <QString>
#include <QTextStream>


struct Theme {
    explicit Theme(const QMap<QString, QColor>&);

    void applyStylesheet(const QMap<QString, QColor>&);

    QColor base, surface, overlay, text;
    QColor brightText, link, highlight, highlightedText;
    QColor disabledText, disabledButtonText, disabledWindowText, disabledHighlight;
};

#endif // THEME_H
