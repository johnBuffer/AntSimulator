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
        WorldCell& current_cell = world.map.get(ant.position);
        for (uint32_t i(max_colonies_count); i--;) {
			if (i != ant.col_id) {
				uint16_t ant_id = current_cell.markers[i].current_ant;
				if (ant_id) {
                    // Set fight mode for this ant
                    ant.fight_mode = FightMode::Fighting;
                    civ::Ref<Ant> other = colonies[i].ants.getRef(ant_id);
                    ant.setTarget(other);
                    // And for the other one
                    ant.fight_mode = FightMode::Fighting;
                    other->setTarget(colonies[ant.col_id].ants.getRef(ant.id));
                    return;
				}
			}
		}
    }
};
