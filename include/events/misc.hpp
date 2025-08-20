//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#ifndef EVENTS_MISC_HPP
#define EVENTS_MISC_HPP

#include "hx2a/tag_type.hpp"

namespace events {

  using namespace hx2a;

  constexpr char events_brand_name[] = "Events";
  
  // As stored in the App Store notification tokens of users.
  constexpr char events_app_name[] = "events";

  template <tag_t tag>
  constexpr tag_t config_name = hx2a::tag_concat<"events_", tag>;

  // Events database name.
  constexpr char dbname[] = "vdb";

  // Clients state database name.
  constexpr char client_state_dbname[] = "csdb";
  
  // It'll be prefixed.
  constexpr tag_t min_app_version_name = config_name<"min_app_version">;
  constexpr double default_min_app_version = 1.0;
  // As coming from the configuration file with the default value above if left undefined.
  double get_min_app_version();
  
  constexpr tag_t events_search_limit_name = config_name<"events_search_limit">;
  // In number of events.
  constexpr size_t default_events_search_limit = 100;
  size_t get_events_search_limit();

  constexpr tag_t venues_search_limit_name = config_name<"venues_search_limit">;
  // In number of venues.
  constexpr size_t default_venues_search_limit = 100;
  size_t get_venues_search_limit();
  
  constexpr tag_t contacts_in_open_invite_limit_name = config_name<"contacts_in_open_invite_limit">;
  constexpr size_t default_contacts_in_open_invite_limit = 16;
  size_t get_contacts_in_open_invite_limit();
  
} // End namespace events.

#endif
