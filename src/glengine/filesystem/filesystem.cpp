// Header file
#include "./filesystem.hpp"

// Qt5
#include <QCoreApplication>
#include <QDir>

// Gets absolute path of directory where executable is placed
QString glengine::getAbsolutePathOfExecutableDirectory() {
  return QCoreApplication::applicationDirPath();
}

// Translates relative to executable path into absolute
QString glengine::getAbsolutePathRelativeToExecutable(const QString &path) {
  return glengine::getAbsolutePathOfExecutableDirectory() + QDir::separator() + path;
}
