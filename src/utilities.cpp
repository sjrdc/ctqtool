/*
 * this file is part of CTQ tool - a tool to explore critical to quality trees
 * Copyright (C) 2021 Sjoerd Crijns
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "utilities.h"

#include <QFileInfo>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QString>
#include <qxmlstream.h>

namespace CtqTool
{
    QString ExtractFilename(const QString& line)
    {
        static const QRegularExpression regexp("[0-9a-z.A-Z_:\\\\]*\\.[0-9a-zA-Z_]{1,}");
        QRegularExpressionMatch match = regexp.match(line);
        if (match.hasMatch())
        {
            return match.captured(0);
        }
        else return QString("");
    }

    bool HasCodeExtension(const QString& filename)
    {
        static QString codeExtensions[] = {
          ".cpp",
          ".c",
          ".h",
          ".i",
          ".res"
        };

        for (auto extension : codeExtensions)
            if (filename.endsWith(extension))
                return true;
        return false;
    }

    bool FileExists(const QString& filename)
    {
        QFileInfo file(filename);
        return (file.exists() && file.isFile());
    }

    bool HasCommonPrefix(const QString& a, const QString& b)
    {
        // a common prefix could be as short as a single character
        return (!a.isEmpty() && !b.isEmpty() && a[0] == b[0]);
    }

    QString CommonPrefix(const QString& a, const QString& b)
    {
        QString prefix;
        bool found = false;
        for (auto i = 0; i < a.length() && !found; ++i)
        {
            if (a[i] == b[i]) prefix.append(a[i]);
            else found = true;
        }
        return prefix;
    }

    QString ExtractMultiLineText(QStringList lines)
    {
        if (lines.size() >= 2)
        {
            auto prefix = CommonPrefix(lines[0], lines[1]);
            for (auto& line : lines)
            {
                line = QString(line.mid(prefix.size(), -1));
            }
        }
        QString text = lines.join(" ");
        text.replace("> >", "");

        return text;
    }

    QString FormatJSON(const QString& s)
    {
        QJsonDocument doc = QJsonDocument::fromJson(s.mid(s.indexOf('{') - 1).toUtf8());
        QString formatted = doc.toJson(QJsonDocument::Indented);
        return formatted;
    }

    QString FormatXML(const QString& s)
    {
        QXmlStreamReader reader(s);
        QString xmlOut;
        QXmlStreamWriter writer(&xmlOut);
        writer.setAutoFormatting(true);

        while (!reader.atEnd()) {
            reader.readNext();
            if (!reader.isWhitespace()) {
                writer.writeCurrentToken(reader);
            }
        }

        return xmlOut;
    }
}
