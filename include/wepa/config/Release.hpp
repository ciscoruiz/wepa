#ifndef WEPA_CONFIG_RELEASE_H
#define WEPA_CONFIG_RELEASE_H

#include <string>

namespace wepa  {

namespace config {

class Release {
public:
   /**
    * @brief getVersion
    * @return The version of this compilation
    */
   static std::string getVersion () throw ();

   /**
    * @brief getArchitecture
    * @return The literal which contains information about this release.
    */
   static std::string getArchitecture () throw ();
};

}
}

#endif // WEPA_CONFIG_RELEASE_H