//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#ifndef EVENTS_EXCEPTION_HPP
#define EVENTS_EXCEPTION_HPP

#include "hx2a/exception.hpp"

#include "events/tags.hpp"

namespace events {

  using hx2a::tag_t;

  // workaroun g++10 ICE.
  template <tag_t application_code, tag_t application_message>
  struct exception_t
  {
    static constexpr auto tag = type_tag<application_code>;
    using type = hx2a::application_exception<tag, application_message>;
  };

  template <tag_t application_code, tag_t application_message>
  using exception = typename exception_t<application_code, application_message>::type;
  
  using booking_already_made = exception<"bookmade", "Booking already made.">;
  using booking_does_not_exist = exception<"bookmiss", "Booking does not exist.">;
  using booking_is_not_for_the_event = exception<"booknotforev", "Booking is not for the event.">;

  using client_state_does_not_exist = exception<"csmiss", "Client state does not exist.">;
  using client_state_already_exists = exception<"csexists", "Client state already exists.">;
  
  using contacts_in_open_invite_limit_reached = exception<"contil", "Contacts in open invite limit reached.">;
  
  using event_cannot_be_checked_in = exception<"ecnotci", "Event cannot be checked in.">;
  using event_does_not_exist = exception<"emiss", "Event does not exist.">;
  using event_is_canceled = exception<"ecx", "Event is canceled.">;
  using event_is_confirmed = exception<"econf", "Event is confirmed.">;
  using event_is_not_bookable = exception<"enbook", "Event is not bookable.">;
  using event_is_rejected = exception<"erej", "Event is rejected.">;
  using event_organizer_display_name_missing = exception<"eodnmiss", "Event organizer display name is missing.">;
  
  using insufficient_capacity = exception<"inscap", "Insufficient capacity.">;
  using invalid_capacity = exception<"invcap", "Invalid capacity.">;
  
  using invite_already_made = exception<"invmade", "Invite already made.">;
  using invite_does_not_exist = exception<"invmiss", "Invite does not exist.">;
  
  using news_does_not_exist = exception<"nmiss", "The news does not exist.">;
  using news_expiry_in_the_past = exception<"nexpast", "The news expiry time is in the past.">;
  using news_text_is_empty = exception<"ntmiss", "The news text is empty.">;
  
  using position_missing = exception<"pmiss", "Position is missing.">;
  
  using too_late = exception<"late", "Too late to perform operation.">;
  
  using events_organization_does_not_exist = exception<"vdne", "The organization named \"events\" does not exist.">;
  using events_organization_duplicate = exception<"vdup", "There are several organizations named \"events\".">;
  
  using venue_claim_does_not_exist = exception<"vcmiss", "Venue claim does not exist.">;
  
  using venue_does_not_exist = exception<"vmiss", "Venue does not exist.">;
  
} // End namespace events.

#endif
