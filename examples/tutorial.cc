#include <sc2api/sc2_api.h>
#include <iostream>

class Bot : public sc2::Agent {
public:
	virtual void OnGameStart() final {
	    std::cout << "Hello, WORLD!" << std::endl;
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
    return 0;
}
