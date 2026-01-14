//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#ifndef EVENTS_PAYLOADS_HPP
#define EVENTS_PAYLOADS_HPP

#include "hx2a/element.hpp"

#include "hx2a/components/area.hpp"
#include "hx2a/components/period.hpp"
#include "hx2a/components/position.hpp"

#include "hx2a/payloads/email_payload.hpp"
#include "hx2a/payloads/query_name.hpp"

#include "messenger/payloads.hpp"

#include "events/ontology.hpp"
#include "events/tags.hpp"

namespace events {

  using namespace hx2a;

  class min_app_version_payload;
  using min_app_version_payload_p = ptr<min_app_version_payload>;
  using min_app_version_payload_r = rfr<min_app_version_payload>;

  class venue_claim_data_payload;
  using venue_claim_data_payload_p = ptr<venue_claim_data_payload>;
  using venue_claim_data_payload_r = rfr<venue_claim_data_payload>;

  class venue_update_images_payload;
  using venue_update_images_payload_p = ptr<venue_update_images_payload>;
  using venue_update_images_payload_r = rfr<venue_update_images_payload>;

  class venue_transfer_payload;
  using venue_transfer_payload_p = ptr<venue_transfer_payload>;
  using venue_transfer_payload_r = rfr<venue_transfer_payload>;

  class venue_data_payload;
  using venue_data_payload_p = ptr<venue_data_payload>;
  using venue_data_payload_r = rfr<venue_data_payload>;

  class venue_data_with_id_payload;
  using venue_data_with_id_payload_p = ptr<venue_data_with_id_payload>;
  using venue_data_with_id_payload_r = rfr<venue_data_with_id_payload>;

  class venue_search_data_payload;
  using venue_search_data_payload_p = ptr<venue_search_data_payload>;
  using venue_search_data_payload_r = rfr<venue_search_data_payload>;

  class venue_search_query;
  using venue_search_query_p = ptr<venue_search_query>;
  using venue_search_query_r = rfr<venue_search_query>;

  class venue_search_reply;
  using venue_search_reply_p = ptr<venue_search_reply>;
  using venue_search_reply_r = rfr<venue_search_reply>;

  class event_data_payload;
  using event_data_payload_p = ptr<event_data_payload>;
  using event_data_payload_r = rfr<event_data_payload>;

  class event_data_for_organizer_payload;
  using event_data_for_organizer_payload_p = ptr<event_data_for_organizer_payload>;
  using event_data_for_organizer_payload_r = rfr<event_data_for_organizer_payload>;

  class event_data_with_id_payload;
  using event_data_with_id_payload_p = ptr<event_data_with_id_payload>;
  using event_data_with_id_payload_r = rfr<event_data_with_id_payload>;

  class event_details_payload;
  using event_details_payload_p = ptr<event_details_payload>;
  using event_details_payload_r = rfr<event_details_payload>;

  class smart_event_data_payload;
  using smart_event_data_payload_p = ptr<smart_event_data_payload>;
  using smart_event_data_payload_r = rfr<smart_event_data_payload>;

  class public_event_data_payload;
  using public_event_data_payload_p = ptr<public_event_data_payload>;
  using public_event_data_payload_r = rfr<public_event_data_payload>;

  class event_search_data_payload;
  using event_search_data_payload_p = ptr<event_search_data_payload>;
  using event_search_data_payload_r = rfr<event_search_data_payload>;

  class event_search_reply;
  using event_search_reply_p = ptr<event_search_reply>;
  using event_search_reply_r = rfr<event_search_reply>;

  class event_data_for_organizer_payload;
  using event_data_for_organizer_payload_p = ptr<event_data_for_organizer_payload>;
  using event_data_for_organizer_payload_r = rfr<event_data_for_organizer_payload>;

  class event_data_for_venue_owner_payload;
  using event_data_for_venue_owner_payload_p = ptr<event_data_for_venue_owner_payload>;
  using event_data_for_venue_owner_payload_r = rfr<event_data_for_venue_owner_payload>;

  class event_update_payload;
  using event_update_payload_p = ptr<event_update_payload>;
  using event_update_payload_r = rfr<event_update_payload>;
  
  class event_update_images_payload;
  using event_update_images_payload_p = ptr<event_update_images_payload>;
  using event_update_images_payload_r = rfr<event_update_images_payload>;
  
  class event_change_venue_payload;
  using event_change_venue_payload_p = ptr<event_change_venue_payload>;
  using event_change_venue_payload_r = rfr<event_change_venue_payload>;
  
  class event_cancel_payload;
  using event_cancel_payload_p = ptr<event_cancel_payload>;
  using event_cancel_payload_r = rfr<event_cancel_payload>;

  class open_invite_data_payload;
  using open_invite_data_payload_p = ptr<open_invite_data_payload>;
  using open_invite_data_payload_r = rfr<open_invite_data_payload>;
  
  class open_invite_create_payload;
  using open_invite_create_payload_p = ptr<open_invite_create_payload>;
  using open_invite_create_payload_r = rfr<open_invite_create_payload>;
  
  class open_invite_add_contact_payload;
  using open_invite_add_contact_payload_p = ptr<open_invite_add_contact_payload>;
  using open_invite_add_contact_payload_r = rfr<open_invite_add_contact_payload>;
  
  class invite_create_payload;
  using invite_create_payload_p = ptr<invite_create_payload>;
  using invite_create_payload_r = rfr<invite_create_payload>;
  
  class invite_data_payload;
  using invite_data_payload_p = ptr<invite_data_payload>;
  using invite_data_payload_r = rfr<invite_data_payload>;

  class booking_data_payload;
  using booking_data_payload_p = ptr<booking_data_payload>;
  using booking_data_payload_r = rfr<booking_data_payload>;

  class booking_data_with_id_payload;
  using booking_data_with_id_payload_p = ptr<booking_data_with_id_payload>;
  using booking_data_with_id_payload_r = rfr<booking_data_with_id_payload>;

  class booking_and_event_data_payload;
  using booking_and_event_data_payload_p = ptr<booking_and_event_data_payload>;
  using booking_and_event_data_payload_r = rfr<booking_and_event_data_payload>;

  class venue_claim_id_payload;
  using venue_claim_id_payload_p = ptr<venue_claim_id_payload>;
  using venue_claim_id_payload_r = rfr<venue_claim_id_payload>;

  class venue_id_payload;
  using venue_id_payload_p = ptr<venue_id_payload>;
  using venue_id_payload_r = rfr<venue_id_payload>;

  class event_id_payload;
  using event_id_payload_p = ptr<event_id_payload>;
  using event_id_payload_r = rfr<event_id_payload>;

  class open_invite_id_payload;
  using open_invite_id_payload_p = ptr<open_invite_id_payload>;
  using open_invite_id_payload_r = rfr<open_invite_id_payload>;

  class invite_id_payload;
  using invite_id_payload_p = ptr<invite_id_payload>;
  using invite_id_payload_r = rfr<invite_id_payload>;

  class open_invite_details_payload;
  using open_invite_details_payload_p = ptr<open_invite_details_payload>;
  using open_invite_details_payload_r = rfr<open_invite_details_payload>;

  class invite_details_payload;
  using invite_details_payload_p = ptr<invite_details_payload>;
  using invite_details_payload_r = rfr<invite_details_payload>;

  class booking_id_payload;
  using booking_id_payload_p = ptr<booking_id_payload>;
  using booking_id_payload_r = rfr<booking_id_payload>;

  class news_id_payload;
  using news_id_payload_p = ptr<news_id_payload>;
  using news_id_payload_r = rfr<news_id_payload>;

  class news_create_payload;
  using news_create_payload_p = ptr<news_create_payload>;
  using news_create_payload_r = rfr<news_create_payload>;

  class news_data_payload;
  using news_data_payload_p = ptr<news_data_payload>;
  using news_data_payload_r = rfr<news_data_payload>;

  class min_app_version_payload: public element<>
  {
    HX2A_ELEMENT(min_app_version_payload, type_tag<"min_app_version_pld">, element,
		 ((_min_app_version, "v")));
  public:

    min_app_version_payload():
      _min_app_version(*this, get_min_app_version())
    {
    }

    slot<double> _min_app_version;
  };

  class user_data_payload: public element<>
  {
    HX2A_ELEMENT(user_data_payload, type_tag<"user_data_pld">, element,
		 ((user_doc_id, user_doc_id_tag),
		  (user_id, user_id_tag),
		  (first_name, first_name_tag),
		  (last_name, last_name_tag),
		  (email, email_tag),
		  (alternate_email, alternate_email_tag)));
  public:

    user_data_payload(const user_r& u):
      user_doc_id(*this, u->get_id()),
      user_id(*this, u->get_user_id()),
      first_name(*this, u->get_forename()),
      last_name(*this, u->get_lastname()),
      email(*this, u->get_email()),
      alternate_email(*this, u->get_alternate_email())
    {
    }

    slot<doc_id> user_doc_id;
    slot<string> user_id;
    slot<string> first_name;
    slot<string> last_name;
    slot<string> email;
    slot<string> alternate_email;
  };

  class images_payload: public element<>
  {
    HX2A_ELEMENT(images_payload, type_tag<"images_pld">, element,
		 ((images, images_tag)));
  public:

    slot_vector<string> images;
  };
  
  class venue_claim_id_payload: public element<>
  {
    HX2A_ELEMENT(venue_claim_id_payload, type_tag<"venue_claim_id_pld">, element,
		 ((venue_claim_id, venue_claim_id_tag)));
  public:

    venue_claim_id_payload(const venue_claim_r& v):
      venue_claim_id(*this, v->get_id())
    {
    }
    
    slot<doc_id> venue_claim_id;
  };

  class venue_id_payload: public element<>
  {
    HX2A_ELEMENT(venue_id_payload, type_tag<"venue_id_pld">, element,
		 ((venue_id, venue_id_tag)));
  public:

    venue_id_payload(const venue_r& v):
      venue_id(*this, v->get_id())
    {
    }
    
    slot<doc_id> venue_id;
  };

  class venue_data_payload: public query_name
  {
    HX2A_ELEMENT(venue_data_payload, type_tag<"venue_data_pld">, query_name,
		 ((owner, owner_tag),
		  (is_private, private_tag),
		  (category, category_tag),
		  (category_description, category_description_tag),
		  (pos, position_tag),
		  (addr, address_tag),
		  (capacity, capacity_tag),
		  (description, description_tag),
		  (event_confirmation_required, event_confirmation_required_tag),
		  (images, images_tag),
		  (rating, rating_tag)));
  public:

    venue_data_payload(serial_t):
      query_name(serial),
      owner(*this),
      is_private(*this, true),
      category(*this, default_category),
      category_description(*this),
      pos(*this),
      addr(*this),
      capacity(*this, infinite_capacity),
      description(*this),
      event_confirmation_required(*this, false),
      images(*this),
      rating(*this, 0)
    {
    }

    venue_data_payload(const venue_r& v):
      query_name(v->get_name()),
      owner(*this, make<user_data_payload>(v->get_owner())),
      is_private(*this, v->is_private()),
      category(*this, v->get_category()),
      category_description(*this, v->get_category_description()),
      pos(*this, v->get_position()->copy()),
      addr(*this, v->get_address()->copy()),
      capacity(*this, v->get_capacity()),
      description(*this, v->get_description()),
      event_confirmation_required(*this, v->get_event_confirmation_required()),
      images(*this),
      rating(*this, v->get_rating())
    {
      const venue::images_type& im = v->get_images();
      // This should be replaced by a reserve followed by a std::copy with back_inserter when it compiles.
      images.resize(im.size());
      auto i = im.cbegin();
      auto e = im.cend();
      auto w = images.begin();

      while (i != e){
	*w = (*i).get();
	++i;
	++w;
      }
    }

    void validate() const {
      if (!pos){
	throw position_missing();
      }

      if (!addr){
	throw address_missing();
      }
    }

    own<user_data_payload> owner;
    slot<bool> is_private;
    slot<category_t> category;
    slot<string> category_description;
    own<position> pos;
    own<address> addr; // Can't call it "address", it is a type name.
    slot<capacity_t> capacity;
    slot<string> description;
    slot<bool> event_confirmation_required;
    slot_vector<string> images;
    slot<venue::rating_t> rating;
  };

  using venue_create_payload = venue_data_payload;
  using venue_create_payload_p = ptr<venue_data_payload>;
  using venue_create_payload_r = rfr<venue_data_payload>;

  class venue_data_with_id_payload: public venue_data_payload
  {
    HX2A_ELEMENT(venue_data_with_id_payload, type_tag<"venue_data_with_id_pld">, venue_data_payload,
		 ((venue_id, venue_id_tag)));
  public:

    venue_data_with_id_payload(const venue_r& v):
      venue_data_payload(v),
      venue_id(*this, v->get_id())
    {
    }
    
    slot<doc_id> venue_id;
  };
  
  class venue_claim_data_payload: public element<>
  {
    HX2A_ELEMENT(venue_claim_data_payload, type_tag<"venue_claim_data_pld">, element,
		 ((timestamp, timestamp_tag),
		  (user_data, user_tag),
		  (venue_data, venue_tag)));
  public:

    venue_claim_data_payload(const venue_claim_r& vc):
      timestamp(*this, vc->get_creation_time()),
      user_data(*this, make<user_data_payload>(vc->get_user())),
      venue_data(*this, make<venue_data_with_id_payload>(vc->get_venue()))
    {
    }

    slot<time_t> timestamp;
    own<user_data_payload> user_data;
    own<venue_data_with_id_payload> venue_data;
  };
  
  class venue_update_payload: public venue_id_payload
  {
    HX2A_ELEMENT(venue_update_payload, type_tag<"venue_update_pld">, venue_id_payload,
		 ((name, name_tag),
		  (category, category_tag),
		  (category_description, category_description_tag),
		  (addr, address_tag),
		  (capacity, capacity_tag),
		  (description, description_tag),
		  (event_confirmation_required, event_confirmation_required_tag),
		  (images, images_tag),
		  (rating, rating_tag)));
  public:

    venue_update_payload(serial_t):
      venue_id_payload(serial),
      name(*this),
      category(*this, default_category),
      category_description(*this),
      addr(*this),
      capacity(*this, infinite_capacity),
      description(*this),
      event_confirmation_required(*this, false),
      images(*this),
      rating(*this, 0) // 0 means unassigned.
    {
    }

    void validate() const {
      if (!addr){
	throw address_missing();
      }
    }

    slot<string> name;
    slot<category_t> category;
    slot<string> category_description;
    own<address> addr; // Can't call it "address", it is a type name.
    slot<capacity_t> capacity;
    slot<string> description;
    slot<bool> event_confirmation_required;
    slot_vector<string> images;
    slot<venue::rating_t> rating;
  };

  class venue_update_images_payload: public images_payload
  {
    HX2A_ELEMENT(venue_update_images_payload, type_tag<"venue_update_images_pld">, images_payload,
		 ((venue_id, venue_id_tag)));
  public:

    slot<doc_id> venue_id;
  };

  class venue_transfer_payload: public venue_id_payload
  {
    HX2A_ELEMENT(venue_transfer_payload, type_tag<"venue_transfer_pld">, venue_id_payload,
		 ((new_owner_id, new_owner_id_tag)));
  public:

    slot<doc_id> new_owner_id;
  };

  // The search results need to incorporate the document identifier.
  class venue_search_data_payload: public venue_data_payload
  {
    HX2A_ELEMENT(venue_search_data_payload, type_tag<"venue_search_data_pld">, venue_data_payload,
		 ((id, id_tag)));
  public:

    venue_search_data_payload(const venue_r& v):
      venue_data_payload(v),
      id(*this, v->get_id())
    {
    }

    slot<doc_id> id;
  };

  class venue_search_query: public area
  {
    HX2A_ELEMENT(venue_search_query, type_tag<"venue_search_query">, area,
		 ((categories, categories_tag)));
  public:
    
    slot_vector<category_t> categories;
  };
  
  class venue_search_reply: public element<>
  {
    HX2A_ELEMENT(venue_search_reply, type_tag<"venue_search_reply">, element,
		 ((venues, venues_tag)));
  public:

    // Created empty, and getting venues data pushed.
    venue_search_reply():
      venues(*this)
    {
    }

    void push_venue_data_back(const venue_search_data_payload_r& vd){
      venues.push_back(vd);
    }

    own_list<venue_search_data_payload> venues;
  };

  // Event-related payloads.
  
  class event_id_payload: public element<>
  {
    HX2A_ELEMENT(event_id_payload, type_tag<"event_id_pld">, element,
		 ((event_id, event_id_tag)));
  public:

    event_id_payload(const event_r& e):
      event_id(*this, e->get_id())
    {
    }
    
    slot<doc_id> event_id;
  };

  class event_create_payload: public query_name
  {
    HX2A_ELEMENT(event_create_payload, type_tag<"event_create_pld">, query_name,
		 ((venue_id, venue_id_tag),
		  (is_private, private_tag),
		  (category, category_tag),
		  (category_description, category_description_tag),
		  (capacity, capacity_tag),
		  (start, start_tag),
		  (duration, duration_tag),
		  (bookings_notice_time, booking_notice_time_tag),
		  (organizer_display_name, organizer_display_name_tag),
		  (images, images_tag)));
  public:

    event_create_payload(serial_t):
      query_name(serial),
      venue_id(*this),
      is_private(*this, true),
      category(*this, default_category),
      category_description(*this),
      capacity(*this, uninitialized_capacity),
      start(*this),
      duration(*this),
      bookings_notice_time(*this, 0), // 0 means no bookings notice time.
      organizer_display_name(*this),
      images(*this)
    {
    }

    event_create_payload(const event_r& ev):
      query_name(ev->get_name()),
      venue_id(*this, ev->get_venue()->get_id()),
      is_private(*this, ev->is_private()),
      category(*this, ev->get_category()),
      category_description(*this, ev->get_category_description()),
      capacity(*this, ev->get_capacity()),
      start(*this, ev->get_start()),
      duration(*this, ev->get_duration()),
      bookings_notice_time(*this, ev->get_bookings_notice_time()),
      organizer_display_name(*this, ev->get_organizer_display_name()),
      images(*this)
    {
      const event::images_type& im = ev->get_images();
      // This should be replaced by a reserve followed by a std::copy with back_inserter when it compiles.
      images.resize(im.size());
      auto i = im.cbegin();
      auto e = im.cend();
      auto w = images.begin();

      while (i != e){
	*w = (*i).get();
	++i;
	++w;
      }
    }

    void validate() const {
      if (organizer_display_name.get().empty()){
	throw event_organizer_display_name_missing();
      }
    }
    
    slot<doc_id> venue_id;
    slot<bool> is_private;
    slot<category_t> category;
    slot<string> category_description;
    slot<capacity_t> capacity;
    slot<time_t> start;
    slot<time_t> duration;
    slot<time_t> bookings_notice_time;
    slot<string> organizer_display_name;
    slot_vector<string> images;
  };

  // This is not for the organizer.
  // The venue name is both in the type and in the venue data, this is for upward compatibility
  // with previous versions of the App. To be removed at some points.
  class event_data_payload: public event_create_payload
  {
    HX2A_ELEMENT(event_data_payload, type_tag<"event_data_pld">, event_create_payload,
		 ((organizer, organizer_tag),
		  (venue_name, venue_name_tag),
		  (venue_data, venue_tag),
		  (state, state_tag),
		  (conversation_id, conversation_id_tag),
		  (bookable, bookable_tag)));
  public:

    event_data_payload(const event_r& e):
      event_create_payload(e),
      organizer(*this, make<user_data_payload>(e->get_organizer())),
      venue_name(*this, e->get_venue()->get_name()),
      venue_data(*this, make<venue_data_payload>(e->get_venue())),
      state(*this, e->get_state()),
      conversation_id(*this),
      bookable(*this, e->is_bookable())
    {
      if (messenger::conversation_p conv = e->get_conversation()){
	conversation_id = (*conv)->get_id();
      }
      else{
	HX2A_LOG(error) << "When calculating event data payload, encountered a null conversation on the event.";
      }
    }

    own<user_data_payload> organizer;
    slot<string> venue_name;
    // Redundant with the former, but to extend without breaking upwards compatibilty.
    own<venue_data_payload> venue_data;
    slot<event::state_t> state;
    slot<doc_id> conversation_id;
    slot<bool> bookable;
  };

  class event_data_with_id_payload: public event_data_payload
  {
    HX2A_ELEMENT(event_data_with_id_payload, type_tag<"event_data_with_id_pld">, event_data_payload,
		 ((event_id, event_id_tag)));
  public:

    event_data_with_id_payload(const event_r& e):
      event_data_payload(e),
      event_id(*this, e->get_id())
    {
    }

    slot<doc_id> event_id;
  };
  
  class event_details_payload: public element<>
  {
    HX2A_ELEMENT(event_details_payload, type_tag<"event_details_pld">, element,
		 ((event_id, event_id_tag),
		  (name, name_tag),
		  (is_private, private_tag),
		  (category, category_tag),
		  (category_description, category_description_tag),
		  (start, start_tag),
		  (duration, duration_tag),
		  (bookings_notice_time, booking_notice_time_tag),
		  (organizer_display_name, organizer_display_name_tag),
		  (venue, venue_tag)));
  public:

    event_details_payload(const event_r& e):
      event_id(*this, e->get_id()),
      name(*this, e->get_name()),
      is_private(*this, e->is_private()),
      category(*this, e->get_category()),
      category_description(*this, e->get_category_description()),
      start(*this, e->get_start()),
      duration(*this, e->get_duration()),
      bookings_notice_time(*this, e->get_bookings_notice_time()),
      organizer_display_name(*this, e->get_organizer_display_name()),
      venue(*this, make<venue_data_payload>(e->get_venue()))
    {
    }

    slot<doc_id> event_id;
    slot<string> name;
    slot<bool> is_private;
    slot<category_t> category;
    slot<string> category_description;
    slot<time_t> start;
    slot<time_t> duration;
    slot<time_t> bookings_notice_time;
    slot<string> organizer_display_name;
    own<venue_data_payload> venue;
  };

  // A richer version for the organizer.
  class event_data_for_organizer_payload: public event_create_payload
  {
    HX2A_ELEMENT(event_data_for_organizer_payload, type_tag<"event_data_for_organizer_pld">, event_create_payload,
		 ((venue_name, venue_name_tag),
		  (state, state_tag),
		  (conversation_id, conversation_id_tag),
		  (bookable, bookable_tag),
		  (bookings_count, bookings_count_tag),
		  (report_count, report_count_tag)));
  public:

    event_data_for_organizer_payload(const event_r& e):
      event_create_payload(e),
      venue_name(*this, e->get_venue()->get_name()),
      state(*this, e->get_state()),
      conversation_id(*this),
      bookable(*this, e->is_bookable()),
      bookings_count(*this, e->get_bookings_count()),
      report_count(*this, e->get_report_count())
    {
      if (messenger::conversation_p conv = e->get_conversation()){
	conversation_id = (*conv)->get_id();
      }
      else{
	HX2A_LOG(error) << "When calculating event data payload, encountered a null conversation on the event.";
      }
    }

    slot<string> venue_name;
    slot<event::state_t> state;
    slot<doc_id> conversation_id;
    slot<bool> bookable;
    slot<capacity_t> bookings_count;
    slot<uint64_t> report_count;
  };

  class event_data_for_venue_owner_payload: public event_data_for_organizer_payload
  {
    HX2A_ELEMENT(event_data_for_venue_owner_payload, type_tag<"event_data_for_venue_owner_pld">, event_data_for_organizer_payload,
		 ((organizer, organizer_tag)));
  public:

    event_data_for_venue_owner_payload(const event_r& e):
      event_data_for_organizer_payload(e),
      organizer(*this, make<user_data_payload>(e->get_organizer()))
    {
    }
    
    own<user_data_payload> organizer;
  };

  // Types created to be able to return events for a given venue and a start timestamp for a user who
  // might or might not be the organizer of the event. The data of private events the user is not the organizer
  // of are just returned with their start and end timestamps, without further details. Other events are
  // fully described.
  // As a result the base type contains only the start and the end. The derived type has more data for
  // public events or private events the user is the organizer of.

  class smart_event_data_payload: public event_id_payload
  {
    HX2A_ELEMENT(smart_event_data_payload, type_tag<"smart_event_data_pld">, event_id_payload,
		 ((event_name, event_name_tag),
		  (state, state_tag),
		  (start, start_tag),
		  (end, end_tag)));
  public:

    smart_event_data_payload(const event_r& e):
      event_id_payload(e),
      event_name(*this, e->get_name()),
      state(*this, e->get_state()),
      start(*this, e->get_start()),
      end(*this, e->get_end())
    {
    }

    slot<string> event_name;
    slot<event::state_t> state;
    slot<time_t> start;
    slot<time_t> end;
  };

  // No private indicator, as the data are just for public events.
  class public_event_data_payload: public smart_event_data_payload
  {
    HX2A_ELEMENT(public_event_data_payload, type_tag<"smart_complete_event_data_pld">, smart_event_data_payload,
		 ((category, category_tag),
		  (category_description, category_description_tag),
		  (capacity, capacity_tag),
		  (bookings_notice_time, booking_notice_time_tag),
		  (organizer_display_name, organizer_display_name_tag),
		  (images, images_tag),
		  (bookable, bookable_tag)));
  public:

    public_event_data_payload(const event_r& e):
      smart_event_data_payload(e),
      category(*this, e->get_category()),
      category_description(*this, e->get_category_description()),
      capacity(*this, e->get_capacity()),
      bookings_notice_time(*this, e->get_bookings_notice_time()),
      organizer_display_name(*this, e->get_organizer_display_name()),
      images(*this),
      bookable(*this, e->is_bookable())
    {
    }
    
    slot<category_t> category;
    slot<string> category_description;
    slot<int> capacity;
    slot<time_t> bookings_notice_time;
    slot<string> organizer_display_name;
    slot_vector<string> images;
    slot<bool> bookable;
  };
  
  class event_update_payload: public event_id_payload
  {
    HX2A_ELEMENT(event_update_payload, type_tag<"event_update_pld">, event_id_payload,
		 ((name, name_tag),
		  (category, category_tag),
		  (category_description, category_description_tag),
		  (capacity, capacity_tag),
		  (start, start_tag),
		  (duration, duration_tag),
		  (bookings_notice_time, booking_notice_time_tag)));
  public:

    slot<string> name;
    slot<category_t> category;
    slot<string> category_description;
    slot<capacity_t> capacity;
    slot<time_t> start;
    slot<time_t> duration;
    slot<time_t> bookings_notice_time;
  };
  
  class event_update_images_payload: public images_payload
  {
    HX2A_ELEMENT(event_update_images_payload, type_tag<"event_update_images_pld">, images_payload,
		 ((event_id, event_id_tag)));
  public:

    slot<doc_id> event_id;
  };

  class event_change_venue_payload: public event_id_payload
  {
    HX2A_ELEMENT(event_change_venue_payload, type_tag<"event_change_venue_pld">, event_id_payload,
		 ((venue_id, venue_id_tag)));
  public:

    slot<doc_id> venue_id;
  };

  class event_cancel_payload: public event_id_payload
  {
    HX2A_ELEMENT(event_cancel_payload, type_tag<"event_cancel_pld">, event_id_payload,
		 ((reason, reason_tag)));
  public:

    slot<string> reason;
  };

  class event_search_data_payload: public event_data_payload
  {
    HX2A_ELEMENT(event_search_data_payload, type_tag<"event_search_data_pld">, event_data_payload,
		 ((event_id, event_id_tag)));
  public:

    event_search_data_payload(const event_r& e):
      event_data_payload(e),
      event_id(*this, e->get_id())
    {
    }

    slot<doc_id> event_id;
  };

  class event_search_reply: public element<>
  {
    HX2A_ELEMENT(event_search_reply, type_tag<"event_search_reply">, element,
		 ((events, events_tag)));
  public:

    // Created empty, and getting events data pushed.
    event_search_reply():
      events(*this)
    {
    }

    void push_event_data_back(const event_search_data_payload_r& vd){
      events.push_back(vd);
    }

    own_list<event_search_data_payload> events;
  };

  class event_search_query: public element<>
  {
    HX2A_ELEMENT(event_search_query, type_tag<"event_search_pld">, element,
		 ((the_area, area_tag),
		  (the_period, start_tag),
		  (categories, categories_tag)));
  public:

    own<area> the_area;
    // This is an interval for the start.
    own<period> the_period;
    slot_vector<category_t> categories;
  };

  class open_invite_data_payload: public element<>
  {
    HX2A_ELEMENT(open_invite_data_payload, type_tag<"open_invite_data_pld">, element,
		 ((contacts, contacts_tag)));
  public:

    open_invite_data_payload(const open_invite_r&);

  private:

    own_vector<contact> contacts;
  };
  
  class open_invite_create_payload: public element<>
  {
    HX2A_ELEMENT(open_invite_create_payload, type_tag<"open_invite_create_pld">, element,
		 ((event_id, event_id_tag)));
  public:

    open_invite_create_payload(const open_invite_r& i):
      event_id(*this, i->get_event()->get_id())
    {
    }

    slot<doc_id> event_id;
  };
  
  class open_invite_add_contact_payload: public email_payload
  {
    HX2A_ELEMENT(open_invite_add_contact_payload, type_tag<"open_invite_add_contact_pld">, email_payload,
		 ((invite_id, invite_id_tag),
		  (first_name, first_name_tag),
		  (last_name, last_name_tag)));
  public:

    slot<doc_id> invite_id;
    slot<string> first_name;
    slot<string> last_name;
  };
  
  class invite_create_payload: public element<>
  {
    HX2A_ELEMENT(invite_create_payload, type_tag<"invite_create_pld">, element,
		 ((event_id, event_id_tag),
		  (guest_id, guest_id_tag)));
  public:

    invite_create_payload(const invite_r& i):
      event_id(*this, i->get_event()->get_id()),
      guest_id(*this, i->get_guest()->get_id())
    {
    }

    slot<doc_id> event_id;
    slot<doc_id> guest_id;
  };
  
  class open_invite_id_payload: public element<>
  {
    HX2A_ELEMENT(open_invite_id_payload, type_tag<"open_invite_id_pld">, element,
		 ((invite_id, invite_id_tag)));
  public:

    open_invite_id_payload(const open_invite_r& i):
      invite_id(*this, i->get_id())
    {
    }
    
    slot<doc_id> invite_id;
  };

  class invite_id_payload: public element<>
  {
    HX2A_ELEMENT(invite_id_payload, type_tag<"invite_id_pld">, element,
		 ((invite_id, invite_id_tag)));
  public:

    invite_id_payload(const invite_r& i):
      invite_id(*this, i->get_id())
    {
    }
    
    slot<doc_id> invite_id;
  };

  class invite_data_payload: public element<>
  {
    HX2A_ELEMENT(invite_data_payload, type_tag<"invite_data_pld">, element,
		 ((host_data, host_tag),
		  (guest_data, guest_tag),
		  (event_details, event_tag),
		  (invite_creation_time, invite_creation_time_tag)));
  public:

    invite_data_payload(const invite_r& i):
      host_data(*this, make<user_data_payload>(i->get_host())),
      guest_data(*this, make<user_data_payload>(i->get_guest())),
      event_details(*this, make<event_details_payload>(i->get_event())),
      invite_creation_time(*this, i->get_creation_time())
    {
    }
    
    own<user_data_payload> host_data; 
    own<user_data_payload> guest_data;
    own<event_details_payload> event_details;
    slot<time_t> invite_creation_time;
  };

  // For an invited user to see the details of the invite, the event and the venue.
  // Works for open invites too.
  class invite_details_payload: public element<>
  {
    HX2A_ELEMENT(invite_details_payload, type_tag<"invite_details_pld">, element,
		 ((host, host_tag),
		  (event_details, event_tag),
		  (invite_creation_time, invite_creation_time_tag)));
  public:

    invite_details_payload(const invite_r& i):
      host(*this, make<user_data_payload>(i->get_host())),
      event_details(*this, make<event_details_payload>(i->get_event())),
      invite_creation_time(*this, i->get_creation_time())
    {
    }
    
    invite_details_payload(const open_invite_r& i):
      host(*this, make<user_data_payload>(i->get_host())),
      event_details(*this, make<event_details_payload>(i->get_event())),
      invite_creation_time(*this, i->get_creation_time())
    {
    }
    
    own<user_data_payload> host;
    own<event_details_payload> event_details;
    slot<time_t> invite_creation_time;
  };
  
  class invite_accept_payload: public invite_id_payload
  {
    HX2A_ELEMENT(invite_accept_payload, type_tag<"invite_accept_pld">, invite_id_payload,
		 ((display_name, display_name_tag),
		  (note, note_tag)));
  public:

    slot<string> display_name;
    slot<string> note;
  };
  
  class book_payload: public element<>
  {
    HX2A_ELEMENT(book_payload, type_tag<"book_pld">, element,
		 ((event_id, event_id_tag),
		  (display_name, display_name_tag),
		  (note, note_tag)));
  public:

    slot<doc_id> event_id;
    slot<string> display_name;
    slot<string> note;
  };
  
  class booking_id_payload: public element<>
  {
    HX2A_ELEMENT(booking_id_payload, type_tag<"booking_id_pld">, element,
		 ((booking_id, booking_id_tag)));
  public:

    booking_id_payload(const booking_r& b):
      booking_id(*this, b->get_id())
    {
    }
    
    slot<doc_id> booking_id;
  };

  class booking_data_payload: public element<>
  {
    HX2A_ELEMENT(booking_data_payload, type_tag<"booking_data_pld">, element,
		 ((host_data, host_tag),
		  (guest_data, guest_tag),
		  (note, note_tag),
		  (booking_creation_time, booking_creation_time_tag),
		  (booking_update_time, booking_update_time_tag),
		  (check_in_time, check_in_time_tag)));
  public:

    booking_data_payload(const booking_r& b):
      host_data(*this),
      guest_data(*this, make<user_data_payload>(b->get_guest())),
      note(*this, b->get_note()),
      booking_creation_time(*this, b->get_creation_time()),
      booking_update_time(*this, b->get_save_time()),
      check_in_time(*this, b->get_check_in_timestamp())
    {
      user_p host = b->get_host();

      if (host){
	host_data = make<user_data_payload>(*host);
      }
    }

    own<user_data_payload> host_data;
    own<user_data_payload> guest_data;
    slot<string> note;
    slot<time_t> booking_creation_time;
    slot<time_t> booking_update_time;
    slot<time_t> check_in_time;
  };

  class booking_data_with_id_payload: public booking_data_payload
  {
    HX2A_ELEMENT(booking_data_with_id_payload, type_tag<"booking_data_with_id_pld">, booking_data_payload,
		 ((booking_id, booking_id_tag)));
  public:

    booking_data_with_id_payload(const booking_r& b):
      booking_data_payload(b),
      booking_id(*this, b->get_id())
    {
    }

    slot<doc_id> booking_id;
  };
  
  // The event data are kept redundant with the nested event data for upwards compatibility with older Apps.
  // At some point they should be removed.
  class booking_and_event_data_payload: public booking_data_payload
  {
    HX2A_ELEMENT(booking_and_event_data_payload, type_tag<"booking_and_event_data_pld">, booking_data_payload,
		 ((event_id, event_id_tag),
		  (event_name, event_name_tag),
		  (event_data, event_tag)));
  public:

    booking_and_event_data_payload(const booking_r& b):
      booking_data_payload(b),
      event_id(*this),
      event_name(*this),
      event_data(*this)
    {
      event_r e = b->get_event();
      event_id = e->get_id();
      event_name = e->get_name();
      event_data = make<event_data_payload>(e);
    }

    // We could turn all this into a own of event data.
    slot<doc_id> event_id;
    slot<string> event_name;
    own<event_data_payload> event_data;
  };

  class booking_and_user_data_payload: public booking_data_payload
  {
    HX2A_ELEMENT(booking_and_user_data_payload, type_tag<"booking_and_user_data_pld">, booking_data_payload,
		 ((display_name, display_name_tag),
		  (messenger_participation_id, messenger_participation_id_tag)));
  public:

    booking_and_user_data_payload(const booking_r& b):
      booking_data_payload(b),
      display_name(*this),
      messenger_participation_id(*this)
    {
      messenger::participation_p part = b->get_messenger_participation();

      if (part){
	display_name = (*part)->get_display_name();
	messenger_participation_id = (*part)->get_id();
      }
    }
    
    slot<string> display_name;
    slot<doc_id> messenger_participation_id;
  };

  // The check-in agent is operating for a given event, and the guest presents their booking id.
  class check_in_payload: public event_id_payload
  {
    HX2A_ELEMENT(check_in_payload, type_tag<"check_in_pld">, event_id_payload,
		 ((booking_id, booking_id_tag)));
  public:
    
    slot<doc_id> booking_id;
  };

  class news_id_payload: public element<>
  {
    HX2A_ELEMENT(news_id_payload, type_tag<"news_id_pld">, element,
		 ((news_id, news_id_tag)));
  public:

    news_id_payload(const news_r& v):
      news_id(*this, v->get_id())
    {
    }
    
    slot<doc_id> news_id;
  };

  class news_create_payload: public venue_id_payload
  {
    HX2A_ELEMENT(news_create_payload, type_tag<"news_create_pld">, venue_id_payload,
		 ((text, text_tag),
		  (expiry_timestamp, expiry_timestamp_tag)));
  public:

    news_create_payload(const news_r& n):
      venue_id_payload(n->get_venue()),
      text(*this, n->get_text()),
      expiry_timestamp(*this, n->get_expiry_timestamp())
    {
    }
    
    slot<string> text;
    slot<time_t> expiry_timestamp;
  };

  class news_data_payload: public element<>
  {
    HX2A_ELEMENT(news_data_payload, type_tag<"news_data_pld">, element,
		 ((venue_data, venue_tag),
		  (text, text_tag),
		  (expiry_timestamp, expiry_timestamp_tag)));
  public:

    news_data_payload(const news_r& n):
      venue_data(*this, make<venue_data_with_id_payload>(n->get_venue())),
      text(*this, n->get_text()),
      expiry_timestamp(*this, n->get_expiry_timestamp())
    {
    }

    own<venue_data_with_id_payload> venue_data;
    slot<string> text;
    slot<time_t> expiry_timestamp;
  };

} // End namespace events.

#endif
