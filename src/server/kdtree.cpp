//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#include "hx2a/db/connector.hpp"

#include "events/kdtree.hpp"

// To do: put the limit, the lag, the refresh delay and the decay factor of the kdcaches in the configuration file (optionally).

namespace events {

  // The container returned by the functions below is not const to allow removal of elements when the database
  // does not find the document any longer.

  cached_venues_type& get_cached_venues(const db::connector& cn){
    // Statics are thread-safe.
    static cached_venues_type c("venues kdcache", cn, venue::index_by_save_timestamp, 128);
    return c;
  }

  cached_events_type& get_cached_events(const db::connector& cn){
    // Statics are thread-safe.
    static cached_events_type c{"events kdcache", cn, event::index_by_save_timestamp, 128};
    return c;
  }

} // End namespace events.

