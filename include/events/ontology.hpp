//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#ifndef EVENTS_ONTOLOGY_HPP
#define EVENTS_ONTOLOGY_HPP

#include "hx2a/root.hpp"
#include "hx2a/slot.hpp"
#include "hx2a/link.hpp"
#include "hx2a/weak_link.hpp"
#include "hx2a/email_address.hpp"

#include "hx2a/db/connector.hpp"

#include "hx2a/components/address.hpp"
#include "hx2a/components/position.hpp"
#include "hx2a/components/user.hpp"

#include "messenger/ontology.hpp"

#include "events/exception.hpp"
#include "events/misc.hpp"
#include "events/tags.hpp"

namespace events {

  using namespace hx2a;

  class venue;
  using venue_p = ptr<venue>;
  using venue_r = rfr<venue>;
  
  class venue_claim;
  using venue_claim_p = ptr<venue_claim>;
  using venue_claim_r = rfr<venue_claim>;
  
  class event;
  using event_p = ptr<event>;
  using event_r = rfr<event>;

  class contact;
  using contact_p = ptr<contact>;
  using contact_r = rfr<contact>;
  
  class open_invite;
  using open_invite_p = ptr<open_invite>;
  using open_invite_r = rfr<open_invite>;
  
  class invite;
  using invite_p = ptr<invite>;
  using invite_r = rfr<invite>;
  
  class booking;
  using booking_p = ptr<booking>;
  using booking_r = rfr<booking>;

  class news;
  using news_p = ptr<news>;
  using news_r = rfr<news>;

  // Strongly-typed capacity for safety.
  enum capacity_t: unsigned int {};
  // A null capacity means infinite capacity.
  const capacity_t infinite_capacity{0};
  const capacity_t uninitialized_capacity{capacity_t(-1)};
  inline bool is_infinite(capacity_t c){ return c == infinite_capacity; }
  inline bool is_finite(capacity_t c){ return !is_infinite(c); }
  inline bool is_uninitialized(capacity_t c){ return c == uninitialized_capacity; }

  // Needs to obey some convention that the GUI applies.
  using category_t = unsigned int;
  // When not supplied the category by default is 0.
  const category_t default_category = 0;

  class venue_claim: public root<>
  {
    HX2A_ROOT(venue_claim, type_tag<"venue_claim">, 1, root,
	      (_user, _venue));
  public:

    venue_claim(const user_r& u, const venue_r& v):
      _user(*this, u),
      _venue(*this, v)
    {
    }

    user_r get_user() const { return *_user; }

    venue_r get_venue() const { return *_venue; }

    void accept();

    void reject(){
      unpublish();
    }

    // Indexes.

    static constexpr tag_t index_by_creation_timestamp = config_name<"vc_c">;
    
  private:

    link<user, "u"> _user;
    link<venue, "v"> _venue;
  };
  
  class venue: public root<>
  {
    HX2A_ROOT(venue, type_tag<"venue">, 1, root,
	      (_owner, _name, _private, _category, _category_description, _position, _address,
	       _capacity, _description, _event_confirmation_required, _images, _rating));
  public:

    using images_type = slot_vector<string, "i">;
    using images_const_iterator = images_type::const_iterator;
    using images_iterator = images_type::iterator;
    using rating_t = double;
    static constexpr rating_t no_rating = 0;

    // A null capacity means unlimited.
    venue(
	  const user_r& owner,
	  string name,
	  bool privacy,
	  category_t cat,
	  string category_description,
	  const position_r& pos,
	  const address_r& addr,
	  capacity_t capacity,
	  string desc,
	  bool event_confirmation_required,
	  rating_t rating
	  ):
      _owner(*this, owner),
      _name(*this, name),
      _private(*this, privacy),
      _category(*this, cat),
      _category_description(*this, category_description),
      _position(*this, pos),
      _address(*this, addr),
      _capacity(*this, capacity),
      _description(*this, desc),
      _event_confirmation_required(*this, event_confirmation_required),
      _images(*this),
      _rating(*this, rating)
    {
    }

    user_r get_owner() const { return *_owner; }

    venue_claim_r claim(const db::connector& cn, const user_r& u){
      return make<venue_claim>(cn, u, *this);
    }

    void transfer(const user_r& new_owner){ _owner = new_owner; }

    const string& get_name() const { return _name; }

    bool is_private() const { return _private; }

    category_t get_category() const { return _category; }

    const string& get_category_description() const { return _category_description; }

    position_r get_position() const { return *_position; }

    address_r get_address() const { return *_address; }

    capacity_t get_capacity() const { return _capacity; }

    bool has_finite_capacity() const { return is_finite(_capacity); }

    bool has_infinite_capacity() const { return is_infinite(_capacity); }

    const string& get_description() const { return _description; }

    bool get_event_confirmation_required() const { return _event_confirmation_required; }

    const images_type& get_images() const { return _images; }

    void push_image_back(string image){
      _images.push_back(image);
    }

    rating_t get_rating() const { return _rating; }

    void set_rating(rating_t r){
      _rating = r;
    }

    // Beware when updating capacity, there might be events already organized. The caller takes care of that.
    // Privacy and position cannot be updated, as they are search criteria in the in-memory index.
    void update(string name, category_t cat, string category_description, const address_r& addr, capacity_t capacity, string desc, bool event_confirmation_required, rating_t rating){
      _name = name;
      _category = cat;
      _category_description = category_description;
      _address = addr;
      _capacity = capacity;
      _description = desc;
      _event_confirmation_required = event_confirmation_required;
      _rating = rating;
    }

    template <typename ImagesHolder>
    void update(const rfr<ImagesHolder>& ih){
      _images.clear();

      for (const auto& i: ih->images){
	push_image_back(i.get());
      }
    }

    // Indexes.

    static constexpr tag_t index_by_save_timestamp = config_name<"v_c">;

    static constexpr tag_t index_by_owner_and_name = config_name<"v_o_n">;

  private:

    // If the user is removed, all the corresponding venues are removed too.
    link<user, "o"> _owner;
    slot<string, "n"> _name;
    // A non bool might be more type-safe.
    slot<bool, "p"> _private;
    slot<category_t, "c"> _category;
    slot<string, "cd"> _category_description;
    // We do not need full HTML5 geolocation, only latitude and longitude.
    own<position, "g"> _position;
    own<address, "a"> _address;
    slot<capacity_t, "C"> _capacity;
    slot<string, "d"> _description;
    slot<bool, "ecr"> _event_confirmation_required;
    // Storing only the URLs.
    images_type _images;
    slot<rating_t, "r"> _rating;
  };

  // The organizer is not necessarily a guest. He'll need to book the event to be a guest.
  class event: public root<>
  {
    HX2A_ROOT(event, type_tag<"event">, 1, root,
	      (_organizer, _name, _private, _category, _category_description, _venue, _state, _state_change_timestamp, _conversation,
	       _capacity, _start, _duration, _end, _bookings_notice_time, _bookings_count, _images, _report_count));
  public:

    using images_type = slot_vector<string, "i">;
    using images_const_iterator = images_type::const_iterator;
    using images_iterator = images_type::iterator;
    using duration_t = time_t;
    static constexpr duration_t unspecified_duration = 0;
    static constexpr time_t unspecified_end = 0;

    // Values assigned so that saved documents are consistent across evolutions of the backend.
    // Also so that kdtrees and cursors can process intervals for easy selections.
    // Tests can be performed in a single instruction for simple use cases, such as checking if an event is bookable:
    // state less or equal to 1 allows a booking.
    enum state_t {
		  unconfirmed = 0,
		  confirmation_requested = 1,
		  confirmed = 2,
		  rejected = 3,
		  canceled = 4
    };

    // No overbooking allowed yet.
    // Capacity is an int so that negative values mean to inherit from the venue's capacity.
    event(
	  const db::connector& c,
	  const user_r& organizer,
	  string name,
	  bool privacy,
	  category_t cat,
	  string category_description,
	  const venue_r& ven,
	  capacity_t capacity,
	  time_t start,
	  duration_t duration = unspecified_duration,
	  // Bookings or cancellations not authorized after end - bookinga_notice_time. 0 means no constraint.
	  // We could have two different values, one for booking one for unbooking.
	  time_t bookings_notice_time = 0, 
	  string organizer_display_name = ""
	  ):
      _organizer(*this, organizer),
      _name(*this, name),
      _private(*this, privacy || ven->is_private()),
      _category(*this, cat),
      _category_description(*this, category_description),
      _venue(*this, ven),
      _state(*this, ven->get_event_confirmation_required() ? unconfirmed : confirmed),
      _state_change_timestamp(*this, 0),
      _conversation(*this, messenger::conversation::build(c, name, organizer, organizer_display_name)),
      _capacity(*this),
      _start(*this, start),
      _duration(*this, duration),
      // I/Os with 0 are faster.
      _end(*this, calculate_end(start, duration)),
      _bookings_notice_time(*this, bookings_notice_time),
      _bookings_count(*this, infinite_capacity),
      _images(*this),
      _report_count(*this)
    {
      capacity_t vc = ven->get_capacity();
      
      if (is_uninitialized(capacity)){
	_capacity = vc;
      }
      // We need to check that the capacity makes sense vs. the capacity of the venue.
      else if (is_finite(vc) && (is_infinite(capacity) || capacity > vc)){
	throw invalid_capacity();
      }
      else{
	_capacity = capacity;
      }
    }

    user_r get_organizer() const { return *_organizer; }

    const string& get_name() const { return _name; }

    void set_name(string n){
      _name = n;

      if (messenger::conversation_p conv = _conversation){
	(*conv)->set_name(n);
      }
    }

    bool is_private() const { return _private; }

    bool is_public() const { return !is_private(); }

    category_t get_category() const { return _category; }

    const string& get_category_description() const { return _category_description; }

    venue_r get_venue() const { return *_venue; }

    void set_venue(const venue_r& v){
      // Capacity check.
      capacity_t vc = _venue->get_capacity();

      if (is_finite(vc) && (capacity_t)_bookings_count > vc){
	throw insufficient_capacity();
      }

      _venue = v;

      if (_venue->get_event_confirmation_required()){
	// Up to the new venue owner to confirm or not.
	_state = unconfirmed;
      }
      else{
	// This is the state in which the event would have been, had it been organized at the new
	// venue in the first place.
	_state = confirmed;
      }
    }

    state_t get_state() const { return _state; }

    void set_state(state_t st){
      _state = st;
      _state_change_timestamp = time();
    }

    time_t get_state_change_timestamp() const { return _state_change_timestamp; }

    // Called by the event organizer.
    void request_confirmation(){
      switch (_state){
      case unconfirmed: 
	{
	  break;
	}
      case confirmation_requested:
      case confirmed:
	{
	  return;
	}
      case canceled:
	{
	  throw event_is_canceled();
	}
      case rejected:
	{
	  throw event_is_rejected();
	}
      }

      set_state(confirmation_requested);
    }

    // Called by the venue owner.
    void confirm(){
      switch (_state){
      case unconfirmed: // The venue owner is authorized to bypass the confirmation request and go straight to confirmation.
      case confirmation_requested:
      case rejected: // The venue owner can change his mind.
	{
	  break;
	}
      case confirmed:
	{
	  return;
	}
      case canceled:
	{
	  throw event_is_canceled();
	}
      }

      set_state(confirmed);
    }
    
    // Called by the venue owner.
    void reject(){
      switch (_state){
      case unconfirmed: // The venue owner is authorized to go straight to rejection.
      case confirmation_requested:
	{
	  break;
	}
      case rejected: 
	{
	  return;
	}
      case confirmed:
	{
	  throw event_is_confirmed();
	}
      case canceled:
	{
	  throw event_is_canceled();
	}
      }

      set_state(rejected);
    }
    
    messenger::conversation_p get_conversation() const { return _conversation; }

    string get_organizer_display_name() const {
      if (!_conversation){
	return {};
      }
      
      return _conversation->get_owner_display_name();
    }

    capacity_t get_capacity() const { return _capacity; }

    // Capacity is an int and not a capacity_t so that -1 can be supplied to indicate inheritance from venue's capacity.
    void set_capacity(capacity_t capacity){
      capacity_t vc = _venue->get_capacity();
      
      if (is_uninitialized(capacity)){
	// Inheriting from the venue's capacity has been requested.
	if (is_finite(vc) && _bookings_count > vc){
	  // There are already too many bookings for the venue's capacity.
	  throw invalid_capacity();
	}
	
	_capacity = vc;
      }
      else{
	// We need to check that the capacity makes sense vs. the capacity of the venue.
	if (
	    // Can't have more capacity than the venue.
	    (is_finite(vc) && (is_infinite(capacity) || capacity > vc)) ||
	    // Can't have less capacity than the number of bookings.
	    (is_finite(capacity) && (capacity_t)_bookings_count > capacity)
	    ){
	  throw invalid_capacity();
	}

	_capacity = capacity;
      }
    }

    // A null max capacity means unlimited capacity.
    bool has_unlimited_capacity() const { return is_infinite(_capacity); }

    // Once an event enters its "window" it stays within it, even when the current time
    // has exceeded its end. We should consider renaming.
    bool is_in_window() const {
      if (time_t bnt = _bookings_notice_time){
	if (time() > _start - bnt){
	  return true;
	}
      }
      else if (time() > _start){
	return true;
      }

      return false;
    }

    // This should probably be complemented by a check_bookable function which throws
    // very specific exceptions to clarify why the caller is not authorized. This being
    // said all these specific exceptions might have to be processed, putting some
    // weight on the client side code.
    bool is_bookable() const {
      return (
	      has_unlimited_capacity() ||
	      get_bookings_count() < get_capacity()
	      ) &&
	_state <= confirmed && // Trick using states number to make only one test.
	!is_in_window()
	;
    }

    time_t get_start() const { return _start; }

    duration_t get_duration() const { return _duration; }

    // Will return 0 in case the event is unlimited.
    time_t get_end() const { return _end; }

    bool is_unlimited() const { return !_end; }

    time_t get_bookings_notice_time() const { return _bookings_notice_time; }

    capacity_t get_bookings_count() const { return _bookings_count; }

    // Checks the notice time.
    void book(){
      if (!is_bookable()){
	throw event_is_not_bookable();
      }
      
      _bookings_count = capacity_t(_bookings_count + 1);
    }

    void unbook(){
      if (is_in_window()){
	throw too_late();
      }

      HX2A_ASSERT(_bookings_count);
      _bookings_count = capacity_t(_bookings_count - 1);
    }

    // Returns the invite for the specified user for the event. Returns null if not found.
    invite_p get_invite(const db::connector&, const user_r&) const;
    
    // Returns the booking for the specified user for the event. Returns null if not found.
    booking_p get_booking(const db::connector&, const user_r&) const;

    const images_type& get_images() const { return _images; }

    void push_image_back(string image){
      _images.push_back(image);
    }

    uint64_t get_report_count() const { return _report_count; }

    void report(){ _report_count = _report_count + 1; }

    // The argument is the reason. Not used yet. 
    // A message could be posted to each participant in a one to one conversation between the root user and each
    // user who participated in the canceled event.
    void cancel(const string&){
      switch (_state){
      case unconfirmed: 
      case confirmation_requested:
      case confirmed:
      case rejected:
	{
	  break;
	}
      case canceled:
	{
	  return;
	}
      }
      
      // The conversation must not linger. The event might be canceled because it violates platform policy.
      if (messenger::conversation_p conv = _conversation){
	(*conv)->unpublish();
      }

      set_state(canceled);
    }

    // Privacy cannot be updated, as it is a search criteria in the in-memory index.
    // Capacity is an int and not a capacity_t so that -1 can be supplied to indicate inheritance from venue's capacity.
    void update(string name, category_t cat, string category_description, capacity_t capacity, time_t start, duration_t duration, time_t bookings_notice_time){
      _name = name;
      _category = cat;
      _category_description = category_description;
      set_capacity(capacity); // Does the extra work related to inheriting the venue's capacity, if needed, plus checks.
      _start = start;
      _duration = duration;
      _end = calculate_end(start, duration);
      _bookings_notice_time = bookings_notice_time;
    }

    template <typename ImagesHolder>
    void update(const rfr<ImagesHolder>& ih){
      _images.clear();

      for (const auto& i: ih->images){
	push_image_back(i.get());
      }
    }

    position_r get_position() const {
      return _venue->get_position();
    }

    // Does not check if the conversation document identifier corresponds to a conversation. It returns the event corresponding
    // to the conversation, if there is such an event.
    static event_p get_from_conversation_id(const db::connector&, const doc_id&);

    // Indexes.

    static constexpr tag_t index_by_conversation = config_name<"e_conv">;

    static constexpr tag_t index_by_save_timestamp = config_name<"e_c">;

    static constexpr tag_t index_by_organizer = config_name<"e_o">;

    static constexpr tag_t index_by_report_count = config_name<"e_rc">;

    static constexpr tag_t index_by_venue_and_report_count = config_name<"e_v_rc">;

    static constexpr tag_t index_by_venue_privacy_and_start_timestamp = config_name<"e_v_p_s">;

    static constexpr tag_t index_by_venue_state_and_start_timestamp = config_name<"e_v_st_s">;

  private:

    static time_t calculate_end(time_t start, time_t duration){ return duration == unspecified_duration ? unspecified_end : start + duration; }
    
    link<user, "o"> _organizer;
    slot<string, "n"> _name;
    // A non bool might be more type-safe.
    slot<bool, "p"> _private;
    slot<category_t, "c"> _category;
    slot<string, "cd"> _category_description;
    link<venue, "v"> _venue;
    slot<state_t, "st"> _state;
    slot<time_t, "stt"> _state_change_timestamp;
    // We do not need to remove the event in case the conversation disappears. Conversely if the event is canceled, the conversation
    // is preserved, as it should.
    // The organizer of the event is the owner of the conversation. The participation of the owner of a conversation is on the conversation
    // document itself.
    weak_link<messenger::conversation, "conv"> _conversation;
    slot<capacity_t, "C"> _capacity;
    slot<time_t, "s"> _start;
    slot<duration_t, "d"> _duration;
    slot<time_t, "e"> _end;
    slot<time_t, "bnt"> _bookings_notice_time;
    // The number of bookings could be calculated using the index on all bookings. It would be costly. We denormalize.
    slot<capacity_t, "bc"> _bookings_count;
    // Storing only the URLs.
    images_type _images;
    slot<uint64_t, "rc"> _report_count;
  };

  // Used as well as a payload.
  class contact: public element<>
  {
    HX2A_ELEMENT(contact, type_tag<"contact">, element,
		 (_first_name, _last_name, _email));
  public:

    contact(const string& first_name, const string& last_name, const string& email):
      _first_name(*this, first_name),
      _last_name(*this, last_name),
      _email(*this, email)
    {
      // Caveat emptor.
      validate_email(email);
    }

    const string& get_first_name() const { return _first_name; }

    const string& get_last_name() const { return _last_name; }

    const string& get_email() const { return _email; }

    contact_r copy() const { return make<contact>(_first_name, _last_name, _email); }
    
  private:

    slot<string, "f"> _first_name;
    slot<string, "l"> _last_name;
    slot<string, "e"> _email;
  };
  
  // Invite without an identified guest. The invite can be shared with multiple guests.
  class open_invite: public root<>
  {
    HX2A_ROOT(open_invite, type_tag<"open_invite">, 1, root,
	      (_event, _host, _contacts));
  public:

    using contacts_type = own_vector<contact, "c">;
    
    open_invite(
		const event_r& e,
		const user_r& host
		):
      _event(*this, e),
      _host(*this, host),
      _contacts(*this)
    {
    }

    event_r get_event() const { return *_event; }

    user_r get_host() const { return *_host; }

    // Adds only if the email address is not yet present.
    // The first and last names can be empty.
    // Validates if the email looks like an email address.
    void add_contact(const contact_r& cont){
      if (_contacts.size() == get_contacts_in_open_invite_limit()){
	throw contacts_in_open_invite_limit_reached();
      }

      const string& email = cont->get_email();
      auto e = _contacts.cend();
      auto i = ::std::find_if(_contacts.cbegin(), e, [email](const auto& c){
						       HX2A_ASSERT(c);
						       return c->get_email() == email;
						   });

      if (i == e){
	_contacts.push_back(cont);
      }
      // Otherwise we silently return without any fuss.
    }

    contacts_type::const_iterator find_contact(const string& email) const {
      return ::std::find_if(_contacts.cbegin(), _contacts.cend(), [email](const auto& c){
	HX2A_ASSERT(c);
	return c->get_email() == email;
      });
    }

    contacts_type::const_iterator contacts_cbegin() const { return _contacts.cbegin(); }

    contacts_type::const_iterator contacts_cend() const { return _contacts.cend(); }
    
    // Returns the newly created booking. The email disappears.
    // Creating the booking will increment the booking count of the event.
    booking_r accept(const db::connector&, const user_r& guest, string display_name, string note);

    void decline(const user_r& guest);

    // Indexes.

    static constexpr tag_t index_by_email = config_name<"oi_ce">;

    static constexpr tag_t index_by_event = config_name<"oi_e">;

  private:
    
    // An open invite disappears if the event is removed.
    link<event, "e"> _event;
    // The host is the one creating the open invite. Note that it's different from the event organizer.
    // It's a strong link because we retract the open invite if the host is removed.
    link<user, "h"> _host;
    contacts_type _contacts;
  };

  // Especially useful for private events.
  class invite: public root<>
  {
    HX2A_ROOT(invite, type_tag<"invite">, 1, root,
	      (_event, _host, _guest));
  public:

    invite(
	   const event_r& e,
	   const user_r& host,
	   const user_r& guest
	   ):
      _event(*this, e),
      _host(*this, host),
      _guest(*this, guest)
    {
    }

    event_r get_event() const { return *_event; }

    user_r get_host() const { return *_host; }
    
    user_r get_guest() const { return *_guest; }

    void notify() const {
      send_email();
      notify_devices();
    }
    
    void send_email() const;

    void notify_devices() const;

    // Returns the newly created booking. The invite disappears.
    // Creating the booking will increment the booking count of the event.
    booking_r accept(const db::connector&, string display_name, string note);

    // Indexes.

    static constexpr tag_t index_by_event_and_guest = config_name<"i_e_g">;

    static constexpr tag_t index_by_guest = config_name<"i_g">;

  private:
    
    // An invite disappears if the event is removed.
    link<event, "e"> _event;
    // The host is the one creating the invite. Note that it's different from the event organizer.
    // It's a strong link because we retract the invite if the host is removed.
    link<user, "h"> _host;
    // An invite disappears when the guest user is removed.
    link<user, "g"> _guest;
  };

  // The document identifier of the booking can be used to generate a 2D barcode that can be scanned at event
  // check-in.
  // The "host" is the one who created the invite, or the one who created the booking. It is potentially
  // distinct from the event organizer.
  class booking: public root<>
  {
    HX2A_ROOT(booking, type_tag<"booking">, 1, root,
	      (_event, _host, _guest, _messenger_participation, _note, _check_in_timestamp));
  public:

    // Increments the booking count of the event.
    booking(
	    const db::connector& c,
	    const event_r& e,
	    const user_r& host,
	    const user_r& guest,
	    string display_name,
	    string note
	    ):
      _event(*this, e),
      _host(*this, host),
      _guest(*this, guest),
      _messenger_participation(*this),
      _note(*this, std::move(note)),
      _check_in_timestamp(*this, 0)
    {
      messenger::conversation_p conv = e->get_conversation();
	
      if (
	  conv &&
	  guest != e->get_organizer() // The organizer of the event does not need a participation, it already has one.
	  ){
	_messenger_participation = make<messenger::participation>(c, *conv, guest, display_name);
      }

      // Incrementing the booking count.
      e->book();
    }

    event_r get_event() const { return *_event; }

    // If the host is removed, we keep the invite, so it's a weak link, and we're not sure
    // we still have a host.
    user_p get_host () const { return _host; }
    
    user_r get_guest() const { return *_guest; }

    // It's a weak link, for performance reasons.
    messenger::participation_p get_messenger_participation() const { return _messenger_participation; }

    const string& get_note() const { return _note; }

    // Returns 0 if not yet checked in.
    time_t get_check_in_timestamp(){
      return _check_in_timestamp;
    }

    bool checked_in(){
      return _check_in_timestamp;
    }
    
    // Does not check if already checked in.
    void check_in(){
      if (get_event()->get_state() != event::confirmed){
	throw event_cannot_be_checked_in();
      }
      
      _check_in_timestamp = time();
    }

    void cancel(){
      // Giving back to the inventory.
      _event->unbook();
      
      // Removing the participation if the user is not the organizer of the event.
      // If he is, his participation stays.
      if (_guest != _event->get_organizer()){
	_messenger_participation->unpublish();
      }

      // Seppuku.
      unpublish();
    }

    // Indexes.

    static constexpr tag_t index_by_event_and_guest = config_name<"b_e_g">;

    static constexpr tag_t index_by_guest = config_name<"b_g">;

    static constexpr tag_t index_by_event_and_checkin_timestamp = config_name<"b_e_cit">;

  private:

    // Strong link. The booking is removed if the event is removed.
    link<event, "e"> _event;
    // The booking is retained even if the host (the user who invited the guest) is removed.
    weak_link<user, "h"> _host;
    link<user, "g"> _guest;
    // No need to load the participation when inspecting a booking.
    weak_link<messenger::participation, "mp"> _messenger_participation;
    slot<string, "n"> _note;
    // If used, 0 means not checked in.
    slot<time_t, "cit"> _check_in_timestamp;
  };

  class news: public root<>
  {
    HX2A_ROOT(news, type_tag<"news">, 1, root,
	      (_venue, _text, _expiry_timestamp));
  public:

    news(const venue_r& v, const string& text, time_t expiry_timestamp):
      _venue(*this, v),
      _text(*this, text),
      _expiry_timestamp(*this, expiry_timestamp)
    {
      check();
    }

    venue_r get_venue() const { return *_venue; }

    const string& get_text() const { return _text; }

    void set_text(const string& text){ _text = text; }

    time_t get_expiry_timestamp() const { return _expiry_timestamp; }

    void set_expiry_timestamp(time_t expiry_timestamp){
      _expiry_timestamp = expiry_timestamp;
      check();
    }

    // Indexes.

    static constexpr tag_t index_by_expiry_timestamp = config_name<"n_e">;

  private:

    void check() const {
      if (_text.get().empty()){
	throw news_text_is_empty();
      }
      
      if (time() >= _expiry_timestamp){
	throw news_expiry_in_the_past();
      }
    }
    
    link<venue, "v"> _venue;
    slot<string, "t"> _text;
    slot<time_t, "e"> _expiry_timestamp;
  };
  
  inline void venue_claim::accept(){
    _venue->transfer(*_user);
    // Remove oneself.
    unpublish();
  }

} // End namespace events.

#endif
