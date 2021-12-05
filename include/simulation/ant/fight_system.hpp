#include "simulation/colony/colony.hpp"
#include "common/index_vector.hpp"


struct FightSystem
{
    void checkForFights(civ::Vector<Colony>& colonies, World& world)
    {
        for (Colony& c : colonies) {
            checkForFights(c, colonies, world);
        }
    }

    void checkForFights(Colony& colony, civ::Vector<Colony>& colonies, World& world)
	{
		for (Ant& a : colony.ants) {
            // Check if the ant has an active fight request
            // from markers sampling
            if (a.fight_request.active) {
                const uint8_t col_id = a.fight_request.col_id;
                const uint16_t ant_id = a.fight_request.ant_id;
                a.setTarget(colonies[col_id].ants.getRef(ant_id));
            }
            // Check only for non already fighting ants
            else if (!a.isFighting()) {
                checkForFight(a, colonies, world);
            }
            // Check that the target is also in fight
            else if (a.target) {
                if (!a.target->isFighting()) {
                    a.target->setTarget(colony.ants.getRef(a.id));
                }
            }
        }
	}

    void checkForFight(Ant& ant, civ::Vector<Colony>& colonies, World& world)
    {
        // Only soldiers can initiate fights
        if (ant.type == Ant::Type::Worker && ant.fight_mode == FightMode::NoFight) {
            return;
        }
        // Check for potential enemies
        WorldCell& current_cell = world.map.get(ant.position);
        for (uint32_t i(Conf::MAX_COLONIES_COUNT); i--;) {
			if (i != ant.col_id) {
				int16_t ant_id = current_cell.markers[i].current_ant;
				if (ant_id > -1) {
                    // Set fight mode for this ant
                    civ::Ref<Ant> other = colonies[i].ants.getRef(ant_id);
                    ant.setTarget(other);
                    // And for the other one
                    other->setTarget(colonies[ant.col_id].ants.getRef(ant.id));
                    return;
				}
			}
		}
    }
};
