#ifndef GLENGINE_FILESYSTEM_FILESYSTEM_HPP
#define GLENGINE_FILESYSTEM_FILESYSTEM_HPP

// STD
#include <string>

namespace glengine {

// Gets absolute path of directory where executable is placed
std::string getAbsolutePathOfExecutableDirectory();

// Translates relative to executable path into absolute
std::string getAbsolutePathRelativeToExecutable(const std::string &path);

}  // namespace glengine

#endif
