#pragma once

#include <QString>

namespace CtqTool
{
    Q_DECL_EXPORT QString ExtractFilename(const QString &line);
    Q_DECL_EXPORT QString ExtractMultiLineText(QStringList lines);
    Q_DECL_EXPORT QString CommonPrefix(const QString& a, const QString& b);
    Q_DECL_EXPORT bool FileExists(const QString &filename);
    Q_DECL_EXPORT QString FormatJSON(const QString& json);
    Q_DECL_EXPORT QString FormatXML(const QString&);
    Q_DECL_EXPORT bool HasCommonPrefix(const QString& a, const QString& b);
    Q_DECL_EXPORT bool HasCodeExtension(const QString &filename);
}
