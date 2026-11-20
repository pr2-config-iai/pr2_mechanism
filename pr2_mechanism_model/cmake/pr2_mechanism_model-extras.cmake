# The exported target links PkgConfig::TINYXML; recreate it for downstream packages.
find_package(PkgConfig REQUIRED)
pkg_check_modules(TINYXML REQUIRED IMPORTED_TARGET tinyxml)
