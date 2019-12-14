#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace reaction {

	struct Chemical
	{
		std::string name;
		int64_t     quantity = 0;
	};

	struct Reaction
	{
		std::vector<Chemical> lhs;
		Chemical              rhs;
	};

	using Reactions         = std::unordered_map<std::string, Reaction>;
	using Quantities        = std::unordered_map<std::string, int64_t>;
	using QuantitiesHistory = std::unordered_map<std::string, std::vector<int64_t>>;

	Reaction readReaction(const std::vector<std::string>& line);

	Reactions readReactions(const std::string& fileName);

	Reactions simplified(const Reactions& reactions, const std::string& lhsName = "ORE", const std::string& rhsName = "FUEL");

	QuantitiesHistory initializeHistory(const Reactions& reactions, const std::string& lhsName = "ORE", const std::string& rhsName = "FUEL");

	void add(QuantitiesHistory& history, const Quantities& quantities);

	void display(const Reactions& reactions, std::ostream& out = std::cout);

	void display(const Quantities& quantities, std::ostream& out = std::cout);

	void display(const QuantitiesHistory& history, std::ostream& out = std::cout, const std::string& separator = "\t");

	void displayCSV(const QuantitiesHistory& history, const std::string& filePath);

	bool canProduce(const Reactions& reactions, const Quantities& quantities, const std::string& name);

	void makeReactions(const Reactions& reactions, Quantities& quantities, const std::string& lhsName = "ORE", const std::string& rhsName = "FUEL");

	int64_t countRequestedQuantity(
	    const Reactions& reactions, int64_t rhsQuantity = 1, const std::string& lhsName = "ORE", const std::string& rhsName = "FUEL");

	int64_t countProducedQuantity(
	    const Reactions& reactions, int64_t lhsQuantity = 1000000000000, const std::string& lhsName = "ORE", const std::string& rhsName = "FUEL");
}
