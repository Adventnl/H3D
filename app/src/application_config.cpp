#include "forge/app/application_config.hpp"

#include "forge/foundation/version.hpp"

namespace forge
{

void ApplicationConfig::apply_defaults()
{
    if (version.empty())
    {
        version = std::string(version_string());
    }
    if (application_name.empty())
    {
        application_name = "Forge3D";
    }
    if (organization_name.empty())
    {
        organization_name = "Forge3D";
    }
}

} // namespace forge
