#ifndef GLSERVICE_FILESYSTEM_HPP
#define GLSERVICE_FILESYSTEM_HPP

// Qt5
#include <QString>

namespace glservice {

// Gets absolute path of directory where executable is placed
QString getAbsolutePathOfExecutableDirectory();

// Translates relative to executable path into absolute
QString getAbsolutePathRelativeToExecutable(const QString &path);

}  // namespace glservice

#endif
