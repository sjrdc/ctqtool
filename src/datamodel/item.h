#pragma once

#include <QString>

namespace CtqTool
{
    class Item
    {
    public:
        void SetWeight(unsigned short);
        unsigned short GetWeight() const;

        void SetText(QString);
        QString GetText() const;

        void SetNote(QString);
        QString GetNote() const;

    private:
        unsigned short weight = 0;
        QString text;
        QString note;
    };
}

