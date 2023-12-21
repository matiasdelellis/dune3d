#pragma once
#include "constraint.hpp"
#include "document/entity/entity_and_point.hpp"
#include "iconstraint_datum.hpp"
#include <glm/glm.hpp>

namespace dune3d {

class ConstraintPointLineDistance : public Constraint, public IConstraintDatum {
public:
    explicit ConstraintPointLineDistance(const UUID &uu);
    explicit ConstraintPointLineDistance(const UUID &uu, const json &j);
    json serialize() const override;

    static constexpr Type s_type = Type::POINT_LINE_DISTANCE;
    Type get_type() const override
    {
        return s_type;
    }

    EntityAndPoint m_point;
    UUID m_line;

    UUID m_wrkpl;

    double m_distance = 1;
    glm::dvec3 m_offset = {0, 0, 0};

    glm::dvec3 get_projected(const Document &doc) const;
    glm::dvec3 get_origin(const Document &doc) const;

    std::unique_ptr<Constraint> clone() const override;

    std::set<UUID> get_referenced_entities() const override;

    std::string get_datum_name() const override
    {
        return "Distance";
    }

    double get_datum() const override
    {
        return std::abs(m_distance);
    }

    void set_datum(double d) override
    {
        m_distance = d * (m_distance >= 0 ? 1 : -1);
    }

    DatumUnit get_datum_unit() const override
    {
        return DatumUnit::MM;
    }

    std::pair<double, double> get_datum_range() const override
    {
        if (m_wrkpl)
            return {-1e3, 1e3};
        else
            return {0, 1e3};
    }

    bool is_movable() const override
    {
        return true;
    }

    void accept(ConstraintVisitor &visitor) const override;
};


} // namespace dune3d
