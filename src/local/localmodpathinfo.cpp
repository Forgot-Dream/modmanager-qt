#include "localmodpathinfo.h"

#include <QDir>
#include <QDebug>

#include "util/tutil.hpp"

LocalModPathInfo::LocalModPathInfo(const QString &dir, const GameVersion &version, ModLoaderType::Type type) :
    path_(dir),
    gameVersion_(version),
    loaderType_(type)
{
    name_ = showText();
}

LocalModPathInfo::LocalModPathInfo(const QString &name, const QString &dir, const GameVersion &version, ModLoaderType::Type type) :
    name_(name),
    path_(dir),
    gameVersion_(version),
    loaderType_(type)
{

}

bool LocalModPathInfo::operator==(const LocalModPathInfo &other) const
{
    //same path, game version, and loader type
    //no name
    if(path_ == other.path_ && gameVersion_ == other.gameVersion_ && loaderType_ == other.loaderType_)
        return true;
    else
        return false;
}

bool LocalModPathInfo::operator!=(const LocalModPathInfo &other) const
{
    return !operator==(other);
}

LocalModPathInfo LocalModPathInfo::fromVariant(const QVariant &variant)
{
    LocalModPathInfo info;
    info.name_ = value(variant, "name").toString();
    info.path_ = value(variant, "dir").toString();
    info.gameVersion_ = value(variant, "gameVersion").toString();
    info.loaderType_ = ModLoaderType::fromString(value(variant, "loaderType").toString());

    return info;
}

QVariant LocalModPathInfo::toVariant() const
{
    QMap<QString, QVariant> map;
    map["name"] = name_;
    map["dir"] = path_;
    map["gameVersion"] = QString(gameVersion_);
    map["loaderType"] = ModLoaderType::toString(loaderType_);

    return QVariant::fromValue(map);
}

QString LocalModPathInfo::showText() const
{
    return isAutoName()? autoName() : name_;
}

bool LocalModPathInfo::exists() const
{
    return QDir(path_).exists();
}

const GameVersion &LocalModPathInfo::gameVersion() const
{
    return gameVersion_;
}

ModLoaderType::Type LocalModPathInfo::loaderType() const
{
    return loaderType_;
}

const QString &LocalModPathInfo::name() const
{
    return name_;
}

void LocalModPathInfo::setName(const QString &newName)
{
    name_ = newName;
}

bool LocalModPathInfo::isAutoName() const
{
    return name_ == autoName();
}

QString LocalModPathInfo::autoName() const
{
    return gameVersion_ + " - " + ModLoaderType::toString(loaderType_);
}

const QString &LocalModPathInfo::path() const
{
    return path_;
}

void LocalModPathInfo::setPath(const QString &newPath)
{
    path_ = newPath;
}

void LocalModPathInfo::setGameVersion(const GameVersion &newGameVersion)
{
    gameVersion_ = newGameVersion;
}

void LocalModPathInfo::setLoaderType(ModLoaderType::Type newLoaderType)
{
    loaderType_ = newLoaderType;
}