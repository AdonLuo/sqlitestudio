#include "common/utils.h"
#include "common/global.h"
#include <QTextCodec>
#include <QString>
#include <QSet>
#include <QVariant>
#include <QDateTime>

void initUtils()
{
    qRegisterMetaType<QList<int>>("QList<int>");
}

bool isXDigit(const QChar& c)
{
    return c.isDigit() || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

QChar charAt(const QString& str, int pos)
{
    if (pos < 0 || pos >= str.size())
        return QChar(0);

    return str[pos];
}

int rand(int min, int max)
{
    return qrand() % (max-min) + min;
}

QString randStr(int length, bool numChars, bool whiteSpaces)
{
    static const char* alphaNumChars = " abcdefghijklmnopqrstuvwxyz1234567890";
    int start = 1;
    int range = start + numChars ? 36 : 26;

    if (whiteSpaces)
    {
        start--;
        range++;
    }

    QString output = "";
    for (int i = 0; i < length; i++)
        output += alphaNumChars[rand(start, range)];

    return output;
}

QString randStr(int length, const QString& charCollection)
{
    int range = charCollection.size();
    QString output = "";
    for (int i = 0; i < length; i++)
        output += charCollection[rand(0, range)];

    return output;
}

QString randBinStr(int length)
{
    char* output = new char[length];
    for (int i =0; i < length; i++)
        output[i] = rand(0, 256);

    return QString::fromLatin1(output, length);
}

QString randStrNotIn(int length, const QSet<QString> set, bool numChars, bool whiteSpaces)
{
    if (length == 0)
        return "";

    QString outStr;
    do
    {
        outStr = randStr(length, numChars, whiteSpaces);
    }
    while (set.contains(outStr));

    return outStr;
}


Range::Range() :
    from(0), to(0)
{
}

Range::Range(qint64 from, qint64 to)
    :from(from), to(to)
{
    fromValid = true;
    toValid = true;
}

void Range::setFrom(qint64 from)
{
    this->from = from;
    fromValid = true;
}

void Range::setTo(qint64 to)
{
    this->to = to;
    toValid = true;
}

qint64 Range::getFrom() const
{
    return from;
}

qint64 Range::getTo() const
{
    return to;
}

bool Range::isValid() const
{
    return fromValid && toValid && from <= to;
}

bool Range::contains(qint64 position) const
{
    return position >= from && position <= to;
}

bool Range::overlaps(const Range& other) const
{
    return overlaps(other.from, other.to);
}

bool Range::overlaps(qint64 from, qint64 to) const
{
    return (this->from >= from && this->from <= to) || (this->to >= from && this->to <= to);
}

Range Range::common(const Range& other) const
{
    return common(other.from, other.to);
}

Range Range::common(qint64 from, qint64 to) const
{
    if (!isValid() || from > to)
        return Range();

    if (this->from >= from)
    {
        if (this->from > to)
            return Range();

        if (this->to < to)
            return Range(this->from, this->to);
        else
            return Range(this->from, to);
    }
    else
    {
        if (from > this->to)
            return Range();

        if (to < this->to)
            return Range(from, to);
        else
            return Range(from, this->to);
    }
}

qint64 Range::length() const
{
    return to - from + 1;
}

QString generateUniqueName(const QString &baseName, const QStringList &existingNames)
{
    QString name = baseName;
    int i = 0;
    while (existingNames.contains(name))
        name = baseName+QString::number(i++);

    return name;
}

bool isNumeric(const QVariant& value)
{
    bool ok;
    value.toLongLong(&ok);
    if (ok)
        return true;

    value.toDouble(&ok);
    return ok;
}

QString rStrip(const QString& str)
{
    if (str.isNull())
        return str;

    for (int n = str.size() - 1; n >= 0; n--)
    {
        if (!str.at(n).isSpace())
            return str.left(n + 1);
    }
    return "";
}

QStringList tokenizeArgs(const QString& str)
{
    QStringList results;
    QString token;
    bool quote = false;
    bool escape = false;
    QChar c;
    for (int i = 0; i < str.length(); i++)
    {
        c = str[i];
        if (escape)
        {
            token += c;
        }
        else if (c == '\\')
        {
            escape = true;
        }
        else if (quote)
        {
            if (c == '"')
            {
                results << token;
                token.clear();
                quote = false;
            }
            else
            {
                token += c;
            }
        }
        else if (c == '"')
        {
            if (token.length() > 0)
                token += c;
            else
                quote = true;
        }
        else if (c.isSpace())
        {
            if (token.length() > 0)
            {
                results << token;
                token.clear();
            }
        }
        else
        {
            token += c;
        }
    }

    if (token.length() > 0)
        results << token;

    return results;
}

QStringList prefixEach(const QString& prefix, const QStringList& list)
{
    QStringList result;
    foreach (const QString& item, list)
        result << (prefix + item);

    return result;
}

int indexOf(const QStringList& list, const QString& value, Qt::CaseSensitivity cs)
{
    return indexOf(list, value, 0, cs);
}

int indexOf(const QStringList& list, const QString& value, int from, Qt::CaseSensitivity cs)
{
    if (cs == Qt::CaseSensitive)
        return list.indexOf(value, from);

    int cnt = list.size();
    for (int i = from; i < cnt; i++)
        if (list[i].compare(value, cs) == 0)
            return i;

    return -1;
}

QString pad(const QString& str, int length, const QChar& fillChar)
{
    if (str.length() >= abs(length))
        return str;

    QString result = str;
    QString fill = QString(fillChar).repeated(abs(length) - str.length());
    if (length >= 0)
        return result.append(fill);
    else
        return result.prepend(fill);
}

QString center(const QString& str, int length, const QChar& fillChar)
{
    if (str.length() >= length)
        return str;

    QString result = str;
    QString fillLeft = QString(fillChar).repeated((length - str.length()) / 2);
    QString fillRight = fillLeft;
    if ((fillLeft.length() + fillRight.length() + str.length()) < length)
        fillLeft += fillChar;

    return result.prepend(fillLeft).append(fillRight);
}

QString longest(const QStringList& strList)
{
    int max = 0;
    QString result;
    foreach (const QString str, strList)
    {
        if (str.size() > max)
        {
            result = str;
            max = str.size();
        }
    }
    return result;
}

QString shortest(const QStringList& strList)
{
    int max = INT_MAX;
    QString result;
    foreach (const QString str, strList)
    {
        if (str.size() < max)
        {
            result = str;
            max = str.size();
        }
    }
    return result;
}

QString longestCommonPart(const QStringList& strList)
{
   if (strList.size() == 0)
       return QString::null;

   QString common;
   QString first = strList.first();
   for (int i = 0; i < first.length(); i++)
   {
       common += first[i];
       foreach (const QString& str, strList)
       {
           if (!str.startsWith(common))
               return common.left(i);
       }
   }
   return common;
}

QStringList applyMargin(const QString& str, int margin)
{
    QStringList lines;
    QString line;
    foreach (QString word, str.split(" "))
    {
        if (((line + word).length() + 1) > margin)
        {
            if (!line.isEmpty())
            {
                lines << line;
                line.clear();
            }

            while ((line + word).length() > margin)
            {
                line += word.left(margin);
                lines << line;
                word = word.mid(margin);
            }
        }

        if (!line.isEmpty())
            line += " ";

        line += word;

        if (line.endsWith("\n"))
        {
            lines << line.trimmed();
            line.clear();
        }
    }

    if (!line.isEmpty())
        lines << line;

    if (lines.size() == 0)
        lines << QString();

    return lines;
}

QDateTime toGregorian(double julianDateTime)
{
    int Z = (int)julianDateTime;
    double F = julianDateTime - Z;

    int A;
    if (Z < 2299161)
    {
        A = Z;
    }
    else
    {
        int alpha = (int)((Z - 1867216.25)/36524.25);
        A = Z + 1 + alpha - (int)(alpha / 4);
    }

    int B = A + 1524;
    int C = (int)((B - 122.1) / 365.25);
    int D = (int)(365.25 * C);
    int E = (int)((B-D) / 30.6001);
    int DD = B - D - (int)(30.6001 * E) + F;
    int MM = (E <= 13) ? E - 1 : E - 13;
    int YYYY = (MM <= 2) ? C - 4715 : C - 4716;

    int mmmBase = round(F * 86400000.0);
    int mmm = mmmBase % 1000;
    int ssBase = mmmBase / 1000;
    int ss = ssBase % 60;
    int mmBase = ssBase / 60;
    int mm = mmBase % 60;
    int hh = (mmBase / 60) + 12;
    if (hh >= 24)
    {
        hh -= 24;
        DD++;
    }

    QDateTime dateTime;
    dateTime.setDate(QDate(YYYY, MM, DD));
    dateTime.setTime(QTime(hh, mm, ss, mmm));
    return dateTime;
}

double toJulian(const QDateTime& gregDateTime)
{
    QDate date = gregDateTime.date();
    QTime time = gregDateTime.time();
    return toJulian(date.year(), date.month(), date.day(), time.hour(), time.minute(), time.second(), time.msec());
}

double toJulian(int year, int month, int day, int hour, int minute, int second, int msec)
{
    int a = (14 - month) / 12;
    int y = year + 4800 + a;
    int m = month + 12 * a - 3;

    // Julian Day
    int jnd = day + (153 * m + 2) / 5 + 365 * y + y / 4 - y / 100 + y / 400 - 32045;

    // Julian Day + Julian Time
    double jndt = jnd + (hour - 12.0) / 24.0 + minute / 1440.0 + second / 86400.0 + msec / 86400000.0;

    return jndt;
}

QString formatFileSize(quint64 size)
{
    quint64 bytes = size;
    quint64 kb = 0;
    quint64 mb = 0;
    quint64 gb = 0;

    QStringList words;
    if (bytes > (1024*1024*1024))
    {
        gb = bytes / (1024*1024*1024);
        bytes %= (1024*1024*1024);
        words << QString("%1GB").arg(gb);
    }

    if (bytes > (1024*1024))
    {
        mb = bytes / (1024*1024);
        bytes %= (1024*1024);
        words << QString("%1MB").arg(mb);
    }

    if (bytes > 1024)
    {
        kb = bytes / 1024;
        bytes %= 1024;
        words << QString("%1KB").arg(kb);
    }

    if (bytes > 0)
        words << QString("%1B").arg(bytes);

    return words.join(" ");
}

QString formatTimePeriod(int msecs)
{
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    QStringList words;
    if (msecs > (1000*60*60))
    {
        hours = msecs / (1000*60*60);
        msecs %= (1000*60*60);
        words << QString("%1h").arg(hours);
    }

    if (msecs > (1000*60))
    {
        minutes = msecs / (1000*60);
        msecs %= (1000*60);
        words << QString("%1m").arg(minutes);
    }

    if (msecs > (1000))
    {
        seconds = msecs / 1000;
        msecs %= 1000;
        words << QString("%1s").arg(seconds);
    }

    if (msecs > 0)
        words << QString("%1ms").arg(msecs);

    return words.join(" ");
}

QStringList common(const QStringList& list1, const QStringList& list2, Qt::CaseSensitivity cs)
{
    QStringList newList;
    for (const QString& str : list1)
    {
        if (list2.contains(str, cs))
            newList << str;
    }
    return newList;
}

QStringList textCodecNames()
{
    QList<QByteArray> codecs = QTextCodec::availableCodecs();
    QStringList names;
    QSet<QString> nameSet;
    for (const QByteArray& codec : codecs)
        nameSet << QString::fromLatin1(codec.constData());

    names = nameSet.toList();
    qSort(names);
    return names;
}

QTextCodec* codecForName(const QString& name)
{
    return QTextCodec::codecForName(name.toLatin1());
}

QTextCodec* defaultCodec()
{
    return QTextCodec::codecForLocale();
}

QString defaultCodecName()
{
    return QString::fromLatin1(QTextCodec::codecForLocale()->name());
}

QStringList splitByLines(const QString& str)
{
    return str.split(QRegExp("\r?\n"));
}

QString joinLines(const QStringList& lines)
{
#ifdef Q_OS_WIN
    static_char* newLine = "\r\n";
#else
    static_char* newLine = "\n";
#endif
    return lines.join(newLine);
}

int sum(const QList<int>& integers)
{
    int res = 0;
    for (int i : integers)
        res += i;

    return res;
}