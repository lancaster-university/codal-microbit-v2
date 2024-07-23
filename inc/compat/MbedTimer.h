#ifndef MbedTimer_h
#define MbedTimer_h

class MbedTimer {
    private:
        uint32_t _start;
        uint32_t _stop = 0;
    public:
        void start() {
            _start = codal::system_timer_current_time_us();
        }
        
        void stop() {
            _stop = codal::system_timer_current_time_us();
        }
        
        void reset() {
            _start = codal::system_timer_current_time_us();
        }

        float read() {
            if(_stop > 0) {
                return (_stop - _start) / 1000000.0;
            } else {
                return (codal::system_timer_current_time_us() - _start) / 1000000.0;
            }
            
        }

        int read_ms() {
            if(_stop > 0) {
                return (_stop - _start) / 1000;
            } else {
                return (codal::system_timer_current_time_us() - _start) / 1000;
            }
        }

        int read_us() {
            if(_stop > 0) {
                return codal::system_timer_current_time_us() - _start;
            } else {
                return codal::system_timer_current_time_us() - _start;
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

#warning "Use of mbed with CODAL is not recommended! These classes will not always behave as expected and are provided to attempt to support existing extensions. Please write your extension using CODAL."

#endif
