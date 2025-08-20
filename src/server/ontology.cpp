//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#include "hx2a/cursor.hpp"
#include "hx2a/mail.hpp"
#include "hx2a/time.hpp"
#include "hx2a/format.hpp"

#include "hx2a/http/expo.hpp"

#include "events/client_state.hpp"
#include "events/exception.hpp"
#include "events/ontology.hpp"

namespace events {

  using namespace hx2a;

  double get_min_app_version(){
    // static as a cache.
    static double v = config::get_number_or(min_app_version_name,
					    default_min_app_version);
    return v;
  }
  
  size_t get_events_search_limit(){
    // static as a cache.
    static size_t v = config::get_number_or(events_search_limit_name,
					    default_events_search_limit);
    return v;
  }

  size_t get_venues_search_limit(){
    // static as a cache.
    static size_t v = config::get_number_or(venues_search_limit_name,
					    default_venues_search_limit);
    return v;
  }
  
  size_t get_contacts_in_open_invite_limit(){
    // static as a cache.
    static size_t v = config::get_number_or(contacts_in_open_invite_limit_name,
					    default_contacts_in_open_invite_limit);
    return v;
  }
  
  invite_p event::get_invite(const db::connector& cn, const user_r& u) const {
    // Check for unicity, attempt to get 2 rows.
    cursor c = cursor_on_key<invite>(cn->get_index(invite::index_by_event_and_guest), {.key = {get_id(), u->get_id()}, .limit = 2});
    c.read_next();
    const auto& rs = c.get_rows();
    const size_t rows_count = rs.size();
    
    // No more than one invite per user. In optimized mode this check is removed because it can fail (unlikely concurrent creations of invites).
    HX2A_ASSERT(rows_count <= 1);

    return rows_count ? rs.front().get_doc() : invite_p{};
  }

  booking_p event::get_booking(const db::connector& cn, const user_r& u) const {
    // Check for unicity, attempt to get 2 rows.
    cursor c = cursor_on_key<booking>(cn->get_index(booking::index_by_event_and_guest), {.key = {get_id(), u->get_id()}, .limit = 2});
    c.read_next();
    const auto& rs = c.get_rows();
    const size_t rows_count = rs.size();
    
    // No more than one booking per user. In optimized mode this check is removed because it can fail (unlikely concurrent creations of bookings).
    HX2A_ASSERT(rows_count <= 1);

    return rows_count ? rs.front().get_doc() : booking_p{};
  }

  event_p event::get_from_conversation_id(const db::connector& cn, const doc_id& conversation_id){
    cursor c = cursor_on_key<event>(cn->get_index(event::index_by_conversation), {.key = {conversation_id}, .limit = 1});
    c.read_next();
    const auto& rs = c.get_rows();
    
    return !rs.empty() ? rs.front().get_doc() : event_p{};
  }

  void invite::send_email() const {
    HX2A_ASSERT(_event);
    event_r e = *_event;
    
    ostringstream subject;
    subject << "You are invited ";
    string event_name = e->get_name();

    if (!event_name.empty()){
      subject << "to " << e->get_name() << ' ';
    }

    subject << "on " << events_brand_name << '!';
    user_r organizer = e->get_organizer();
    HX2A_ASSERT(_host);
    user_r host = *_host;
    HX2A_ASSERT(_guest);
    user_r guest = *_guest;
    ostringstream body;
    // Inserting \r\n instead of plain \n because some mail servers want both:
    /*
      Your message contains invalid characters (bare line feed characters) which the email servers atyahoo.com don't support.
      How to Fix It
      It appears that your email program added invalid characters (bare line feed characters) into your message when you sent it. Try using a different email program to send your message. If the problem continues send this non-delivery report (NDR) message to your email admin for assistance.	
    */
    body <<
      "Hello " << guest->get_user_id() << ",\r\n\r\n"
      "You have been invited by "
      ;
    string inviter;

    if (!host->is_root_user()){
      if (host != e->get_organizer()){
	inviter = host->get_informal_name();
      }
      else{
	inviter = organizer->get_informal_name();
      }
    }

    HX2A_ASSERT(!inviter.empty());
    body << inviter << ". To see the rest of the details, please visit the User invite section in our app for more information.\r\n\r\n"
      "Best regards,\r\n"
      "Team " << events_brand_name << "\r\n"
      ;
    
    mail m(
	   _guest->get_email(),
	   "", // Replaced automatically from data in configuration file when empty.
	   "", // Idem.
	   string(subject.str()),
	   string(body.str())
	   );
    m.send();
  }
  
  booking_r open_invite::accept(const db::connector& c, const user_r& guest, string display_name, string note){
    HX2A_ASSERT(_event);
    HX2A_ASSERT(_host);

    auto i = find_contact(guest->get_email());

    if (i == _contacts.cend()){
      throw invite_does_not_exist();
    }
    
    if (_event->is_bookable()){
      booking_r b = make<booking>(c, *_event, *_host, guest, display_name, note);
      // Removing the email address.
      _contacts.erase(i);
      return b;
    }

    throw event_is_not_bookable();
  }

  void open_invite::decline(const user_r& guest){
    auto i = find_contact(guest->get_email());

    if (i == _contacts.cend()){
      throw invite_does_not_exist();
    }
    
    _contacts.erase(i);
  }

  booking_r invite::accept(const db::connector& c, string display_name, string note){
    HX2A_ASSERT(_event);
    HX2A_ASSERT(_host);
    HX2A_ASSERT(_guest);

    if (_event->is_bookable()){
      booking_r b = make<booking>(c, *_event, *_host, *_guest, display_name, note);
      // Committing seppuku.
      unpublish();
      return b;
    }

    throw event_is_not_bookable();
  }

  void invite::notify_devices() const {
    // Retrieving all the client states for the invited user. Sending an Expo message to all the current
    // tokens.
    db::connector cn{client_state_dbname};
    // By batches of 100. Unlikely it's more than one.
    cursor c = cursor_on_key<client_state>(cn->get_index(client_state::index_by_user), {.key = {_guest->get_id()}, .limit = 100});
    doc_id did = _guest->get_id();
    
    for_each_doc(c, [this, &did](const client_state_r& cs){
      const string& app_token = cs->get_app_token();
      HX2A_LOG(trace) << "Found client state " << cs->get_id() << " for user " << did << " containing token \"" << app_token << "\".";

      if (app_token.size()){
	HX2A_ASSERT(_event);
	event_r e = *_event;
	user_r organizer = e->get_organizer();
	HX2A_ASSERT(_host);
	user_r host = *_host;
	string inviter;
	
	if (!host->is_root_user()){
	  if (host != e->get_organizer()){
	    inviter = host->get_informal_name();
	  }
	  else{
	    inviter = organizer->get_informal_name();
	  }
	}
	
	expo::notifier n(app_token, format("You have been invited by ", inviter), {} /* no body */);
	n();
      }
    });
  }

} // End namespace events.
