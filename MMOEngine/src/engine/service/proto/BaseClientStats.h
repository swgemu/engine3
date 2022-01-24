/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

/**
 * @author      : lordkator (lordkator@swgemu.com)
 * @file        : BaseClientStats.h
 * @created     : Mon Jan 24 12:40:12 UTC 2022
 */

#pragma once

#include "system/lang.h"

namespace engine {
 namespace service {
  namespace proto {
	class BaseClient;

	class BaseClientStats {
		// State
		bool isLocal;
		bool needsCalculation;
		Time calculationTimeStamp;

		// Facts
		uint64 totalPacketsSent;
		uint64 totalPacketsReceived;
		uint16 tickDelta;
		uint64 lastAckMs;
		uint64 totalAckMs;
		uint64 countAcks;
		uint64 minAckMs;
		uint64 maxAckMs;
		uint16 maxPacketsSentPerSecond;
		uint16 maxPacketsReceivedPerSecond;

		// Calculations
		uint64 lastTotalPacketsSent;
		uint64 lastTotalPacketsReceived;
		uint16 packetsSentPerSecond;
		uint16 packetsReceivedPerSecond;
		uint16 avgAckMs;

	public:
		BaseClientStats(bool local = false) {
			isLocal = local;
			reset();
		}

		void reset() {
			needsCalculation = false;
			totalPacketsSent = 0;
			totalPacketsReceived = 0;
			tickDelta = 0;
			lastAckMs = 0;
			totalAckMs = 0;
			countAcks = 0;
			minAckMs = ULLONG_MAX;
			maxAckMs = 0;
			maxPacketsSentPerSecond = 0;
			maxPacketsReceivedPerSecond = 0;
			lastTotalPacketsSent = 0;
			lastTotalPacketsReceived = 0;
			packetsSentPerSecond = 0;
			packetsReceivedPerSecond = 0;
			avgAckMs = 0;
		}

		uint64 getTotalPacketsSent() const {
			return totalPacketsSent;
		}

		void setTotalPacketsSent(uint64 newTotalPacketsSent) {
			totalPacketsSent = newTotalPacketsSent;
			needsCalculation = true;
		}

		uint64 getTotalPacketsReceived() const {
			return totalPacketsReceived;
		}

		void setTotalPacketsReceived(uint64 newTotalPacketsReceived) {
			totalPacketsReceived = newTotalPacketsReceived;
			needsCalculation = true;
		}

		void setTimeStamp(const Time& timeStamp) {
			calculationTimeStamp = timeStamp;
			needsCalculation = true;
		}

		uint16 getPacketsSentPerSecond() {
			updateCalculatedStats(false);
			return packetsSentPerSecond;
		}

		uint16 getPacketsReceivedPerSecond() {
			updateCalculatedStats(false);
			return packetsReceivedPerSecond;
		}

		uint16 getTickDelta() const {
			return tickDelta;
		}

		void setTickDelta(uint16 newTickDelta) {
			tickDelta = newTickDelta;
		}

		uint64 getLastAckMs() const {
			return lastAckMs;
		}

		uint64 getTotalAckMs() const {
			return totalAckMs;
		}

		uint64 getCountAcks() const {
			return countAcks;
		}

		uint64 getMinAckMs() const {
			return minAckMs == ULLONG_MAX ? 0 : minAckMs;
		}

		uint64 getMaxAckMs() const {
			return maxAckMs;
		}

		uint16 getAvgAckMs() {
			avgAckMs = countAcks > 0 ? totalAckMs / countAcks : 0;
			return avgAckMs;
		}

		void updateAckStats(int64 elapsedMs) {
			++countAcks;

			totalAckMs += elapsedMs;
			avgAckMs = countAcks > 0 ? totalAckMs / countAcks : 0;

			lastAckMs = elapsedMs;

			if (lastAckMs > 0 && lastAckMs < minAckMs) {
				minAckMs = elapsedMs;
			}

			if (lastAckMs > maxAckMs) {
				maxAckMs = lastAckMs;
			}
		}

		void updateCalculatedStats(bool resetPeriod = false, bool forceCalculation = false) {
			if (needsCalculation || forceCalculation) {
				auto elapsedSeconds = (calculationTimeStamp.miliDifference(Time::MONOTONIC_TIME) / 1000);

				if (elapsedSeconds > 0) {
					packetsSentPerSecond = (totalPacketsSent - lastTotalPacketsSent) / elapsedSeconds;
					lastTotalPacketsSent = totalPacketsSent;

					if (packetsSentPerSecond > maxPacketsSentPerSecond) {
						maxPacketsSentPerSecond = packetsSentPerSecond;
					}

					packetsReceivedPerSecond = (totalPacketsReceived - lastTotalPacketsReceived) / elapsedSeconds;
					lastTotalPacketsReceived = totalPacketsReceived;

					if (packetsReceivedPerSecond > maxPacketsReceivedPerSecond) {
						maxPacketsReceivedPerSecond = packetsReceivedPerSecond;
					}
				}

				needsCalculation = false;
			}

			if (resetPeriod) {
				calculationTimeStamp.updateToCurrentTime(Time::MONOTONIC_TIME);
			}
		}

		String asJSONFragment(bool resetTimePeriod = true) {
			updateCalculatedStats();

			StringBuffer buf;
			String prefix = isLocal ? "local" : "remote";

			auto packetStatsAge = (calculationTimeStamp.miliDifference(Time::MONOTONIC_TIME) / 1000);

			buf << ", \"" << prefix << "TotalPacketsSent\":" << totalPacketsSent
				<< ", \"" << prefix << "TotalPacketsReceived\":" << totalPacketsReceived
				<< ", \"" << prefix << "PacketsSentPerSecond\":" << packetsSentPerSecond
				<< ", \"" << prefix << "PacketsReceivedPerSecond\":" << packetsReceivedPerSecond
				<< ", \"" << prefix << "MaxPacketsSentPerSecond\":" << maxPacketsSentPerSecond
				<< ", \"" << prefix << "MaxPacketsReceivedPerSecond\":" << maxPacketsReceivedPerSecond
				<< ", \"" << prefix << "PacketStatsAge\":" << (packetStatsAge < 0 ? 0 : packetStatsAge)
				<< ", \"" << prefix << "TickDelta\":" << tickDelta
				<< ", \"" << prefix << "LastAckMs\":" << lastAckMs
				<< ", \"" << prefix << "MinAckMs\":" << (minAckMs == ULLONG_MAX ? 0 : minAckMs)
				<< ", \"" << prefix << "AvgAckMs\":" << avgAckMs
				<< ", \"" << prefix << "MaxAckMs\":" <<  maxAckMs
				<< ", \"" << prefix << "CountAcks\":" << countAcks
				<< ", \"" << prefix << "TotalAckMs\":" << totalAckMs
				;

			return buf.toString();
		}
	};
}
}
}
