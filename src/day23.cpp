#include "io.h"
#include "opcode.h"
#include "test.h"
#include <boost/optional/optional.hpp>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_vector.h>
#include <tbb/task_group.h>
#include <tbb/task_scheduler_init.h>
#include <thread>

namespace {

	class Logger
	{
	public:
		Logger(bool verbose = true) : verbose_{ verbose } {}

		void started(int64_t address)
		{
			if (verbose_) {
				auto s = std::string{};
				s += "[";
				s += std::to_string(address);
				s += "] ";
				s += "Started";
				queue_.push(s);
			}
		}

		void finished(int64_t address)
		{
			if (verbose_) {
				auto s = std::string{};
				s += "[";
				s += std::to_string(address);
				s += "] ";
				s += "Finished";
				queue_.push(s);
			}
		}

		void initialized(int64_t address)
		{
			if (verbose_) {
				auto s = std::string{};
				s += "[";
				s += std::to_string(address);
				s += "] ";
				s += "Initialized";
				queue_.push(s);
			}
		}

		void received(int64_t address, int64_t x, int64_t y)
		{
			if (verbose_) {
				auto s = std::string{};
				s += "[";
				s += std::to_string(address);
				s += "] ";
				s += "Received (";
				s += std::to_string(x);
				s += ",";
				s += std::to_string(y);
				s += ")";
				queue_.push(s);
			}
		}

		void sent(int64_t address, int64_t to, int64_t x, int64_t y)
		{
			if (verbose_) {
				auto s = std::string{};
				s += "[";
				s += std::to_string(address);
				s += "] ";
				s += "Sent (";
				s += std::to_string(x);
				s += ",";
				s += std::to_string(y);
				s += ") to ";
				s += "[";
				s += std::to_string(to);
				s += "] ";
				queue_.push(s);
			}

			if (to == 255 && !part1_) {
				part1_ = y;
				auto s = std::string{};
				s += "Part 1: ";
				s += std::to_string(*part1_);
				queue_.push(s);
			}

			if (address == 255 && to == 0) {
				if (!part2a_ || *part2a_ != y)
					part2a_ = y;
				else if (!part2b_) {
					part2b_ = y;
					auto s = std::string{};
					s += "Part 2: ";
					s += std::to_string(*part2b_);
					queue_.push(s);
				
					finished_ = true;
					verbose_ = false;
				}
			}
		}

		void idle(size_t address, size_t idleAddress, bool idle, int64_t nIdles)
		{
			if (verbose_) {
				auto s = std::string{};
				s += "[";
				s += std::to_string(address);
				s += "] ";
				if (idle)
					s += "Idle [";
				else
					s += "Woke up [";
				s += std::to_string(idleAddress);
				s += "] - N idles = ";
				s += std::to_string(nIdles);
				queue_.push(s);
			}
		}

		void run()
		{	
			std::string s;
			while (!finished_ || !queue_.empty()) {
				if (queue_.try_pop(s))
					std::cout << s << "\n";
				else
					std::this_thread::yield();
			}

			std::cin.get();
			std::exit(0);
		}

	private:
		bool                               verbose_;
		tbb::concurrent_queue<std::string> queue_;

		boost::optional<int64_t> part1_, part2a_, part2b_;
		bool finished_ = false;
	};

	class NAT
	{
	public:
		using MessageFunction = std::function<void(int64_t, int64_t, int64_t)>;

		NAT() { idles_.resize(50, false); }

		void setMessageFunction(MessageFunction messageFunction) { messageFunction_ = std::move(messageFunction); }
		void setLogger(Logger& logger) { logger_ = &logger; }

		void putMessage(int64_t x, int64_t y) { lastPacket_ = std::make_pair(x, y); }

		void setIdle(int64_t address, bool idle)
		{
			if (idles_[address] != idle) {
				idles_[address]   = idle;
				const auto nIdles = std::count(idles_.begin(), idles_.end(), true);
				logger_->idle(255, address, idle, nIdles);
			}
		}

		void run()
		{
			for (;;) {
				const auto nIdles = std::count(idles_.begin(), idles_.end(), true);
				if (nIdles == 50 && lastPacket_) {
					logger_->sent(255, 0, lastPacket_->first, lastPacket_->second);
					messageFunction_(0, lastPacket_->first, lastPacket_->second);

					if (ySent && ySent == lastPacket_->second)
						std::cout << "Part 2: " << lastPacket_->second << "\n";

					lastPacket_ = boost::none;
				}
				else {
					std::this_thread::yield();
				}
			}
		}

	private:
		tbb::concurrent_vector<bool>                 idles_;
		boost::optional<std::pair<int64_t, int64_t>> lastPacket_;
		MessageFunction                              messageFunction_;
		Logger*                                      logger_ = nullptr;

		boost::optional<int64_t> ySent;
	};

	class Computer
	{
	public:
		using MessageFunction = std::function<void(int64_t, int64_t, int64_t)>;
		using IdleFunction    = std::function<void(int64_t, bool)>;

		Computer(int64_t address, std::vector<int64_t> code) : address_{ address }, code_{ std::move(code) } {};

		void setMessageFunction(MessageFunction messageFunction) { messageFunction_ = std::move(messageFunction); }
		void setIdleFunction(IdleFunction idleFunction) { idleFunction_ = std::move(idleFunction); }
		void setLogger(Logger& logger) { logger_ = &logger; }

		void putMessage(int64_t x, int64_t y) { queue_.push({ x, y }); }

		void run()
		{
			bool initialized = false;

			auto inputs = std::vector<int64_t>{};

			const auto inputFunction = [&]() {
				if (!initialized) {
					logger_->initialized(address_);
					initialized = true;
					return address_;
				}

				if (inputs.empty()) {
					auto value = std::pair<int64_t, int64_t>{};
					if (queue_.try_pop(value)) {
						idleFunction_(address_, false);
						logger_->received(address_, value.first, value.second);
						inputs.push_back(value.first);
						inputs.push_back(value.second);
					}
				}

				if (!inputs.empty()) {
					const auto input = inputs.front();
					inputs.erase(inputs.begin());
					return input;
				}

				idleFunction_(address_, true);
				std::this_thread::yield();
				return int64_t{ -1 };
			};

			auto outputs = std::vector<int64_t>{};

			const auto outputFunction = [&](int64_t value) {
				outputs.push_back(value);
				if (outputs.size() == 3) {
					messageFunction_(outputs[0], outputs[1], outputs[2]);
					logger_->sent(address_, outputs[0], outputs[1], outputs[2]);
					outputs.clear();
				}
			};

			logger_->started(address_);
			opcode::run(code_, inputFunction, outputFunction);
			logger_->finished(address_);
		}


	private:
		int64_t                                            address_;
		std::vector<int64_t>                               code_;
		tbb::concurrent_queue<std::pair<int64_t, int64_t>> queue_;
		MessageFunction                                    messageFunction_;
		IdleFunction                                       idleFunction_;
		Logger*                                            logger_ = nullptr;
	};
}

int main(int argc, char* argv[])
{
	const auto code = io::readLineOfIntegers("day23_input.txt");

	auto nat       = NAT{};
	auto computers = std::vector<Computer>{};
	for (int64_t i = 0; i < 50; ++i)
		computers.emplace_back(i, code);

	const auto messageFunction = [&](int64_t to, int64_t x, int64_t y) {
		if (to == 255)
			nat.putMessage(x, y);
		else
			computers[to].putMessage(x, y);
	};

	const auto idleFunction = [&](int64_t address, bool idle) { nat.setIdle(address, idle); };

	auto logger = Logger{ false };

	nat.setMessageFunction(messageFunction);
	nat.setLogger(logger);
	for (int64_t i = 0; i < 50; ++i) {
		computers[i].setMessageFunction(messageFunction);
		computers[i].setIdleFunction(idleFunction);
		computers[i].setLogger(logger);
	}

	tbb::task_scheduler_init init{ 52 };
	tbb::task_group          g;
	g.run([&] { nat.run(); });
	for (int64_t i = 0; i < 50; ++i)
		g.run([i, &computers] { computers[i].run(); });
	g.run([&] { logger.run(); });
	g.wait();
}
