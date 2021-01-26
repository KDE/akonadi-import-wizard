/*
   MorkParser.cpp - Mozilla Mork Format Parser/Reader
   SPDX-FileCopyrightText: 2007 <ScalingWeb.com>
   SPDX-FileContributor: Yuriy Soroka <ysoroka@scalingweb.com>
   SPDX-FileContributor: Anton Fedoruk <afedoruk@scalingweb.com>

   SPDX-License-Identifier: BSD-3-Clause
*/

#include "MorkParser.h"
#include <QFile>
#include <QIODevice>
//        =============================================================
//        MorkParser::MorkParser
#include "thunderbirdplugin_debug.h"

MorkParser::MorkParser(int DefaultScope)
{
    initVars();
    mDefaultScope = DefaultScope;
}

//        =============================================================
//        MorkParser::open

bool MorkParser::open(const QString &path)
{
    initVars();

    QFile MorkFile(path);

    // Open file
    if (!MorkFile.exists() || !MorkFile.open(QIODevice::ReadOnly)) {
        mError = FailedToOpen;
        return false;
    }

    // Check magic header
    QByteArray MagicHeader = MorkFile.readLine();

    if (!MagicHeader.contains(MorkMagicHeader)) {
        mError = UnsupportedVersion;
        return false;
    }

    mMorkData = MorkFile.readAll();
    MorkFile.close();

    // Parse mork
    return parse();
}

//        =============================================================
//        MorkParser::error

MorkErrors MorkParser::error() const
{
    return mError;
}

//        =============================================================
//        MorkParser::initVars

void MorkParser::initVars()
{
    mError = NoError;
    mMorkPos = 0;
    nowParsing_ = NPValues;
    mCurrentCells = nullptr;
    mNextAddValueId = 0x7fffffff;
}

//        =============================================================
//        MorkParser::parse

bool MorkParser::parse()
{
    bool Result = true;
    char cur = 0;

    // Run over mork chars and parse each term
    cur = nextChar();

    while (Result && cur) {
        if (!isWhiteSpace(cur)) {
            // Figure out what a term
            switch (cur) {
            case '<':
                // Dict
                Result = parseDict();
                break;
            case '/':
                // Comment
                Result = parseComment();
                break;
            case '{':
                Result = parseTable();
                // Table
                break;
            case '[':
                Result = parseRow(0, 0);
                // Row
                break;
            case '@':
                Result = parseGroup();
                // Group
                break;
            default:
                mError = DefectedFormat;
                Result = false;
                break;
            }
        }

        // Get next char
        cur = nextChar();
    }

    return Result;
}

//        =============================================================
//        MorkParser::isWhiteSpace

bool MorkParser::isWhiteSpace(char c) const
{
    switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
    case '\f':
        return true;
    default:
        return false;
    }
}

//        =============================================================
//        MorkParser::nextChar

char MorkParser::nextChar()
{
    char cur = 0;

    if (mMorkPos < mMorkData.length()) {
        cur = mMorkData[mMorkPos];
        mMorkPos++;
    }

    if (!cur) {
        cur = 0;
    }

    return cur;
}

//        =============================================================
//        MorkParser::parseDict

bool MorkParser::parseDict()
{
    char cur = nextChar();
    bool Result = true;
    nowParsing_ = NPValues;

    while (Result && cur != '>' && cur) {
        if (!isWhiteSpace(cur)) {
            switch (cur) {
            case '<':
                if (mMorkData.mid(mMorkPos - 1, strlen(MorkDictColumnMeta)) == MorkDictColumnMeta) {
                    nowParsing_ = NPColumns;
                    mMorkPos += strlen(MorkDictColumnMeta) - 1;
                }
                break;
            case '(':
                Result = parseCell();
                break;
            case '/':
                Result = parseComment();
                break;
            }
        }

        cur = nextChar();
    }

    return Result;
}

//        =============================================================
//        MorkParser::parseComment

bool MorkParser::parseComment()
{
    char cur = nextChar();
    if ('/' != cur) {
        return false;
    }

    while (cur != '\r' && cur != '\n' && cur) {
        cur = nextChar();
    }

    return true;
}

//        =============================================================
//        MorkParser::parseCell

bool MorkParser::parseCell()
{
    bool Result = true;
    bool bColumnOid = false;
    bool bValueOid = false;
    bool bColumn = true;
    int Corners = 0;

    // Column = Value
    QString Column;
    QString Text;

    char cur = nextChar();

    // Process cell start with column (bColumn == true)
    while (Result && cur != QLatin1Char(')') && cur) {
        switch (cur) {
        case '^':
            // Oids
            Corners++;
            if (Corners == 1) {
                bColumnOid = true;
            } else if (Corners == 2) {
                bColumn = false;
                bValueOid = true;
            } else {
                Text += QLatin1Char(cur);
            }

            break;
        case '=':
            // From column to value
            if (bColumn) {
                bColumn = false;
            } else {
                Text += QLatin1Char(cur);
            }
            break;
        case '\\': {
            // Get next two chars
            char NextChar = nextChar();
            if ('\r' != NextChar && '\n' != NextChar) {
                Text += QLatin1Char(NextChar);
            } else {
                nextChar();
            }
            break;
        }
        case '$': {
            // Get next two chars
            QString HexChar;
            HexChar += QLatin1Char(nextChar());
            HexChar += QLatin1Char(nextChar());
            Text += QLatin1Char((char)HexChar.toInt(nullptr, 16));
            break;
        }
        default:
            // Just a char
            if (bColumn) {
                Column += QLatin1Char(cur);
            } else {
                Text += QLatin1Char(cur);
            }
            break;
        }

        cur = nextChar();
    }
    Q_UNUSED(bColumnOid)
    // Apply column and text
    int ColumnId = Column.toInt(nullptr, 16);

    if (NPRows != nowParsing_) {
        // Dicts
        if (!Text.isEmpty()) {
            if (nowParsing_ == NPColumns) {
                mColumns[ColumnId] = Text;
                qCDebug(THUNDERBIRDPLUGIN_LOG) << " column :" << ColumnId << " Text " << Text;
            } else {
                mValues[ColumnId] = Text;
                qCDebug(THUNDERBIRDPLUGIN_LOG) << " ColumnId " << ColumnId << " Value : " << Text;
            }
        }
    } else {
        if (!Text.isEmpty()) {
            // Rows
            int ValueId = Text.toInt(nullptr, 16);

            if (bValueOid) {
                (*mCurrentCells)[ColumnId] = ValueId;
            } else {
                mNextAddValueId--;
                mValues[mNextAddValueId] = Text;
                (*mCurrentCells)[ColumnId] = mNextAddValueId;
            }
        }
    }
    return Result;
}

//        =============================================================
//        MorkParser::parseTable

bool MorkParser::parseTable()
{
    bool Result = true;
    QString TextId;
    int Id = 0, Scope = 0;

    char cur = nextChar();

    // Get id
    while (cur != QLatin1Char('{') && cur != QLatin1Char('[') && cur != QLatin1Char('}') && cur) {
        if (!isWhiteSpace(cur)) {
            TextId += QLatin1Char(cur);
        }

        cur = nextChar();
    }

    parseScopeId(TextId, Id, Scope);

    // Parse the table
    while (Result && cur != QLatin1Char('}') && cur) {
        if (!isWhiteSpace(cur)) {
            switch (cur) {
            case '{':
                Result = parseMeta('}');
                break;
            case '[':
                Result = parseRow(Id, Scope);
                break;
            case '-':
            case '+':
                break;
            default: {
                QString JustId;
                while (!isWhiteSpace(cur) && cur) {
                    JustId += QLatin1Char(cur);
                    cur = nextChar();

                    if (cur == QLatin1Char('}')) {
                        return Result;
                    }
                }

                int JustIdNum = 0, JustScopeNum = 0;
                parseScopeId(JustId, JustIdNum, JustScopeNum);

                setCurrentRow(Scope, Id, JustScopeNum, JustIdNum);
                break;
            }
            }
        }

        cur = nextChar();
    }

    return Result;
}

//        =============================================================
//        MorkParser::parseScopeId

void MorkParser::parseScopeId(const QString &textId, int &Id, int &Scope) const
{
    int Pos = 0;

    if ((Pos = textId.indexOf(QLatin1Char(':'))) >= 0) {
        QString tId = textId.mid(0, Pos);
        QString tSc = textId.mid(Pos + 1, textId.length() - Pos);

        if (tSc.length() > 1 && tSc[0] == QLatin1Char('^')) {
            // Delete '^'
            tSc.remove(0, 1);
        }

        Id = tId.toInt(nullptr, 16);
        Scope = tSc.toInt(nullptr, 16);
    } else {
        Id = textId.toInt(nullptr, 16);
    }
}

//        =============================================================
//        MorkParser::setCurrentRow

void MorkParser::setCurrentRow(int TableScope, int TableId, int RowScope, int RowId)
{
    if (!RowScope) {
        RowScope = mDefaultScope;
    }

    if (!TableScope) {
        TableScope = mDefaultScope;
    }

    mCurrentCells = &(mMork[abs(TableScope)][abs(TableId)][abs(RowScope)][abs(RowId)]);
}

//        =============================================================
//        MorkParser::parseRow

bool MorkParser::parseRow(int TableId, int TableScope)
{
    bool Result = true;
    QString TextId;
    int Id = 0, Scope = 0;
    nowParsing_ = NPRows;

    char cur = nextChar();

    // Get id
    while (cur != QLatin1Char('(') && cur != QLatin1Char(']') && cur != QLatin1Char('[') && cur) {
        if (!isWhiteSpace(cur)) {
            TextId += QLatin1Char(cur);
        }

        cur = nextChar();
    }

    parseScopeId(TextId, Id, Scope);
    setCurrentRow(TableScope, TableId, Scope, Id);

    // Parse the row
    while (Result && cur != ']' && cur) {
        if (!isWhiteSpace(cur)) {
            switch (cur) {
            case '(':
                Result = parseCell();
                break;
            case '[':
                Result = parseMeta(']');
                break;
            default:
                Result = false;
                break;
            }
        }

        cur = nextChar();
    }

    return Result;
}

//        =============================================================
//        MorkParser::parseGroup

bool MorkParser::parseGroup()
{
    return parseMeta('@');
}

//        =============================================================
//        MorkParser::parseMeta

bool MorkParser::parseMeta(char c)
{
    char cur = nextChar();

    while (cur != c && cur) {
        cur = nextChar();
    }

    return true;
}

//        =============================================================
//        MorkParser::getTables

MorkTableMap *MorkParser::getTables(int TableScope)
{
    TableScopeMap::iterator iter;
    iter = mMork.find(TableScope);

    if (iter == mMork.end()) {
        return nullptr;
    }

    return &iter.value();
}

//        =============================================================
//        MorkParser::getRows

MorkRowMap *MorkParser::getRows(int RowScope, RowScopeMap *table)
{
    RowScopeMap::iterator iter;
    iter = table->find(RowScope);

    if (iter == table->end()) {
        return nullptr;
    }

    return &iter.value();
}

//        =============================================================
//        MorkParser::getValue

QString MorkParser::getValue(int oid)
{
    MorkDict::ConstIterator foundIter = mValues.constFind(oid);

    if (foundIter == mValues.constEnd()) {
        return QString();
    }

    return *foundIter;
}

//        =============================================================
//        MorkParser::getColumn

QString MorkParser::getColumn(int oid)
{
    MorkDict::ConstIterator foundIter = mColumns.constFind(oid);

    if (foundIter == mColumns.constEnd()) {
        return QString();
    }

    return *foundIter;
}
