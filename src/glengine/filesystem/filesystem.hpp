#ifndef GLENGINE_FILESYSTEM_FILESYSTEM_HPP
#define GLENGINE_FILESYSTEM_FILESYSTEM_HPP

// Qt5
#include <QString>

namespace glengine {

// Gets absolute path of directory where executable is placed
QString getAbsolutePathOfExecutableDirectory();

// Translates relative to executable path into absolute
QString getAbsolutePathRelativeToExecutable(const QString &path);

}  // namespace glengine

#endif
