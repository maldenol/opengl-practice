// Header file
#include "./filesystem.hpp"

// Qt5
#include <QCoreApplication>
#include <QDir>

// Gets absolute path of directory where executable is placed
std::string glengine::getAbsolutePathOfExecutableDirectory() {
  return QCoreApplication::applicationDirPath().toStdString();
}

// Translates relative to executable path into absolute
std::string glengine::getAbsolutePathRelativeToExecutable(const std::string &path) {
  return getAbsolutePathOfExecutableDirectory() + QDir::separator().toLatin1() + path;
}
