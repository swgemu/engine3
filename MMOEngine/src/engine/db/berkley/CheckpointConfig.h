/*
 * CheckpointConfig.h
 *
 *  Created on: 15/02/2010
 *      Author: victor
 */

#ifndef CHECKPOINTCONFIG_H_
#define CHECKPOINTCONFIG_H_

namespace engine {
  namespace db {
    namespace berkley {

	class CheckpointConfig {
		uint32 KBytes;
		uint32 minutes;
		bool force;

	public:
		static CheckpointConfig DEFAULT;

	public:
		CheckpointConfig() {
			KBytes = 0;
			minutes = 0;
			bool force = false;
		}

		inline void setKBytes(uint32 kbytes) {
			KBytes = kbytes;
		}

		inline void setMinutes(uint32 min) {
			minutes = min;
		}

		inline void setForce(bool forceCheckpoint) {
			force = forceCheckpoint;
		}

		inline uint32 getKBytes() const {
			return KBytes;
		}

		inline uint32 getMinutes() const {
			return minutes;
		}

		inline bool getForce() const {
			return force;
		}
	};

    } // namespace berkley
  } // namespace db
} // namespace engine

#endif /* CHECKPOINTCONFIG_H_ */
