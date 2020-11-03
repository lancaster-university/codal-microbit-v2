#ifndef MbedTimer_h
#define MbedTimer_h

class MbedTimer {
    private:
        uint32_t _start;
        uint32_t _stop = 0;
    public:
        void start() {
            _start = system_timer_current_time_us();
        }
        
        void stop() {
            _stop = system_timer_current_time_us();
        }
        
        void reset() {
            _start = system_timer_current_time_us();
        }

        float read() {
            if(_stop > 0) {
                return (_stop - _start) / 1000000.0;
            } else {
                return (system_timer_current_time_us() - _start) / 1000000.0;
            }
            
        }

        int read_ms() {
            if(_stop > 0) {
                return (_stop - _start) / 1000;
            } else {
                return (system_timer_current_time_us() - _start) / 1000;
            }
        }

        int read_us() {
            if(_stop > 0) {
                return system_timer_current_time_us() - _start;
            } else {
                return system_timer_current_time_us() - _start;
            }
        }

        operator float() {
            return read();
        }

        int read_high_resolution_us() {
            return read_us();
        }

        int elapsed_time() {
            return read_ms();
        }

};

#endif
