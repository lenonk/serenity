#ifndef SERENITY_COMMON_SERVICE_HPP__
#define SERENITY_COMMON_SERVICE_HPP__

namespace serenity { namespace common {
    
    template <class request_type, class response_type>
    class service {
        public:
            using request = request_type;
            using response = response_type;

            service() { }

            virtual bool handle(const request_type &, response_type &) = 0;
    };
    
} /* http */ } /* serenity */

#endif /* end of include guard: SERENITY_HTTP_SERVICE_HPP__ */
