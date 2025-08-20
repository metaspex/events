//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#ifndef EVENTS_KDTREE_HPP
#define EVENTS_KDTREE_HPP

// This header implements two kdcaches, one for venues, one for events.
// The pattern is: Large cache implementing search and with inexpensive document gets.

#include <time.h>

#include "hx2a/kdcache.hpp"
#include "hx2a/db/connector.hpp"

#include "events/ontology.hpp"

namespace events {

  // Venues
  
  inline doc_id venue_get_owner_id(const venue& v){ return v.get_owner()->get_id(); }
  inline latitude_t venue_get_latitude(const venue& v){ return v.get_position()->get_latitude(); }
  inline longitude_t venue_get_longitude(const venue& v){ return v.get_position()->get_longitude(); }
  inline bool venue_is_private(const venue& v){ return v.is_private(); }
  inline category_t venue_get_category(const venue& v){ return v.get_category(); }

  using cached_venues_type = kdcache<
    venue,
    slice_g<venue, doc_id, venue_get_owner_id>,
    slice_g<venue, latitude_t, venue_get_latitude>,
    slice_g<venue, longitude_t, venue_get_longitude>,
    slice_g<venue, bool, venue_is_private>,
    slice_g<venue, category_t, venue_get_category>
    >;

  using cached_venue = cached_venues_type::cached;
  
  cached_venues_type& get_cached_venues(const db::connector& cn);

  // Events
  
  inline doc_id event_get_organizer_id(const event& e){ return e.get_organizer()->get_id(); }
  inline latitude_t event_get_latitude(const event& e){ return e.get_position()->get_latitude(); }
  inline longitude_t event_get_longitude(const event& e){ return e.get_position()->get_longitude(); }
  inline bool event_is_private(const event& e){ return e.is_private(); }
  inline category_t event_get_category(const event& e){ return e.get_category(); }
  inline time_t event_get_start(const event& e){ return e.get_start(); }
  
  using cached_events_type = kdcache<
    event,
    slice_g<event, doc_id, event_get_organizer_id>,
    slice_g<event, latitude_t, event_get_latitude>,
    slice_g<event, longitude_t, event_get_longitude>,
    slice_g<event, bool, event_is_private>,
    slice_g<event, category_t, event_get_category>,
    slice_g<event, time_t, event_get_start>
    >;

  using cached_event = cached_events_type::cached;
  
  cached_events_type& get_cached_events(const db::connector& cn);

} // End namespace events.

#endif
