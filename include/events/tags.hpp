//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#ifndef EVENTS_TAGS_HPP
#define EVENTS_TAGS_HPP

#include "hx2a/tag_type.hpp"
#include "hx2a/service_name.hpp"

// List of "readable" tags for payloads. Enforces reuse and consistency.

namespace events {

  using hx2a::tag_t;

  // Useful for type tags and exceptions.
  template <tag_t tag>
  constexpr tag_t type_tag = hx2a::tag_concat<"events:", tag>;

  // Used for service names.
  template <hx2a::service_name_t tag>
  constexpr hx2a::service_name_t srv_tag = hx2a::srv_concat<"events_", tag>;

  constexpr tag_t address_tag                             = "addr";
  constexpr tag_t alternate_email_tag                     = "amail";
  constexpr tag_t area_tag                                = "area";
  constexpr tag_t bookable_tag                            = "bookable";
  constexpr tag_t booking_creation_time_tag               = "ct";
  constexpr tag_t booking_id_tag                          = "booking_id";
  constexpr tag_t booking_notice_time_tag                 = "bnt";
  constexpr tag_t booking_update_time_tag                 = "ut";
  constexpr tag_t bookings_count_tag                      = "bookings_count";
  constexpr tag_t capacity_tag                            = "capacity";
  constexpr tag_t categories_tag                          = "categories";
  constexpr tag_t category_description_tag                = "category_desc";
  constexpr tag_t category_tag                            = "category";
  constexpr tag_t check_in_time_tag                       = "cit";
  constexpr tag_t confirmed_tag                           = "confirmed";
  constexpr tag_t contacts_tag                            = "contacts";
  constexpr tag_t conversation_id_tag                     = "conversation_id";
  constexpr tag_t description_tag                         = "description";
  constexpr tag_t display_name_tag                        = "display_name";
  constexpr tag_t duration_tag                            = "duration";
  constexpr tag_t email_tag                               = "email";
  constexpr tag_t end_tag                                 = "end";
  constexpr tag_t event_confirmation_required_tag         = "event_conf_req";
  constexpr tag_t event_id_tag                            = "event_id";
  constexpr tag_t event_name_tag                          = "event_name";
  constexpr tag_t event_start_tag                         = "event_start";
  constexpr tag_t event_tag                               = "event";
  constexpr tag_t events_tag                              = "events";
  constexpr tag_t expiry_timestamp_tag                    = "expiry";
  constexpr tag_t first_name_tag                          = "first_name";
  constexpr tag_t guest_id_tag                            = "guest_id";
  constexpr tag_t guest_tag                               = "guest";
  constexpr tag_t host_tag                                = "host";
  constexpr tag_t id_tag                                  = "id";
  constexpr tag_t images_tag                              = "images";
  constexpr tag_t invite_creation_time_tag                = "ct";
  constexpr tag_t invite_id_tag                           = "invite_id";
  constexpr tag_t last_name_tag                           = "last_name";
  constexpr tag_t messenger_participation_id_tag          = "msg_partid";
  constexpr tag_t name_tag                                = "name";
  constexpr tag_t new_owner_id_tag                        = "new_owner_id";
  constexpr tag_t news_id_tag                             = "news_id";
  constexpr tag_t note_tag                                = "note";
  constexpr tag_t organizer_display_name_tag              = "organizer_dn";
  constexpr tag_t organizer_tag                           = "organizer";
  constexpr tag_t owner_tag                               = "owner";
  constexpr tag_t position_tag                            = "position";
  constexpr tag_t private_tag                             = "private";
  constexpr tag_t rating_tag                              = "rating";
  constexpr tag_t reason_tag                              = "reason";
  constexpr tag_t report_count_tag                        = "report_count";
  constexpr tag_t start_tag                               = "start";
  constexpr tag_t state_tag                               = "state";
  constexpr tag_t text_tag                                = "text";
  constexpr tag_t timestamp_tag                           = "timestamp";
  constexpr tag_t user_tag                                = "user";
  constexpr tag_t user_doc_id_tag                         = "user_doc_id";
  constexpr tag_t user_id_tag                             = "user_id";
  constexpr tag_t venue_claim_id_tag                      = "venue_claim_id";
  constexpr tag_t venue_id_tag                            = "venue_id";
  constexpr tag_t venue_name_tag                          = "venue_name";
  constexpr tag_t venue_tag                               = "venue";
  constexpr tag_t venues_tag                              = "venues";
  
} // End namespace events.

#endif

