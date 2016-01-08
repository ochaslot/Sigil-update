/************************************************************************
**
**  Copyright (C) 2011  John Schember <john@nachtimwald.com>
**
**  This file is part of Sigil.
**
**  Sigil is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Sigil is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Sigil.  If not, see <http://www.gnu.org/licenses/>.
**
*************************************************************************/

#pragma once
#ifndef SPELLCHECK_H
#define SPELLCHECK_H

#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QStringList>

class Hunspell;
class QStringList;
class QTextCodec;

/**
 * Singleton.
 */
class SpellCheck
{
public:
    static SpellCheck *instance();
    ~SpellCheck();

    QStringList userDictionaries();
    QStringList dictionaries();
    QString currentDictionary() const;
    bool spell(const QString &word);
    QStringList suggest(const QString &word);
    void clearIgnoredWords();
    void ignoreWord(const QString &word);
    void ignoreWordInDictionary(const QString &word);

    QString getWordChars();

    void setDictionary(const QString &name, bool forceReplace = false);
    void reloadDictionary();

    void addToUserDictionary(const QString &word, QString dict_name = "");
    QStringList allUserDictionaryWords();
    QStringList userDictionaryWords(QString dict_name);

    /**
     * The location of the user dictionary directories.
     */
    static QString dictionaryDirectory();
    static QString userDictionaryDirectory();
    static QString currentUserDictionaryFile();
    static QString userDictionaryFile(QString dict_name);

    void loadDictionaryNames();

private:
    SpellCheck();

    Hunspell *m_hunspell;
    QTextCodec *m_codec;
    QString m_wordchars;
    QString m_dictionaryName;
    //
    QHash<QString, QString> m_dictionaries;
    QStringList m_ignoredWords;

    static SpellCheck *m_instance;
};

#endif // SPELLCHECK_H
