#include "tool_constrain_parallel.hpp"
#include "document/document.hpp"
#include "document/entity/entity.hpp"
#include "document/entity/entity_arc2d.hpp"
#include "document/entity/entity_line2d.hpp"
#include "document/entity/entity_bezier2d.hpp"
#include "document/constraint/constraint_parallel.hpp"
#include "document/constraint/constraint_arc_arc_tangent.hpp"
#include "document/constraint/constraint_arc_line_tangent.hpp"
#include "document/constraint/constraint_bezier_line_tangent.hpp"
#include "document/constraint/constraint_bezier_bezier_tangent_symmetric.hpp"
#include "core/tool_id.hpp"
#include <optional>
#include "util/selection_util.hpp"
#include <iostream>
#include "tool_common_impl.hpp"

namespace dune3d {
static std::optional<std::pair<UUID, UUID>> two_entities_from_selection(const Document &doc,
                                                                        const std::set<SelectableRef> &sel)
{
    if (sel.size() != 2)
        return {};
    auto it = sel.begin();
    auto &sr1 = *it++;
    auto &sr2 = *it;

    if (sr1.type != SelectableRef::Type::ENTITY)
        return {};
    if (sr2.type != SelectableRef::Type::ENTITY)
        return {};

    auto &en1 = doc.get_entity(sr1.item);
    auto &en2 = doc.get_entity(sr2.item);
    const auto t1 = en1.get_type();
    const auto t2 = en2.get_type();
    if ((t1 == Entity::Type::LINE_3D && sr1.point == 0 && t2 == Entity::Type::WORKPLANE)
        || (t1 == Entity::Type::WORKPLANE && t2 == Entity::Type::LINE_3D && sr2.point == 0)
        || ((t1 == Entity::Type::LINE_2D || t1 == Entity::Type::LINE_3D)
            && (t2 == Entity::Type::LINE_2D || t2 == Entity::Type::LINE_3D) && (sr1.point == 0) && (sr2.point == 0)))
        return {{en1.m_uuid, en2.m_uuid}};

    return {};
}

static std::optional<std::pair<UUID, UUID>> two_arcs_from_selection(const Document &doc,
                                                                    const std::set<SelectableRef> &sel)
{
    if (sel.size() != 2)
        return {};
    auto it = sel.begin();
    auto &sr1 = *it++;
    auto &sr2 = *it;

    if (sr1.type != SelectableRef::Type::ENTITY)
        return {};
    if (sr2.type != SelectableRef::Type::ENTITY)
        return {};

    if (sr1.point != 0)
        return {};

    if (sr2.point != 0)
        return {};

    auto &en1 = doc.get_entity(sr1.item);
    auto &en2 = doc.get_entity(sr2.item);
    if (en1.of_type(Entity::Type::ARC_2D, Entity::Type::BEZIER_2D)
        && en2.of_type(Entity::Type::ARC_2D, Entity::Type::BEZIER_2D))
        return {{en1.m_uuid, en2.m_uuid}};

    return {};
}

struct ArcAndLine {
    UUID arc;
    UUID line;
};

static std::optional<ArcAndLine> arc_and_line_from_selection(const Document &doc, const std::set<SelectableRef> &sel)
{
    if (sel.size() != 2)
        return {};
    auto it = sel.begin();
    auto &sr1 = *it++;
    auto &sr2 = *it;

    if (sr1.type != SelectableRef::Type::ENTITY)
        return {};
    if (sr2.type != SelectableRef::Type::ENTITY)
        return {};

    if (sr1.point != 0)
        return {};

    if (sr2.point != 0)
        return {};

    auto &en1 = doc.get_entity(sr1.item);
    auto &en2 = doc.get_entity(sr2.item);
    if (en1.get_type() == Entity::Type::ARC_2D && en2.get_type() == Entity::Type::LINE_2D)
        return {{en1.m_uuid, en2.m_uuid}};
    else if (en1.get_type() == Entity::Type::LINE_2D && en2.get_type() == Entity::Type::ARC_2D)
        return {{en2.m_uuid, en1.m_uuid}};

    return {};
}

struct BezierAndLine {
    UUID bezier;
    UUID line;
};

static std::optional<BezierAndLine> bezier_and_line_from_selection(const Document &doc,
                                                                   const std::set<SelectableRef> &sel)
{
    if (sel.size() != 2)
        return {};
    auto it = sel.begin();
    auto &sr1 = *it++;
    auto &sr2 = *it;

    if (sr1.type != SelectableRef::Type::ENTITY)
        return {};
    if (sr2.type != SelectableRef::Type::ENTITY)
        return {};

    if (sr1.point != 0)
        return {};

    if (sr2.point != 0)
        return {};

    auto &en1 = doc.get_entity(sr1.item);
    auto &en2 = doc.get_entity(sr2.item);
    if (en1.get_type() == Entity::Type::BEZIER_2D && en2.get_type() == Entity::Type::LINE_2D)
        return {{en1.m_uuid, en2.m_uuid}};
    else if (en1.get_type() == Entity::Type::LINE_2D && en2.get_type() == Entity::Type::BEZIER_2D)
        return {{en2.m_uuid, en1.m_uuid}};

    return {};
}

ToolBase::CanBegin ToolConstrainParallel::can_begin()
{
    if (m_tool_id == ToolID::CONSTRAIN_BEZIER_BEZIER_TANGENT_SYMMETRIC) {
        auto bezs = two_arcs_from_selection(get_doc(), m_selection);
        if (!bezs.has_value())
            return false;
        return get_entity(bezs->first).of_type(Entity::Type::BEZIER_2D)
               && get_entity(bezs->second).of_type(Entity::Type::BEZIER_2D);
    }
    return two_entities_from_selection(get_doc(), m_selection).has_value()
           || two_arcs_from_selection(get_doc(), m_selection).has_value()
           || arc_and_line_from_selection(get_doc(), m_selection).has_value()
           || bezier_and_line_from_selection(get_doc(), m_selection).has_value();
}

ToolResponse ToolConstrainParallel::begin(const ToolArgs &args)
{
    if (auto tp = two_entities_from_selection(get_doc(), m_selection)) {
        auto &constraint = add_constraint<ConstraintParallel>();
        constraint.m_entity1 = tp->first;
        constraint.m_entity2 = tp->second;
        constraint.m_wrkpl = get_workplane_uuid();
        reset_selection_after_constrain();
        return ToolResponse::commit();
    }
    if (auto tp = arc_and_line_from_selection(get_doc(), m_selection)) {
        auto &arc = get_entity<EntityArc2D>(tp->arc);
        auto &line = get_entity<EntityLine2D>(tp->line);
        if (arc.m_wrkpl != line.m_wrkpl)
            return ToolResponse::end();
        for (const unsigned int arc_pt : {1, 2}) {
            for (const unsigned int line_pt : {1, 2}) {
                auto ap = arc.get_point(arc_pt, get_doc());
                auto lp = line.get_point(line_pt, get_doc());
                std::cout << arc_pt << " " << line_pt << " " << glm::length(ap - lp) << std::endl;
                if (glm::length(ap - lp) < 1e-6) {
                    auto &constraint = add_constraint<ConstraintArcLineTangent>();

                    constraint.m_arc = {tp->arc, arc_pt};
                    constraint.m_line = tp->line;
                    reset_selection_after_constrain();
                    return ToolResponse::commit();
                }
            }
        }
    }
    if (auto tp = bezier_and_line_from_selection(get_doc(), m_selection)) {
        auto &bez = get_entity<EntityBezier2D>(tp->bezier);
        auto &line = get_entity<EntityLine2D>(tp->line);
        if (bez.m_wrkpl != line.m_wrkpl)
            return ToolResponse::end();
        for (const unsigned int bez_pt : {1, 2}) {
            for (const unsigned int line_pt : {1, 2}) {
                auto ap = bez.get_point(bez_pt, get_doc());
                auto lp = line.get_point(line_pt, get_doc());
                if (glm::length(ap - lp) < 1e-6) {
                    auto &constraint = add_constraint<ConstraintBezierLineTangent>();

                    constraint.m_bezier = {tp->bezier, bez_pt};
                    constraint.m_line = tp->line;
                    reset_selection_after_constrain();
                    return ToolResponse::commit();
                }
            }
        }
    }
    if (auto tp = two_arcs_from_selection(get_doc(), m_selection)) {
        auto &arc1 = get_entity(tp->first);
        auto &arc2 = get_entity(tp->second);
        if (dynamic_cast<const IEntityInWorkplane &>(arc1).get_workplane()
            != dynamic_cast<const IEntityInWorkplane &>(arc2).get_workplane())
            return ToolResponse::end();
        for (const unsigned int arc1_pt : {1, 2}) {
            for (const unsigned int arc2_pt : {1, 2}) {
                auto ap1 = arc1.get_point(arc1_pt, get_doc());
                auto ap2 = arc2.get_point(arc2_pt, get_doc());
                if (glm::length(ap1 - ap2) < 1e-6) {
                    ConstraintArcArcTangent &constraint =
                            m_tool_id == ToolID::CONSTRAIN_BEZIER_BEZIER_TANGENT_SYMMETRIC
                                    ? add_constraint<ConstraintBezierBezierTangentSymmetric>()
                                    : add_constraint<ConstraintArcArcTangent>();

                    constraint.m_arc1 = {arc1.m_uuid, arc1_pt};
                    constraint.m_arc2 = {arc2.m_uuid, arc2_pt};
                    reset_selection_after_constrain();
                    return ToolResponse::commit();
                }
            }
        }
    }

    return ToolResponse::end();
}

ToolResponse ToolConstrainParallel::update(const ToolArgs &args)
{
    return ToolResponse();
}
} // namespace dune3d
