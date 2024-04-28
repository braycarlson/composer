#include "include/theme.h"


Theme::Theme(const QMap<QString, QColor>& mapping) {
    base = mapping.value("base", QColor("#000000"));
    surface = mapping.value("surface", QColor("#000000"));
    overlay = mapping.value("overlay", QColor("#000000"));
    text = mapping.value("text", QColor("#000000"));
    brightText = mapping.value("brightText", QColor("#000000"));
    link = mapping.value("link", QColor("#000000"));
    highlight = mapping.value("highlight", QColor("#000000"));
    highlightedText = mapping.value("highlightedText", QColor("#000000"));
    disabledText = mapping.value("disabledText", QColor("#000000"));
    disabledButtonText = mapping.value("disabledButtonText", QColor("#000000"));
    disabledWindowText = mapping.value("disabledWindowText", QColor("#000000"));
    disabledHighlight = mapping.value("disabledHighlight", QColor("#000000"));
}

void Theme::applyStylesheet(const QMap<QString, QColor>& palette) {
    QString path = "src/stylesheet.qss";

    QFile file(path);
    QString base;

    if (file.open(QFile::ReadOnly)) {
        base = QTextStream(&file).readAll();
    }

    QString tooltip = QString(
        "QToolTip {"
        " background-color: %1;"
        " color: %2;"
        " border: 1px solid %3;"
        " padding: 4px;"
        " border-radius: 2px;"
        " opacity: 200;"
        "}"
    ).arg(
        palette["overlay"].name(),
        palette["text"].name(),
        palette["highlight"].name()
    );

    qApp->setStyleSheet(base + tooltip);
}
