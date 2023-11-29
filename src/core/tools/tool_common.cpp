#include "tool_common.hpp"
#include "document/entity/entity_workplane.hpp"
#include "tool_common_impl.hpp"
#include "document/group/group.hpp"

namespace dune3d {
Document &ToolCommon::get_doc()
{
    return m_core.get_current_document();
}

Group &ToolCommon::get_group()
{
    return get_doc().get_group(m_core.get_current_group());
}

UUID ToolCommon::get_workplane_uuid()
{
    return m_core.get_current_workplane();
}

EntityWorkplane *ToolCommon::get_workplane()
{
    auto uu = get_workplane_uuid();
    if (uu)
        return &get_entity<EntityWorkplane>(uu);
    else
        return nullptr;
}

void ToolCommon::set_current_group_solve_pending()
{
    get_doc().set_group_solve_pending(m_core.get_current_group());
}

void ToolCommon::set_current_group_generate_pending()
{
    get_doc().set_group_generate_pending(m_core.get_current_group());
}

void ToolCommon::set_current_group_update_solid_model_pending()
{
    get_doc().set_group_update_solid_model_pending(m_core.get_current_group());
}

} // namespace dune3d
