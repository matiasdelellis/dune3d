#pragma once
#include "group.hpp"
#include "igroup_generate.hpp"
#include "igroup_solid_model.hpp"
#include "igroup_source_group.hpp"
#include <glm/glm.hpp>

namespace dune3d {

class Document;
class SolidModel;

class GroupLoft : public Group, public IGroupSolidModel, public IGroupSourceGroup {
public:
    explicit GroupLoft(const UUID &uu);
    explicit GroupLoft(const UUID &uu, const json &j);
    static constexpr Type s_type = Type::LOFT;
    Type get_type() const override
    {
        return s_type;
    }

    struct Source {
        UUID wrkpl;
        UUID group;
    };

    std::vector<Source> m_sources;

    std::set<UUID> get_source_groups() const override;

    bool m_ruled = false;

    std::shared_ptr<const SolidModel> m_solid_model;

    Operation m_operation = Operation::UNION;
    Operation get_operation() const override
    {
        return m_operation;
    }
    void set_operation(Operation op) override
    {
        m_operation = op;
    }

    const SolidModel *get_solid_model() const override;

    void update_solid_model(const Document &doc) override;

    std::list<GroupStatusMessage> m_loft_messages;
    std::list<GroupStatusMessage> get_messages() const override;

    json serialize() const override;
    std::unique_ptr<Group> clone() const override;

    std::set<UUID> get_referenced_entities(const Document &doc) const override;
    std::set<UUID> get_referenced_groups(const Document &doc) const override;

    std::set<UUID> get_required_entities(const Document &doc) const override;
    std::set<UUID> get_required_groups(const Document &doc) const override;
};

} // namespace dune3d
