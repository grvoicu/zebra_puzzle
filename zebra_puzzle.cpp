#include "stdafx.h"
#include <map>
#include "constraint_solver/constraint_solver.h"

template <typename M, typename V>
void MapToVec(const  M & m, V & v) {
	for (typename M::const_iterator it = m.begin(); it != m.end(); ++it) {
		v.push_back(it->second);
	}
}

namespace operations_research {
	IntVar* findVal(std::vector<IntVar*> vars, int64 val) {
		for each (IntVar* var in vars) {
			if (var->Value() == val)
				return var;
		}
		return NULL;
	}

	void ZebraPuzzle() {
		Solver s("Zebra Puzzle");

		const enum characteristics { CST_START=0, colours=0, nations, pets, cigars, drinks, CST_END};

		std::map<characteristics const, std::vector<std::string> const> names = {
			{ colours, { "red", "blue", "green", "white", "yellow" } },
			{ nations, { "SE", "UK", "NO", "DK", "DE" } },
			{ pets,    { "dog", "bird", "cat", "horse", "fish" } },
			{ cigars,  { "Pall Mall", "Dunhill", "Blue Master", "Prince", "Blend" } },
			{ drinks,  { "milk", "water", "chocolate", "cofee", "tea" } }
		};
		std::vector<std::map<std::string const, IntVar*>> dict(CST_END);
		std::vector<std::vector<IntVar*>> int_vars(CST_END);
		const int64 n = names[CST_START].size();

		/* Decision variables values represent the house number */
		for (int characteristic = CST_START; characteristic < CST_END; characteristic++) {
			
			std::map<std::string const, IntVar*> *characteristic_dict = &dict[characteristic];
			
			for (int64 i = 0; i < n; i++)
			{
				std::string name = (names[(characteristics)characteristic])[i];
			
				// Decision variables
				characteristic_dict->emplace(name, s.MakeIntVar(1, n, name));
			}

			// Model constraint: Characteristics to house mapping is a one-to-one relation
			MapToVec(*characteristic_dict, int_vars[characteristic]);
			s.AddConstraint(s.MakeAllDifferent(int_vars[characteristic]));
		}
		LOG(INFO) << "Decision variables created" << std::endl;
		
		// Constraints

		// 1. The Englishman lives in the red house.
		s.AddConstraint(s.MakeEquality(dict[nations]["UK"], dict[colours]["red"]));
		// 2. The Swede keeps dogs.
		s.AddConstraint(s.MakeEquality(dict[nations]["SE"], dict[pets]["dog"]));
		// 3. The Dane drinks tea.
		s.AddConstraint(s.MakeEquality(dict[nations]["DK"], dict[drinks]["tea"]));
		// 4. The green house is just to the left of the white one.
		s.AddConstraint(s.MakeEquality(
			s.MakeDifference(dict[colours]["green"], dict[colours]["white"]), -1));
		// 5. The owner of the green house drinks coffee.
		s.AddConstraint(s.MakeEquality(dict[colours]["green"], dict[drinks]["cofee"]));
		// 6. The Pall Mall smoker keeps birds.
		s.AddConstraint(s.MakeEquality(dict[cigars]["Pall Mall"], dict[pets]["bird"]));
		// 7. The owner of the yellow house smokes Dunhills.
		s.AddConstraint(s.MakeEquality(dict[colours]["yellow"], dict[cigars]["Dunhill"]));
		// 8. The man in the center house drinks milk.
		s.AddConstraint(s.MakeEquality(dict[drinks]["milk"], (int)ceil(n/2.0)));
		// 9. The Norwegian lives in the first house.
		s.AddConstraint(s.MakeEquality(dict[nations]["NO"], 1));
		// 10. The Blend smoker has a neighbor who keeps cats.
		s.AddConstraint(s.MakeEquality(
			s.MakeAbs(s.MakeDifference(dict[cigars]["Blend"], dict[pets]["cat"])), 1));
		// 11. The man who keeps horses lives next to the Dunhill smoker.
		s.AddConstraint(s.MakeEquality(
			s.MakeAbs(s.MakeDifference(dict[pets]["horse"], dict[cigars]["Dunhill"])), 1));
		// 12. The man who smokes Blue Masters drinks chocolate.
		s.AddConstraint(s.MakeEquality(dict[cigars]["Blue Master"], dict[drinks]["chocolate"]));
		// 13. The German smokes Prince.
		s.AddConstraint(s.MakeEquality(dict[nations]["DE"], dict[cigars]["Prince"]));
		// 14. The Norwegian lives next to the blue house.
		s.AddConstraint(s.MakeEquality(
			s.MakeAbs(s.MakeDifference(dict[nations]["NO"], dict[colours]["blue"])), 1));
		// 15. The Blend smoker has a neighbor who drinks water.
		s.AddConstraint(s.MakeEquality(
			s.MakeAbs(s.MakeDifference(dict[cigars]["Blend"], dict[drinks]["water"])), 1));

		LOG(INFO) << "Constraints added" << std::endl;

		DecisionBuilder* const db = s.MakeDefaultPhase(int_vars[colours]);
		s.NewSearch(db);

		while (s.NextSolution()) {
			LOG(INFO) << "Solution found:";
			//LOG(INFO) << "UK lives in house " << dict[nations]["UK"]->Value() << " red house is " << dict[colours]["red"]->Value() << std::endl;
			LOG(INFO) << "The fish belongs to " << findVal(int_vars[nations], dict[pets]["fish"]->Value())->name();
		}

	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	operations_research::ZebraPuzzle();
	return 0;
}

