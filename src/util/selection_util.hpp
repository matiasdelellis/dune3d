#pragma once
#include "uuid.hpp"
#include <optional>
#include <set>
#include <array>
#include "document/entity/entity_and_point.hpp"

namespace dune3d {

class Document;
class SelectableRef;
enum class EntityType;

struct TwoPoints {
    EntityAndPoint point1;
    EntityAndPoint point2;

    std::set<EntityAndPoint> get_enps() const;
};

std::optional<TwoPoints> two_points_from_selection(const Document &doc, const std::set<SelectableRef> &sel);

std::optional<EntityAndPoint> entity_and_point_from_hover_selection(const Document &doc,
                                                                    const std::optional<SelectableRef> &sel);

struct LineAndPoint {
    UUID line;
    EntityAndPoint point;

    enum class AllowSameEntity { YES, NO };

    std::set<EntityAndPoint> get_enps() const;
};

std::optional<LineAndPoint>
line_and_point_from_selection(const Document &doc, const std::set<SelectableRef> &sel,
                              LineAndPoint::AllowSameEntity allow_same_entity = LineAndPoint::AllowSameEntity::NO);
std::optional<LineAndPoint>
circle_and_point_from_selection(const Document &doc, const std::set<SelectableRef> &sel,
                                LineAndPoint::AllowSameEntity allow_same_entity = LineAndPoint::AllowSameEntity::NO);
std::optional<EntityAndPoint> entity_and_point_from_selection(const Document &doc, const std::set<SelectableRef> &sel);
std::optional<EntityAndPoint> entity_and_point_from_selection(const Document &doc, const std::set<SelectableRef> &sel,
                                                              EntityType type);


struct LinesAndPoint {
    std::array<UUID, 2> lines;
    EntityAndPoint point;

    std::set<EntityAndPoint> get_enps() const;
};

std::optional<LinesAndPoint> lines_and_point_from_selection(const Document &doc, const std::set<SelectableRef> &sel);
std::optional<UUID> document_from_selection(const std::set<SelectableRef> &sel);

} // namespace dune3d
