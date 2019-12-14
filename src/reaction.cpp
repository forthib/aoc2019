#include "reaction.h"

#include "io.h"
#include <algorithm>
#include <boost/optional.hpp>
#include <fstream>

namespace reaction {

	Reaction readReaction(const std::vector<std::string>& line)
	{
		auto reaction = Reaction{};
		for (size_t i = 0, n = line.size() / 2; i < n; ++i) {
			const auto quantity = std::atoll(line[2 * i].c_str());
			const auto name     = line[2 * i + 1];
			const auto chemical = Chemical{ name, quantity };
			if (i == n - 1)
				reaction.rhs = chemical;
			else
				reaction.lhs.push_back(chemical);
		}
		return reaction;
	}

	Reactions readReactions(const std::string& fileName)
	{
		const auto lines = io::readLinesOfStrings(fileName, " ,=>");

		auto reactions = Reactions{};
		for (const auto& line : lines) {
			const auto reaction = readReaction(line);
			if (!reactions.emplace(reaction.rhs.name, reaction).second)
				throw std::exception{ "two reactions produce the same chemical" };
		}
		return reactions;
	}

	QuantitiesHistory initializeHistory(const Reactions& reactions, const std::string& lhsName, const std::string& rhsName)
	{
		auto history = QuantitiesHistory{};
		for (auto&& entry : reactions)
			history[entry.first] = {};
		history[lhsName] = {};
		history[rhsName] = {};
		return history;
	}

	std::string generateName(size_t index)
	{
		const auto c1 = static_cast<char>('A' + (index / 26));
		const auto c2 = static_cast<char>('A' + (index % 26));
		return std::string{ c1 } + std::string{ c2 };
	}

	Reactions simplified(const Reactions& reactions, const std::string& lhsName, const std::string& rhsName)
	{
		size_t cpt = 0;

		auto transformation     = std::unordered_map<std::string, std::string>{};
		transformation[lhsName] = lhsName;
		transformation[rhsName] = rhsName;
		for (auto&& entry : reactions) {
			if (transformation.count(entry.first) == 0)
				transformation[entry.first] = generateName(cpt++);
		}

		auto result = Reactions{};
		for (auto&& entry : reactions) {
			auto reaction = entry.second;
			for (auto& chemical : reaction.lhs)
				chemical.name = transformation.at(chemical.name);
			reaction.rhs.name                      = transformation.at(reaction.rhs.name);
			result[transformation.at(entry.first)] = reaction;
		}
		return result;
	}

	void add(QuantitiesHistory& history, const Quantities& quantities)
	{
		for (auto&& entry : history)
			entry.second.push_back(quantities.count(entry.first) == 0 ? 0 : quantities.at(entry.first));
	}

	void display(const Reactions& reactions, std::ostream& out)
	{
		auto names = std::vector<std::string>{};
		for (auto&& entry : reactions)
			names.push_back(entry.first);
		std::sort(names.begin(), names.end());

		for (const auto& name : names) {
			const auto& reaction = reactions.at(name);
			out << reaction.rhs.quantity << reaction.rhs.name << " <-";
			for (const auto& chemical : reaction.lhs)
				out << " " << chemical.quantity << chemical.name;
			out << "\n";
		}
		out << "\n";
	}

	void display(const Quantities& quantities, std::ostream& out)
	{
		auto names = std::vector<std::string>{};
		for (auto&& entry : quantities)
			names.push_back(entry.first);
		std::sort(names.begin(), names.end());

		for (const auto& name : names)
			out << name << "\t";
		out << "\n";
		for (const auto& name : names)
			out << quantities.at(name) << "\t";
		out << "\n";
		out << "\n";
	}

	void display(const QuantitiesHistory& history, std::ostream& out, const std::string& separator)
	{
		auto names = std::vector<std::string>{};
		for (auto&& entry : history)
			names.push_back(entry.first);
		std::sort(names.begin(), names.end());

		out << names[0];
		for (size_t iName = 1, n = names.size(); iName < n; ++iName)
			out << separator << names[iName];
		out << "\n";

		for (size_t iStep = 0, n = history.begin()->second.size(); iStep < n; ++iStep) {
			out << history.at(names[0])[iStep];
			for (size_t iName = 1, n = names.size(); iName < n; ++iName)
				out << separator << history.at(names[iName])[iStep];
			out << "\n";
		}

		out << "\n";
	}

	void displayCSV(const QuantitiesHistory& history, const std::string& filePath)
	{
		std::ofstream out{ filePath };
		display(history, out, ";");
	}

	bool canProduce(const Reactions& reactions, const Quantities& quantities, const std::string& name)
	{
		const auto& reaction = reactions.at(name);
		for (const auto& chemical : reactions.at(name).lhs)
			if (quantities.count(chemical.name) == 0 || chemical.quantity > quantities.at(chemical.name))
				return false;
		return true;
	}

	void makeReactions(const Reactions& reactions, Quantities& quantities, const std::string& lhsName, const std::string& rhsName)
	{
		const auto getNextName = [&]() -> boost::optional<std::string> {
			for (auto&& entries : quantities)
				if (entries.second > 0 && entries.first != lhsName)
					if (reactions.at(entries.first).rhs.quantity <= entries.second)
						return entries.first;
			for (auto&& entries : quantities)
				if (entries.second > 0 && entries.first != lhsName)
					if (!canProduce(reactions, quantities, entries.first))
						return entries.first;
			for (auto&& entries : quantities)
				if (entries.second > 0 && entries.first != lhsName)
					return entries.first;
			return boost::none;
		};

		while (const auto name = getNextName()) {
			const auto& reaction = reactions.at(*name);

			auto n    = quantities.at(*name) / reaction.rhs.quantity;
			auto left = quantities.at(*name) % reaction.rhs.quantity;

			if (n == 0) {
				n = 1;
				left -= reaction.rhs.quantity;
			}

			for (const auto& chemical : reaction.lhs)
				quantities[chemical.name] += n * chemical.quantity;
			quantities[*name] = left;
		}
	}

	int64_t countRequestedQuantity(const Reactions& reactions, int64_t rhsQuantity, const std::string& lhsName, const std::string& rhsName)
	{
		auto quantities     = Quantities{};
		quantities[rhsName] = rhsQuantity;
		makeReactions(reactions, quantities, lhsName, rhsName);
		return quantities[lhsName];
	}

	int64_t countProducedQuantity(const Reactions& reactions, int64_t lhsQuantity, const std::string& lhsName, const std::string& rhsName)
	{
		const auto nRequestedTo1 = countRequestedQuantity(reactions, 1, lhsName, rhsName);

		auto minN = lhsQuantity / nRequestedTo1;
		auto maxN = 2 * minN;

		while (countRequestedQuantity(reactions, maxN, lhsName, rhsName) <= lhsQuantity)
			maxN *= 2;

		while (maxN - minN > 1) {
			const auto n = (minN + maxN) / 2;
			if (countRequestedQuantity(reactions, n, lhsName, rhsName) > lhsQuantity)
				maxN = n;
			else
				minN = n;
		}

		return minN;
	}
}
