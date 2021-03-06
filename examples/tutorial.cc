#include <sc2api/sc2_api.h>
#include <iostream>
#include "common/Astar_algorithm.h"

class Bot : public sc2::Agent {
public:
	virtual void OnGameStart() final {
	    std::cout << "Hello, WORLD!" << std::endl;
	}

    virtual void OnStep() final{
        //std::cout << "minerals: " << Observation()->GetMinerals()  << " | Vespene: " << Observation()->GetVespene() << std::endl;
    }

    virtual void OnUnitIdle(const sc2::Unit *unit) final{
        switch(unit->unit_type.ToType()){
            case sc2::UNIT_TYPEID::TERRAN_COMMANDCENTER:
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::TRAIN_SCV);
                break;
            case sc2::UNIT_TYPEID::TERRAN_SCV:                
                Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART, FindNearestMineral(unit->pos));
                break;
            default:
                break;
        }
    }

private:
    const sc2::Unit* FindNearestMineral(sc2::Point2D start){
        const sc2::Unit* nearest;
        sc2::Units total_neutral_units = Observation()->GetUnits(sc2::Unit::Alliance::Neutral);
        for(auto neutral_unit : total_neutral_units){
            if(neutral_unit->unit_type == sc2::UNIT_TYPEID::NEUTRAL_MINERALFIELD){
                nearest = neutral_unit;
            }
        }

        return nearest;    
    }
    
};

int main(int argc, char* argv[]) {
    sc2::Coordinator coordinator;
    coordinator.LoadSettings(argc,argv);

    Bot bot;
    coordinator.SetParticipants({
        CreateParticipant(sc2::Race::Terran, &bot),
        CreateComputer(sc2::Race::Zerg)
    });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE); 
    
    while(coordinator.Update()){
    }   

    return 0;
}


