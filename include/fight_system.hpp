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
            if (!a.fighting) {
                checkForFight(a, colonies, world);
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
                    ant.fighting = true;
                    civ::Ref<Ant> other = colonies[i].ants.getRef(ant_id);
                    ant.setTarget(other);
                    //std::cout << "Fight found between (" << int(ant.col_id) << ", " << ant.id << ") and (" << i << ", " << ant_id << ")" << std::endl;
                    // And for the other one
                    other->fighting = true;
                    other->setTarget(colonies[ant.col_id].ants.getRef(ant.id));
                    return;
				}
			}
		}
    }
};
