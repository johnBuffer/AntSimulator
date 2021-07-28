#include "colony.hpp"
#include "world.hpp"


struct FightSystem
{
    void checkForFights(std::vector<Colony>& colonies, World& world)
    {
        for (Colony& c : colonies) {
            checkForFights(c, colonies, world);
        }
    }

    void checkForFights(Colony& colony, std::vector<Colony>& colonies, World& world)
	{
		for (Ant& a : colony.ants) {
            if (!a.isFighting()) {
                checkForFight(a, colonies, world);
            } else if (a.target) {
                if (!a.target->isFighting()) {
                    a.target->setTarget(colony.ants.getRef(a.id));
                }
            }
        }
	}

    void checkForFight(Ant& ant, std::vector<Colony>& colonies, World& world)
    {
        // Only soldiers can initiate fights
        if (ant.type == Ant::Type::Worker && ant.fight_mode == FightMode::NoFight) {
            return;
        }
        // Check for potential enemies
        WorldCell& current_cell = world.map.get(ant.position);
        if (current_cell.markers[ant.col_id].fighting) {
            return;
        }
        for (uint32_t i(max_colonies_count); i--;) {
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
