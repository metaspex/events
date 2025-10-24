//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#include "hx2a/server.hpp"
#include "hx2a/service.hpp"
#include "hx2a/session_info.hpp"
#include "hx2a/login_checker_prologue.hpp"
#include "hx2a/login_checker_session_prologue.hpp"
#include "hx2a/root_checker_prologue.hpp"
#include "hx2a/paginated_services.hpp"
#include "hx2a/json_leading_value_remover.hpp"
#include "hx2a/db/connector.hpp"
#include "hx2a/build_key.hpp"
#include "hx2a/debug_mode.hpp"
#include "hx2a/user_doc_id_adder.hpp"
#include "hx2a/user_email_adder.hpp"
#include "hx2a/projector.hpp"

#include "hx2a/payloads/app_token_payload.hpp"

// Embedding all Foundation Ontology services.
#include "hx2a/services/foundation.hpp"

#include "messenger/exception.hpp"

#include "events/client_state.hpp"
#include "events/exception.hpp"
#include "events/ontology.hpp"
#include "events/payloads.hpp"
#include "events/kdtree.hpp"

namespace events {

  inline client_state_p get_client_state(const session_info& si){
    doc_id session_id = si.get_session_id();
    
    if (session_id.is_null()){
      // Never assigned.
      HX2A_LOG(trace) << "No Events client state found.";
      return {};
    }

    db::connector cn{client_state_dbname};
    return client_state::get(cn, session_id).or_throw<messenger::client_state_does_not_exist>();    
  }

  inline client_state_r set_client_state(session_info& si, const user_r& u, const string& token){
    doc_id session_id = si.get_session_id();
    
    if (session_id.is_null()){
      db::connector cn{client_state_dbname};
      // Cleaning up zombie client states (e.g., if a user has logged in without properly logged out).
      client_state::clean_up(cn, u, token);
      client_state_r cs = make<client_state>(cn, u, token);
      doc_id session_id = cs->get_id();
      HX2A_LOG(trace) << "Created a Events client state document, its id is " << session_id;
      si.set_session_id(session_id);
      return cs;
    }

    throw client_state_already_exists();
  }

} // End namespace events.

// For Messenger perusal.
namespace messenger {

  // Client state manipulation for the Messenger. We have only the Messenger client state, we use the
  // document type it offers.
  // The Messenger gives its database as first argument in case it is useful. In our case it is not.
  
  void set_client_state(const db::connector&, session_info& si, const client_state_r& cs){
    // The assignment of the App token should have created a Events client state.
    events::client_state_r vcs = events::get_client_state(si).or_throw<client_state_does_not_exist>();
    vcs->set_messenger_client_state(cs);
  }

  client_state_p get_client_state(const db::connector&, const session_info& si){
    events::client_state_p cs = events::get_client_state(si);

    if (!cs){
      return {};
    }

    return (*cs)->get_messenger_client_state();
  }
  
} // End namespace messenger.

namespace events {

  // Reusable security checker functors. Reusability ensures that security is consistent and without hole.
  // Root is fine in all the checks below.

  namespace
  {
    struct owner_checker
    {
      // It'll allow the root user.
      void operator()(const venue_r& v, const user_r& u) const {
	if (v->get_owner() != u && !u->is_root_user()){
	  throw unauthorized();
	}
      }
    };

    struct organizer_checker
    {
      // It'll allow the root user.
      void operator()(const event_r& e, const user_r& u) const {
	if (e->get_organizer() != u && !u->is_root_user()){
	  throw unauthorized();
	}
      }
    };

    // Call this only if you do not need the booking subsequently.
    struct guest_checker
    {
      void operator()(const db::connector& cn, const event_r& e, const user_r& u) const {
	if (e->get_booking(cn, u) == nullptr){
	  throw unauthorized();
	}
      }
    };
    
    struct organizer_or_guest_checker
    {
      void operator()(const booking_r& b, const user_r& u) const {
	if (b->get_guest() != u && b->get_event()->get_organizer() != u && !u->is_root_user()){
	  throw unauthorized();
	}
      }
    };
    
    struct organizer_host_or_guest_checker
    {
      void operator()(const invite_r& i, const user_r& u) const {
	if (i->get_host() != u && i->get_guest() != u && i->get_event()->get_organizer() != u && !u->is_root_user()){
	  throw unauthorized();
	}
      }
      
      void operator()(const booking_r& b, const user_r& u) const {
	if (b->get_host() != u && b->get_guest() != u && b->get_event()->get_organizer() != u && !u->is_root_user()){
	  throw unauthorized();
	}
      }
    };
    
    struct organizer_or_venue_owner_checker
    {
      void operator()(const event_r& e, const user_r& u) const {
	if (e->get_organizer() != u && e->get_venue()->get_owner() != u && !u->is_root_user()){
	  throw unauthorized();
	}
      }
    };
    
    struct privacy_checker
    {
      // Only the owner or the root user are allowed if the venue is private.
      // Finding whether the user has an invite or a booking for an event at the venue is a very expensive
      // calculation, we would have to find another way if we want to remain frugal.
      void operator()(const venue_r& v, const user_r& u) const {
	if (
	    v->is_private() &&
	    v->get_owner() != u &&
	    !u->is_root_user()
	    ){
	  throw unauthorized();
	}
      }
      
      void operator()(const venue_r& v, const user_p& u) const {
	if (!u){
	  if (v->is_private()){
	    throw unauthorized();
	  }

	  return;
	}
	
	operator()(v, *u);
      }
      
      // Only the organizer, the root user, an invited user or a booked user are allowed if the event is private.
      // We might want to restrict access only for invites or bookings for upcoming events only.
      void operator()(const db::connector& cn, const event_r& e, const user_r& u) const {
	if (
	    e->is_private() &&
	    e->get_organizer() != u &&
	    !u->is_root_user() &&
	    !e->get_invite(cn, u) && // No invite.
	    !e->get_booking(cn, u) // No booking.
	    ){
	  throw unauthorized();
	}
      }
      
      void operator()(const db::connector& cn, const event_r& e, const user_p& u) const {
	if (!u){
	  if (e->is_private()){
	    throw unauthorized();
	  }

	  return;
	}

	operator()(cn, e, *u);
      }
    };

  } // namespace

  // Reusable utilities for paginated services.

  struct event_doc_id_adder
  {
    row_key_t operator()(const row_key_t& t, const rfr<event_id_payload>& query) const {
      return build_key(query->event_id, t);
    }
  };

  // Must add the query and check that the user is the organizer of the event.
  struct organizer_checker_prologue: login_checker_prologue
  {
    organizer_checker_prologue(const user_p& u, const rfr<event_id_payload>& query, const db::connector& c):
      login_checker_prologue(u)
    {
      // Looking for the event and checking that it exists and the user calling the service is the organizer.
      event_r e = event::get(c, query->event_id).or_throw<event_does_not_exist>();
      organizer_checker()(e, user);
    }
  };

  // Session/App token handling. These functions handle the client state.

  // To be called after _login.
  // We could remove the login Web service and create one that bundles the code below with login.
  auto _start_session = service<srv_tag<"start_session">>
    ([](const login_checker_session_prologue& prologue, const rfr<app_token_payload>& query){
      const string& token = query->token;
      HX2A_ASSERT(token.size());
      client_state_r cs = set_client_state(prologue.session, prologue.user, token);
    });

  // To be called before _logout.
  // This removes the client state document altogether.
  // We could remove the logout Web service and create one that bundles the code below with logout.
  auto _stop_session = service<srv_tag<"stop_session">>
    ([](const login_checker_session_prologue& prologue){
      client_state_p cs = get_client_state(prologue.session);
      HX2A_ASSERT(cs);
      
      // Never know.
      if (cs){
	(*cs)->unpublish();
      }
    });

  // Venue claim services.
  auto _venue_claim = service<srv_tag<"venue_claim">>
    ([](const login_checker_prologue& prologue, const rfr<venue_id_payload>& query){
      db::connector cn{dbname};
      venue_r v = venue::get(cn, query->venue_id).or_throw<venue_does_not_exist>();
      return make<venue_claim_id_payload>(v->claim(cn, prologue.user));
    });

  auto _venue_claim_accept = service<srv_tag<"venue_claim_accept">, root_checker_prologue>
    ([](const rfr<venue_claim_id_payload>& query){
      db::connector cn{dbname};
      venue_claim_r v = venue_claim::get(cn, query->venue_claim_id).or_throw<venue_claim_does_not_exist>();
      v->accept();
    });

  auto _venue_claim_reject = service<srv_tag<"venue_claim_reject">, root_checker_prologue>
    ([](const rfr<venue_claim_id_payload>& query){
      db::connector cn{dbname};
      venue_claim_r v = venue_claim::get(cn, query->venue_claim_id).or_throw<venue_claim_does_not_exist>();
      v->reject();
    });

  // Paginated services listing venue claims by decreasing creation timestamp.

  paginated_services<
    srv_tag<"venue_claims">,
    venue_claim,
    projector<venue_claim_data_payload>,
    root_checker_prologue
  >
  _venue_claims(config::get_id(dbname), venue_claim::index_by_creation_timestamp);

  // Venue services.

  // Only root can create all kinds of venues. Regular users can only create private venues.
  // We could add a check that a regular user does not create more private venues than a predefined limit.
  auto _venue_create = service<srv_tag<"venue_create">>
    ([](const login_checker_prologue& prologue, const rfr<venue_create_payload>& query){
      db::connector cn{dbname};
      
      query->validate();
      position_p pos = query->pos;
      address_p addr = query->addr;
      // The validation above should have checked that.
      HX2A_ASSERT(pos);
      HX2A_ASSERT(addr);
      bool privacy = query->is_private;

      if (!privacy && !prologue.user->is_root_user()){
	throw unauthorized();
      }

      // Need to cut the address, it is already owned by the payload. Cannot own it twice.
      (*addr)->cut();
      // Need to cut the position, it is already owned by the payload. Cannot own it twice.
      (*pos)->cut();
      // The venue cache will be updated by the cache reading the database.
      venue_r v = make<venue>(cn, prologue.user, query->name, privacy, query->category, query->category_description, *pos, *addr, query->capacity, query->description, query->event_confirmation_required, query->rating);
      
      for (const auto& i: query->images){
	v->push_image_back(i.get());
      }
      
      return make<venue_id_payload>(v);
    });

  // Anybody can get a public venue.
  // Only the owner and the root user can get it if it is private.
  // Allowing somebody to get venue details by search if they have an invite or a booking for that venue is extravagantly expensive. So we have
  // specific services for that, giving the invite or the booking.
  auto _venue_get = service<srv_tag<"venue_get">>
    ([](const user_p& u, const rfr<venue_id_payload>& query){
      db::connector cn{dbname};
      venue_r v = venue::get(cn, query->venue_id).or_throw<venue_does_not_exist>();
      privacy_checker()(v, u);
      return make<venue_data_payload>(v);
    });

  auto _venue_get_from_invite = service<srv_tag<"venue_get_from_invite">>
    ([](const user_p& u, const rfr<invite_id_payload>& query){
      db::connector cn{dbname};
      invite_r i = invite::get(cn, query->invite_id).or_throw<invite_does_not_exist>();

      if (i->get_guest() != u){
	throw unauthorized();
      }
      
      return make<venue_data_payload>(i->get_event()->get_venue());
    });

  auto _venue_get_from_booking = service<srv_tag<"venue_get_from_booking">>
    ([](const user_p& u, const rfr<booking_id_payload>& query){
      db::connector cn{dbname};
      booking_r b = booking::get(cn, query->booking_id).or_throw<booking_does_not_exist>();

      if (b->get_guest() != u){
	throw unauthorized();
      }
      
      return make<venue_data_payload>(b->get_event()->get_venue());
    });

  // Only the owner or the root user can update a venue.
  auto _venue_update = service<srv_tag<"venue_update">>
    ([](const login_checker_prologue& prologue, const rfr<venue_update_payload>& query){
      db::connector cn{dbname};
      // If successful we're sure the address is present.
      query->validate();
      venue_r v = venue::get(cn, query->venue_id).or_throw<venue_does_not_exist>();
      owner_checker()(v, prologue.user);
      v->update(query->name, query->category, query->category_description, query->addr->copy(), query->capacity, query->description, query->event_confirmation_required, query->rating);
    });

  // Only the owner or the root user can update a venue.
  auto _venue_update_images = service<srv_tag<"venue_update_images">>
    ([](const login_checker_prologue& prologue, const rfr<venue_update_images_payload>& query){
      db::connector cn{dbname};
      venue_r v = venue::get(cn, query->venue_id).or_throw<venue_does_not_exist>();
      owner_checker()(v, prologue.user);
      v->update(query);
    });

  auto _venue_transfer = service<srv_tag<"venue_transfer">>
    ([](const login_checker_prologue& prologue, const rfr<venue_transfer_payload>& query){
      db::connector cn{dbname};
      venue_r v = venue::get(cn, query->venue_id).or_throw<venue_does_not_exist>();
      owner_checker()(v, prologue.user);
      db::connector dc(db::directory_database);
      user_r new_owner = user::get(dc, query->new_owner_id).or_throw<user_does_not_exist>();
      v->transfer(new_owner);
    });

  auto _venue_remove = service<srv_tag<"venue_remove">>
    ([](const login_checker_prologue& prologue, const rfr<venue_id_payload>& query){
      db::connector cn{dbname};
      venue_r v = venue::get(cn, query->venue_id).or_throw<venue_does_not_exist>();
      owner_checker()(v, prologue.user);
      v->unpublish();
    });

  using venues_vector = std::vector<venue_p>;
  using venues_vector_iterator = venues_vector::iterator;

  static inline void fill_venue_search_reply(const venue_search_reply_r& sr, venues_vector_iterator& i, venues_vector_iterator e){
    while (i != e){
      sr->push_venue_data_back(make<venue_search_data_payload>(**i));
      ++i;
    }
  }
  
  // Finding all venues in an area.
  // If the function returns an empty reply (JSON object {}) it means that the user must zoom in. There are too many documents.
  // If the function finds nothing, the JSON array will be empty. This allows to distinguish the two cases.
  // An empty array of categories means that the client wishes to grab them all.
  // The root user will see all venues, including the private ones.
  // Regular users can see all public venues and all their own private ones.
  // A possible extension is to have a kdtree of invites/bookings so that the invited/booked users can do a  search and see
  // the private venues with events they have invites/bookings for.
  auto _venue_search = service<srv_tag<"venue_search">>
    ([](const user_p& u, const rfr<venue_search_query>& query) -> venue_search_reply_p {
      db::connector cn{dbname};
      cached_venues_type& cvc = get_cached_venues(cn);
      HX2A_LOG(trace) << "The venues cache contains " << cvc.size() << " venues.";
      // No connector yet, we look in the venues kdtree.
      // We add one so that if we find more than the requested amount, we return nothing so that the user has to zoom in.
      size_t vsl = get_venues_search_limit() + 1;
      venues_vector v(vsl);
      venues_vector_iterator i = v.begin();
      // Putting the intervals aside in case we reuse them for erasure.
      interval<latitude_t> li = query->get_latitude_interval();
      interval<longitude_t> Li = query->get_longitude_interval();
      // An empty array of categories means that the client wishes to grab them all.
      bool all_categories = query->categories.empty();

      // The root user sees everything.
      if (u && (*u)->is_root_user()){
	HX2A_LOG(trace) << "The search is performed by the root user, which has more prerogatives.";
	
	if (all_categories){
	  HX2A_LOG(trace) << "Searching for all categories.";
	
	  // We can do everything in one search in the kdtree.
	  auto e = cvc.search(
			      i,
			      vsl,
			      {undefined}, // Any owner doc id.
			      li, // Latitude.
			      Li, // Longitude.
			      {undefined}, // Any privacy.
			      {undefined} // Any category.
			      );
	  
	  if (size_t(e - i) == vsl){
	    // It means zoom in!
	    HX2A_LOG(trace) << "Too many venues found, please zoom in.";
	    return {};
	  }
	  
	  HX2A_LOG(trace) << "Found " << e - i << " venues.";
	  // Now we can connect to the database and get the documents (if any).
	  venue_search_reply_r sr = make<venue_search_reply>();
	  fill_venue_search_reply(sr, i, e);
	  return sr;
	}

	// A custom list of categories. Let's loop.
	venue_search_reply_r sr = make<venue_search_reply>();
	
	for (const auto& cat: query->categories){
	  auto e = cvc.search(
			      i,
			      vsl,
			      {undefined}, // Any owner doc id.
			      li, // Latitude.
			      Li, // Longitude.
			      {undefined}, // Any privacy.
			      {cat} // Category.
			      );
	  vsl -= size_t(e - i);

	  if (!vsl){
	    // It means zoom in!
	    HX2A_LOG(trace) << "Too many venues found, please zoom in.";
	    return {};
	  }
	  
	  HX2A_LOG(trace) << "Found " << e - i << " venues for category " << cat << '.';
	  fill_venue_search_reply(sr, i, e);
	  HX2A_ASSERT(i == e);
	}
	
	return sr;
      }

      HX2A_LOG(trace) << "The search is not performed by the root user.";
      
      // The user is not root. They have access only to public venues and their own private ones.
      if (all_categories){
	HX2A_LOG(trace) << "Searching for all categories.";
	
	// Let's start with public venues.
	auto e = cvc.search(
			    i,
			    vsl,
			    {undefined}, // Any owner doc id.
			    li, // Latitude.
			    Li, // Longitude.
			    {false}, // Grabbing the public venues.
			    {undefined} // Any category.
			    );
	vsl -= size_t(e - i);
	
	if (!vsl){
	  // It means zoom in!
	  HX2A_LOG(trace) << "Too many venues found, please zoom in.";
	  return {};
	}

	HX2A_LOG(trace) << "Found " << e - i << " public venues.";
	// Now we can connect to the database and get the documents (if any).
	venue_search_reply_r sr = make<venue_search_reply>();
	fill_venue_search_reply(sr, i, e);

	if (u){
	  // Let's add the user's private venues.
	  e = cvc.search(
			 i,
			 vsl,
			 {(*u)->get_id()}, // The user's owner doc id only.
			 li, // Latitude.
			 Li, // Longitude.
			 {true}, // Grabbing the private venues.
			 {undefined} // Any category.
			 );
	  vsl -= size_t(e - i);
	  
	  if (!vsl){
	    // It means zoom in!
	    HX2A_LOG(trace) << "Too many venues found, please zoom in.";
	    return {};
	  }
	  
	  HX2A_LOG(trace) << "Found " << e - i << " private venues.";
	  fill_venue_search_reply(sr, i, e);
	  HX2A_ASSERT(i == e);
	}
	
	return sr;
      }

      // A custom list of categories. 
      venue_search_reply_r sr = make<venue_search_reply>();
      
      for (const auto& cat: query->categories){
	// Let's start with public venues.
	auto e = cvc.search(
			    i,
			    vsl,
			    {undefined}, // Any owner doc id.
			    li, // Latitude.
			    Li, // Longitude.
			    {false}, // Grabbing the public venues.
			    {cat} // Category.
			    );
	vsl -= size_t(e - i);
	
	if (!vsl){
	  // It means zoom in!
	  HX2A_LOG(trace) << "Too many venues found, please zoom in.";
	  return {};
	}

	HX2A_LOG(trace) << "Found " << e - i << " public venues for category " << cat << '.';
	fill_venue_search_reply(sr, i, e);
	HX2A_ASSERT(i == e);

	if (u){
	  // Let's add the user's private venues.
	  e = cvc.search(
			 i,
			 vsl,
			 {(*u)->get_id()}, // The user's owner doc id.
			 li, // Latitude.
			 Li, // Longitude.
			 {true}, // Grabbing the private venues.
			 {cat} // Category.
			 );
	  vsl -= size_t(e - i);
	  
	  if (!vsl){
	    // It means zoom in!
	    HX2A_LOG(trace) << "Too many venues found, please zoom in.";
	    return {};
	  }
	  
	  HX2A_LOG(trace) << "Found " << e - i << " private venues for category " << cat << '.';
	  fill_venue_search_reply(sr, i, e);
	  HX2A_ASSERT(i == e);
	}
      }
      
      return sr;
    });

  // Paginated services listing venues.

  paginated_services<
    srv_tag<"venues_per_owner">,
    venue,
    projector<venue_data_payload>,
    login_checker_prologue,
    void,
    user_doc_id_adder,
    json_leading_value_remover
  >
  _venues_per_owner(config::get_id(dbname), venue::index_by_owner_and_name);

  // Events services.
  
  // Anybody logged in can create an event.
  auto _event_create = service<srv_tag<"event_create">>
    ([](const login_checker_prologue& prologue, const rfr<event_create_payload>& query){
      db::connector cn{dbname};
      venue_r v = venue::get(cn, query->venue_id).or_throw<venue_does_not_exist>();
      query->validate();
      // The event cache will be updated by the cache reading the database.
      event_r e = make<event>(cn, prologue.user, query->name, query->is_private, query->category, query->category_description, v, query->capacity, query->start, query->duration, query->bookings_notice_time, query->organizer_display_name);

      for (const auto& i: query->images){
	e->push_image_back(i.get());
      }
      
      return make<event_id_payload>(e);
    });

  // Anybody can get a public event.
  // Only the organizer, the root user, an invited user or a booked user can get it if it is private.
  auto _event_get = service<srv_tag<"event_get">>
    ([](const user_p& u, const rfr<event_id_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      privacy_checker()(cn, e, u);
      return make<event_data_payload>(e);
    });

  // Only the organizer and the root user can ask for an event to be confirmed by the venue owner.
  // Confirmation is only possible when the event is in unconfirmed state.
  auto _event_confirmation_request = service<srv_tag<"event_confirmation_request">>
    ([](const login_checker_prologue& prologue, const rfr<event_id_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      organizer_checker()(e, prologue.user);
      e->request_confirmation();
      // Should send a notification to the venue owner here.
    });

  // An event is to be confirmed by the venue owner or the root user.
  auto _event_confirm = service<srv_tag<"event_confirm">>
    ([](const login_checker_prologue& prologue, const rfr<event_id_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      owner_checker()(e->get_venue(), prologue.user);
      e->confirm();
      // Should send a notification to the organizer here.
    });

  // An event is to be rejected by the venue owner or the root user.
  auto _event_reject = service<srv_tag<"event_reject">>
    ([](const login_checker_prologue& prologue, const rfr<event_id_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      owner_checker()(e->get_venue(), prologue.user);
      e->reject();
      // Should send a notification to the organizer here.
    });

  // This can only be called by the organizer. The data are richer and encompass booking data.
  auto _event_get_for_organizer = service<srv_tag<"event_get_for_organizer">>
    ([](const login_checker_prologue& prologue, const rfr<event_id_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      organizer_checker()(e, prologue.user);
      return make<event_data_for_organizer_payload>(e);
    });

  // Obtaining event data from a Messenger conversation id.
  auto _event_get_from_conv = service<srv_tag<"event_get_from_conv">>
    ([](const login_checker_prologue& prologue, const rfr<messenger::conversation_id_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get_from_conversation_id(cn, query->_conversation_id).or_throw<event_does_not_exist>();
      privacy_checker()(cn, e, prologue.user);
      return make<event_data_with_id_payload>(e);
    });

  // Obtaining booking data from a Messenger conversation id.
  auto _booking_get_from_conv = service<srv_tag<"booking_get_from_conv">>
    ([](const login_checker_prologue& prologue, const rfr<messenger::conversation_id_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get_from_conversation_id(cn, query->_conversation_id).or_throw<event_does_not_exist>();
      privacy_checker()(cn, e, prologue.user);
      booking_r b = e->get_booking(cn, prologue.user).or_throw<booking_does_not_exist>();
      return make<booking_data_with_id_payload>(b);
    });

  // Only the organizer, the venue owner or the root user can cancel an event.
  // The conversation does not survive the event cancellation.
  // The service cannot therefore notify all the participants by message in the conversation.
  auto _event_cancel = service<srv_tag<"event_cancel">>
    ([](const login_checker_prologue& prologue, const rfr<event_cancel_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      organizer_or_venue_owner_checker()(e, prologue.user);
      e->cancel(query->reason);
    });

  // Only the organizer or the root user can update an event.
  auto _event_update = service<srv_tag<"event_update">>
    ([](const login_checker_prologue& prologue, const rfr<event_update_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      organizer_checker()(e, prologue.user);
      // The update payload sets capacity to 0 if unspecified.
      e->update(query->name, query->category, query->category_description, query->capacity, query->start, query->duration, query->bookings_notice_time);
    });

  // Only a participant can report. A participant can report multiple times.
  auto _event_report = service<srv_tag<"event_report">>
    ([](const login_checker_prologue& prologue, const rfr<event_id_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      guest_checker()(cn, e, prologue.user);
      e->report();
    });

  // Only the organizer or the root user can update an event.
  auto _event_update_images = service<srv_tag<"event_update_images">>
    ([](const login_checker_prologue& prologue, const rfr<event_update_images_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      organizer_checker()(e, prologue.user);
      e->update(query);
    });

  // Only the organizer or the root user can change the venue of an event.
  auto _event_change_venue = service<srv_tag<"event_change_venue">>
    ([](const login_checker_prologue& prologue, const rfr<event_change_venue_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      organizer_checker()(e, prologue.user);
      venue_r v = venue::get(cn, query->venue_id).or_throw<venue_does_not_exist>();
      e->set_venue(v);
      // An email could be sent to the venue owner and guests and invited people could be notified.
    });

  using events_vector = std::vector<event_p>;
  using events_vector_iterator = events_vector::iterator;

  static inline void fill_event_search_reply(const event_search_reply_r& sr, events_vector_iterator& i, events_vector_iterator e){
    while (i != e){
      event_r ce = **i;

      if (ce->is_bookable()){
	sr->push_event_data_back(make<event_search_data_payload>(ce));
      }
      
      ++i;
    }
  }
  
  // Finding all events in an area and a period.
  // If the function returns an empty reply (JSON object {}) it means that the user must zoom in. There are too many documents.
  // If the function finds nothing, the JSON array will be empty. This allows to distinguish the two cases.
  // A possible extension is to have a kdtree of invites/bookings so that the invited/booked users can do a  search and see
  // the private events they have invites/bookings for.
  auto _event_search = service<srv_tag<"event_search">>
    ([](const user_p& u, const rfr<event_search_query>& query) -> event_search_reply_p {
      db::connector cn{dbname};
      
      cached_events_type& cec = get_cached_events(cn);
      HX2A_LOG(trace) << "The events cache contains " << cec.size() << " events.";
      // No connector yet, we look in the events kdtree.
      // We add one so that if we find more than the requested amount, we return nothing so that the user has to zoom in.
      size_t vsl = get_events_search_limit() + 1;
      events_vector v(vsl);
      auto i = v.begin();
      area_r ar = query->the_area.or_throw<position_missing>();
      
      // Putting the intervals aside in case we reuse them for erasure.
      interval<latitude_t> li = ar->get_latitude_interval();
      interval<longitude_t> Li = ar->get_longitude_interval();
      // A little bit more complicated for the start period.
      interval<time_t> ti(undefined);

      if (period_p per = query->the_period){
	ti = (*per)->get_interval();
      }

      // An empty array of categories means that the client wishes to grab them all.
      bool all_categories = query->categories.empty();
      
      // The root user sees everything.
      if (u && (*u)->is_root_user()){
	HX2A_LOG(trace) << "The search is performed by the root user, which has more prerogatives.";
	
	if (all_categories){
	  HX2A_LOG(trace) << "Searching for all categories.";
	
	  auto e = cec.search(
			      i,
			      vsl,
			      {undefined}, // Any organizer doc id.
			      li, // Latitude.
			      Li, // Longitude.
			      {undefined}, // Any privacy.
			      {undefined}, // Any category.
			      ti // Start
			      );

	  // We haven't yet checked if the events are bookable, we implement a pessimistic heuristic.
	  // To be fully accurate we would need to acquire all the events, check if they are bookable
	  // and then make the test.
	  // This would require saving the events acquired to fill up the reply with that.
	  if (size_t(e - i) == vsl){
	    // It means zoom in!
	    HX2A_LOG(trace) << "Too many events found, please zoom in.";
	    return {};
	  }
	  
	  HX2A_LOG(trace) << "Found " << e - i << " events.";
	  // Now we can connect to the database and get the documents (if any).
	  event_search_reply_r sr = make<event_search_reply>();
	  // It'll filter out the events that are not bookable.
	  fill_event_search_reply(sr, i, e);
	  return sr;
	}

	// A custom list of categories. Let's loop.
	event_search_reply_r sr = make<event_search_reply>();
	
	for (const auto& cat: query->categories){
	  auto e = cec.search(
			      i,
			      vsl,
			      {undefined}, // Any organizer doc id.
			      li, // Latitude.
			      Li, // Longitude.
			      {undefined}, // Any privacy.
			      {cat}, // Category.
			      ti // Start
			      );
	  vsl -= size_t(e - i);
	  
	  // We haven't yet checked if the events are bookable, we implement a pessimistic heuristic.
	  if (!vsl){
	    // It means zoom in!
	    HX2A_LOG(trace) << "Too many events found, please zoom in.";
	    return {};
	  }
	  
	  HX2A_LOG(trace) << "Found " << e - i << " events for category " << cat << '.';
	  // It'll filter out the events that are not bookable.
	  fill_event_search_reply(sr, i, e);
	  HX2A_ASSERT(i == e);
	}
	
	return sr;
      }

      HX2A_LOG(trace) << "The search is not performed by the root user.";
      
      // The user is not root. They have access only to public events and their own private ones.
      if (all_categories){
	HX2A_LOG(trace) << "Searching for all categories.";
	
	// Let's start with public events.
	auto e = cec.search(
			    i,
			    vsl,
			    {undefined}, // Any organizer doc id.
			    li, // Latitude.
			    Li, // Longitude.
			    {false}, // Grabbing the public events.
			    {undefined}, // Any category.
			    ti // Start
			    );
	vsl -= size_t(e - i);
	
	// We haven't yet checked if the events are bookable, we implement a pessimistic heuristic.
	if (!vsl){
	  // It means zoom in!
	  HX2A_LOG(trace) << "Too many events found, please zoom in.";
	  return {};
	}

	HX2A_LOG(trace) << "Found " << e - i << " public event(s).";
	event_search_reply_r sr = make<event_search_reply>();
	// It'll filter out the events that are not bookable.
	fill_event_search_reply(sr, i, e);

	if (u){
	  // Let's add the user's private events.
	  e = cec.search(
			 i,
			 vsl,
			 {(*u)->get_id()}, // The user's organizer doc id only.
			 li, // Latitude.
			 Li, // Longitude.
			 {true}, // Grabbing the private events.
			 {undefined}, // Any category.
			 ti // Start
			 );
	  vsl -= size_t(e - i);
	  
	  // We haven't yet checked if the events are bookable, we implement a pessimistic heuristic.
	  if (!vsl){
	    // It means zoom in!
	    HX2A_LOG(trace) << "Too many events found, please zoom in.";
	    return {};
	  }
	  
	  HX2A_LOG(trace) << "Found " << e - i << " private events.";
	  // It'll filter out the events that are not bookable.
	  fill_event_search_reply(sr, i, e);
	  HX2A_ASSERT(i == e);
	}
	
	return sr;
      }

      // A custom list of events.
      event_search_reply_r sr = make<event_search_reply>();
      
      for (const auto& cat: query->categories){
	// Let's start with public events.
	auto e = cec.search(
			    i,
			    vsl,
			    {undefined}, // Any organizer doc id.
			    li, // Latitude.
			    Li, // Longitude.
			    {false}, // Grabbing the public events.
			    {cat}, // Category.
			    ti // Start
			    );
	vsl -= size_t(e - i);
	
	// We haven't yet checked if the events are bookable, we implement a pessimistic heuristic.
	if (!vsl){
	  // It means zoom in!
	  HX2A_LOG(trace) << "Too many events found, please zoom in.";
	  return {};
	}
      
	HX2A_LOG(trace) << "Found " << e - i << " public events for category " << cat << '.';
	// It'll filter out the events that are not bookable.
	fill_event_search_reply(sr, i, e);
	HX2A_ASSERT(i == e);

	if (u){
	  // Let's add the user's private events.
	  e = cec.search(
			 i,
			 vsl,
			 {(*u)->get_id()}, // The user's organizer doc id only.
			 li, // Latitude.
			 Li, // Longitude.
			 {true}, // Grabbing the private events.
			 {cat}, // Category.
			 ti // Start
			 );
	  vsl -= size_t(e - i);
	  
	  // We haven't yet checked if the events are bookable, we implement a pessimistic heuristic.
	  if (!vsl){
	    // It means zoom in!
	    HX2A_LOG(trace) << "Too many events found, please zoom in.";
	    return {};
	  }
	  
	  HX2A_LOG(trace) << "Found " << e - i << " private events for category " << cat << '.';
	  // It'll filter out the events that are not bookable.
	  fill_event_search_reply(sr, i, e);
	  HX2A_ASSERT(i == e);
	}
      }
      
      return sr;
    });

  // Only the organizer of the event or a person with a booking or an invite can create an open invite.
  auto _open_invite_create = service<srv_tag<"open_invite_create">>
    ([](const login_checker_prologue& prologue, const rfr<open_invite_create_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();

      if (
	  !e->is_bookable() ||
	  (
	   (e->get_organizer() != prologue.user) && // The organizer is allowed to send invites.
	   (e->is_private() || (!e->get_invite(cn, prologue.user) && !e->get_booking(cn, prologue.user))) // People with an invite or a booking can send invite for a public event.
	   )
	  ){
	throw unauthorized();
      }

      // The user calling the service is the host.
      open_invite_r i = make<open_invite>(cn, e, prologue.user);
      return make<open_invite_id_payload>(i);
    });

  // Anybody even not logged in that possesses the document identifier of the open invite can consult the open invite.
  auto _open_invite_get = service<srv_tag<"open_invite_get">>
    ([](const rfr<invite_id_payload>& query){
      db::connector cn{dbname};
      open_invite_r i = open_invite::get(cn, query->invite_id).or_throw<invite_does_not_exist>();
      return make<invite_details_payload>(i);
    });

  // The service is fully open. It is protected by a limit in the number of email addresses that can be added.
  auto _open_invite_add_contact = service<srv_tag<"open_invite_add_contact">>
    ([](const user_p& u, const rfr<open_invite_add_contact_payload>& query){
      db::connector cn{dbname};

      // A registered user cannot add their contact.
      if (u){
	throw unauthorized();
      }

      string email = query->email;
      db::connector dc(db::directory_database);
      
      // Now the user might not be logged in, but might supply their email address. Let's check if the email address corresponds to an existing user.
      if (user::get_from_email(dc, email)){
	throw unauthorized();
      }
      
      open_invite::get(cn, query->invite_id).or_throw<invite_does_not_exist>()->add_contact(make<contact>(query->first_name, query->last_name, email));
    });

  // Only the organizer of the event or a person with a booking or an invite can create an invite.
  // Must check that an invite does not exist yet.
  auto _invite_create = service<srv_tag<"invite_create">>
    ([](const login_checker_prologue& prologue, const rfr<invite_create_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();

      if (
	  !e->is_bookable() ||
	  (
	   (e->get_organizer() != prologue.user) && // The organizer is allowed to send invites.
	   (e->is_private() || (!e->get_invite(cn, prologue.user) && !e->get_booking(cn, prologue.user))) // People with an invite or a booking can send invite for a public event.
	   )
	  ){
	throw unauthorized();
      }

      db::connector dc(db::directory_database);
      user_r g = user::get(dc, query->guest_id).or_throw<user_does_not_exist>();

      // Checking that no invite for the user exists yet.
      if (e->get_invite(cn, g)){
	throw invite_already_made();
      }
      
      // Checking that no booking for the user exists yet.
      if (e->get_booking(cn, g)){
	throw booking_already_made();
      }

      // The user calling the service is the host.
      invite_r i = make<invite>(cn, e, prologue.user, g);
      i->notify();
      return make<invite_id_payload>(i);
    });

  // Only the invited user, the organizer of the event or the root user can inspect an invite.
  auto _invite_get = service<srv_tag<"invite_get">>
    ([](const login_checker_prologue& prologue, const rfr<invite_id_payload>& query){
      db::connector cn{dbname};
      invite_r i = invite::get(cn, query->invite_id).or_throw<invite_does_not_exist>();
      organizer_host_or_guest_checker()(i, prologue.user);
      return make<invite_details_payload>(i);
    });

  // Only the invited user can accept.
  auto _open_invite_accept = service<srv_tag<"open_invite_accept">>
    ([](const login_checker_prologue& prologue, const rfr<invite_accept_payload>& query){
      db::connector cn{dbname};
      open_invite_r i = open_invite::get(cn, query->invite_id).or_throw<invite_does_not_exist>();
      // Creating the booking will increment the booking count of the event.
      booking_r b = i->accept(cn, prologue.user, query->display_name, query->note);
      return make<booking_id_payload>(b);
    });

  // Only the invited user can decline.
  auto _open_invite_decline = service<srv_tag<"open_invite_decline">>
    ([](const login_checker_prologue& prologue, const rfr<invite_id_payload>& query){
      db::connector cn{dbname};
      open_invite_r i = open_invite::get(cn, query->invite_id).or_throw<invite_does_not_exist>();
      i->decline(prologue.user);
    });

  // Only the invited user can accept.
  auto _invite_accept = service<srv_tag<"invite_accept">>
    ([](const login_checker_prologue& prologue, const rfr<invite_accept_payload>& query){
      db::connector cn{dbname};
      invite_r i = invite::get(cn, query->invite_id).or_throw<invite_does_not_exist>();

      if (i->get_guest() != prologue.user){
	throw unauthorized();
      }

      // Creating the booking will increment the booking count of the event.
      booking_r b = i->accept(cn, query->display_name, query->note);
      return make<booking_id_payload>(b);
    });

  // Only the invited user and the organizer can decline.
  // As this is the same service for the organizer to cancel an invite, we have just one. So when an organizer wants to cancel a not-yet-accepted
  // invite, he just has to call decline.
  auto _invite_decline = service<srv_tag<"invite_decline">>
    ([](const login_checker_prologue& prologue, const rfr<invite_id_payload>& query){
      db::connector cn{dbname};
      invite_r i = invite::get(cn, query->invite_id).or_throw<invite_does_not_exist>();

      if (i->get_guest() != prologue.user && i->get_event()->get_organizer() != prologue.user){
	throw unauthorized();
      }

      i->unpublish();
    });

  // Anybody logged in can book a public event.
  // Must check that a booking does not exist yet.
  // Even the event organizer must make a booking if they want to be a guest. That ensures that inventory is properly decremented.
  auto _book = service<srv_tag<"book">>
    ([](const login_checker_prologue& prologue, const rfr<book_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();

      if (e->is_private()){
	throw unauthorized();
      }

      // Checking that no booking for the user exists yet.
      if (booking_p b = e->get_booking(cn, prologue.user)){
	throw booking_already_made();
      }

      // If there is an invite, we can remove it.
      if (invite_p i = e->get_invite(cn, prologue.user)){
	return make<booking_id_payload>((*i)->accept(cn, query->display_name, query->note));
      }

      // It's a public event, the guest is their own host.
      // Creating the booking will increment the booking count of the event.
      booking_r b = make<booking>(cn, e, prologue.user, prologue.user, query->display_name, query->note);
      return make<booking_id_payload>(b);
    });

  // The document identifier of the booking can be used to generate a 2D barcode that can be scanned at event
  // check-in.
  auto _booking_get = service<srv_tag<"booking_get">>
    ([](const login_checker_prologue& prologue, const rfr<booking_id_payload>& query){
      db::connector cn{dbname};
      booking_r b = booking::get(cn, query->booking_id).or_throw<booking_does_not_exist>();
      organizer_host_or_guest_checker()(b, prologue.user);
      return make<booking_and_event_data_payload>(b);
    });

  // Only the user who performed the booking or the organizer of the event can cancel a booking.
  auto _booking_cancel = service<srv_tag<"booking_cancel">>
    ([](const login_checker_prologue& prologue, const rfr<booking_id_payload>& query){
      db::connector cn{dbname};
      booking_r b = booking::get(cn, query->booking_id).or_throw<booking_does_not_exist>();
      organizer_or_guest_checker()(b, prologue.user);
      b->cancel();
    });

  // Typically the GUI should display a 2D barcode for the doc id of the booking, and the check-in agent
  // should call this service, by scanning the 2D barcode and calling this service.
  // The check-in agent needs to supply the event id too to verify that the booking corresponds to the
  // event he is operating for.
  // Will do nothing if already checked in.
  auto _check_in = service<srv_tag<"check_in">>
    ([](const login_checker_prologue& prologue, const rfr<check_in_payload>& query){
      db::connector cn{dbname};
      event_r e = event::get(cn, query->event_id).or_throw<event_does_not_exist>();
      organizer_checker()(e, prologue.user);
      booking_r b = booking::get(cn, query->booking_id).or_throw<booking_does_not_exist>();

      // Checking that the booking corresponds to the event being checked-in.
      if (b->get_event() != e){
	throw booking_is_not_for_the_event();
      }

      if (!b->checked_in()){
	b->check_in();
      }
    });

  // Paginated services for a user to list all their open invites.

  paginated_services<
    srv_tag<"open_invites_per_user">,
    open_invite,
    projector<invite_details_payload>,
    login_checker_prologue,
    void,
    user_email_adder,
    json_leading_value_remover
  >
  _open_invites_per_user(config::get_id(dbname), open_invite::index_by_email);

  // Paginated services for a user to list all their invites.

  paginated_services<
    srv_tag<"invites_per_user">,
    invite,
    projector<invite_details_payload>,
    login_checker_prologue,
    void,
    user_doc_id_adder,
    json_leading_value_remover
  >
  _invites_per_user(config::get_id(dbname), invite::index_by_guest);

  // Paginated services for a user to list all their bookings.

  paginated_services<
    srv_tag<"bookings_per_user">,
    booking,
    projector<booking_and_event_data_payload>,
    login_checker_prologue,
    void,
    user_doc_id_adder,
    json_leading_value_remover
  >
  _bookings_per_user(config::get_id(dbname), booking::index_by_guest);

  // Paginated services for an event organizer.

  // To list all the open invites for a given event.

  paginated_services<
    srv_tag<"open_invites_per_event">,
    open_invite,
    projector<open_invite_data_payload>,
    organizer_checker_prologue,
    event_id_payload,
    event_doc_id_adder,
    json_leading_value_remover
  >
  _open_invites_per_event(config::get_id(dbname), open_invite::index_by_event);
  
  // To list all the invites for a given event.

  paginated_services<
    srv_tag<"invites_per_event">,
    invite,
    projector<invite_data_payload>,
    organizer_checker_prologue,
    event_id_payload,
    event_doc_id_adder,
    json_leading_value_remover
  >
  _invites_per_event(config::get_id(dbname), invite::index_by_event_and_guest);
  
  // To list all the bookings for a given event, per user.
   
  paginated_services<
    srv_tag<"bookings_per_event">,
    booking,
    projector<booking_data_payload>,
    organizer_checker_prologue,
    event_id_payload,
    event_doc_id_adder,
    json_leading_value_remover
  >
  _bookings_per_event(config::get_id(dbname), booking::index_by_event_and_guest);
  
  // To list all the bookings for a given event, per check-in timestamp. Null timestamps are not checked-in yet,
  // so not checked in bookings will come first.
   
  paginated_services<
    srv_tag<"bookings_per_event_cit">,
    booking,
    projector<booking_data_payload>,
    organizer_checker_prologue,
    event_id_payload,
    event_doc_id_adder,
    json_leading_value_remover
  >
  _bookings_per_event_cit(config::get_id(dbname), booking::index_by_event_and_checkin_timestamp);
  
  // To list all their events by start date.
  
  paginated_services<
    srv_tag<"events_per_organizer">,
    event,
    projector<event_data_payload>,
    login_checker_prologue,
    void,
    user_doc_id_adder,
    json_leading_value_remover
  >
  _events_per_organizer(config::get_id(dbname), event::index_by_organizer);
  
  // To list all the public events starting later than a given timestamp for a given venue.

  struct venue_doc_id_public_adder
  {
    template <typename Query>
    row_key_t operator()(const row_key_t& t, const rfr<Query>& query) const {
      HX2A_LOG(trace) << "Adding the doc id of the venue " << query->venue_id << " and the private indicator to false as the first element of the key in paginated services.";
      return build_key(
		       query->venue_id,
		       false, // Not private.
		       t
		       );
    }
  };
  
  paginated_services<
    srv_tag<"events_for_venue">,
    event,
    projector<smart_event_data_payload>,
    login_checker_prologue,
    venue_id_payload,
    venue_doc_id_public_adder, // Adds the venue id and false to the keys.
    json_leading_values_remover<2> // Removes the two leading values.
  >
  _events_for_venue(config::get_id(dbname), event::index_by_venue_privacy_and_start_timestamp);

  // Same, but for the venue owner.
  
  struct events_for_owned_venue_prologue: login_checker_prologue
  {
    template <typename Query>
    events_for_owned_venue_prologue(const user_p& u, const rfr<Query>& query, const db::connector& cn):
      login_checker_prologue(u) // The user must log in.
    {
      doc_id vid = query->venue_id;
      venue_r v = venue::get(cn, vid).or_throw<venue_does_not_exist>();

      if (v->get_owner() != user){
	throw unauthorized();
      }
    }
  };

  struct venue_doc_id_adder
  {
    template <typename Query>
    row_key_t operator()(const row_key_t& t, const rfr<Query>& query) const {
      return build_key(
		       query->venue_id,
		       t
		       );
    }
  };
  
  paginated_services<
    srv_tag<"events_for_owned_venue">,
    event,
    projector<event_data_for_venue_owner_payload>,
    events_for_owned_venue_prologue,
    venue_id_payload,
    venue_doc_id_adder,
    json_leading_value_remover // Removing only the leading fixed Venue doc id.
  >
  _events_for_owned_venue(config::get_id(dbname), event::index_by_venue_state_and_start_timestamp);

  // Paginated services showing the events for a given venue by decreasing report count.
  // Can be called by the venue owner or the root user.
  paginated_services<
    srv_tag<"events_for_owned_venue_report_count">,
    event,
    projector<event_data_for_venue_owner_payload>,
    events_for_owned_venue_prologue,
    venue_id_payload,
    venue_doc_id_adder,
    json_leading_value_remover // Removing only the leading fixed Venue doc id.
    >
  _events_for_owned_venue_report_count(config::get_id(dbname), event::index_by_venue_and_report_count);

  // Paginated services showing the events by decreasing report count (any venue).
  // For the root user only.
  paginated_services<
    srv_tag<"events_by_report_count">,
    event,
    projector<event_data_for_venue_owner_payload>,
    root_checker_prologue
  >
  _events_by_report_count(config::get_id(dbname), event::index_by_report_count);

  // News services.

  auto _news_post = service<srv_tag<"news_post">>
    ([](const login_checker_prologue& prologue, const rfr<news_create_payload>& query){
      db::connector cn{dbname};
      venue_r v = venue::get(cn, query->venue_id).or_throw<venue_does_not_exist>();
      owner_checker()(v, prologue.user);
      return make<news_id_payload>(make<news>(cn, v, query->text, query->expiry_timestamp));
    });

  auto _news_remove = service<srv_tag<"news_remove">>
    ([](const login_checker_prologue& prologue, const rfr<news_id_payload>& query){
      db::connector cn{dbname};
      news_r n = news::get(cn, query->news_id).or_throw<news_does_not_exist>();
      owner_checker()(n->get_venue(), prologue.user);
      n->unpublish();
    });

  // Paginated services listing news from the one expiring the soonest after in the future to the latest, following
  // the startkey supplied. Normal use is to send "startkey": [now] (replace now with the appropriate timestamp corresponding
  // to the moment you call the service.
  
  paginated_services<
    srv_tag<"newsfeed">,
    news,
    projector<news_data_payload>,
    login_checker_prologue
  >
  _newsfeed(config::get_id(dbname), news::index_by_expiry_timestamp);

  // Anybody can call it without login.
  auto _min_app_version = service<srv_tag<"min_app_version">>
    ([]{
      return make<min_app_version_payload>();
    });

} // End namespace events.
