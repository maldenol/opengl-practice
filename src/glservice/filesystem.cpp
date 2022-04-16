// Header file
#include "./filesystem.hpp"

// Qt5
#include <QCoreApplication>
#include <QDir>

// Gets absolute path of directory where executable is placed
QString glservice::getAbsolutePathOfExecutableDirectory() {
  return QCoreApplication::applicationDirPath();
}

// Translates relative to executable path into absolute
QString glservice::getAbsolutePathRelativeToExecutable(const QString &path) {
  return glservice::getAbsolutePathOfExecutableDirectory() + QDir::separator() + path;
}
