//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#ifndef EVENTS_CLIENT_STATE_HPP
#define EVENTS_CLIENT_STATE_HPP

#include "hx2a/cursor.hpp"
#include "hx2a/for_each_doc.hpp"
#include "hx2a/own.hpp"
#include "hx2a/root.hpp"
#include "hx2a/slot.hpp"

#include "hx2a/components/user.hpp"

#include "messenger/client_state.hpp"

#include "events/misc.hpp"
#include "events/tags.hpp"

namespace events {

  class client_state;
  using client_state_p = ptr<client_state>;
  using client_state_r = rfr<client_state>;

  // The App token is stored in the Messenger client state App token.
  // The client state is retrieved from the user link, and then the token is found on the client
  // state document.
  class client_state: public root<>
  {
    HX2A_ROOT(client_state, type_tag<"client_state">, 1, root,
	      (_user, _messenger_client_state));

  public:

    client_state(const user_r& u, const string& token):
      _user(*this, u),
      _messenger_client_state(*this)
    {
      set_app_token(token);
    }

    user_r get_user() const {
      HX2A_ASSERT(_user);
      return *_user;
    }

    messenger::client_state_p get_messenger_client_state() const { return _messenger_client_state; }
    
    void set_messenger_client_state(const messenger::client_state_r& mcs){
      HX2A_ASSERT(!_messenger_client_state);
      _messenger_client_state = mcs;
    }
    
    // No set user function.

    string get_app_token() const {
      messenger::client_state_p mcs = _messenger_client_state;

      if (!mcs){
	return {};
      }

      return (*mcs)->get_app_token();
    }

    // Cleaning up zombie client state documents (e.g., if a user has logged in without properly logged out).
    static void clean_up(const db::connector& cn, const user_r& u, const string& token){
      // By batches of 100. Unlikely it's more than one.
      cursor c = cursor_on_key<client_state>(cn->get_index(index_by_token), {.key = {token}, .limit = 100});
      
      for_each_doc(c, [&u, &token](const client_state_r& cs){
	HX2A_LOG(trace) << "While creating a client state for user " << u->get_id() << ", found leftover client state for user " << cs->get_user()->get_id() << " and token \"" << token << "\". Removing it.";
	cs->unpublish();
      });
    }

    // Indexes.

    constexpr static tag_t index_by_user = config_name<"cs_u">;
    
    constexpr static tag_t index_by_token = config_name<"cs_t">;
    
  private:

    void set_app_token(const string& token){
      messenger::client_state_p mcs = _messenger_client_state;

      if (!mcs){
	messenger::client_state_r mcsr = make<messenger::client_state>();
	mcsr->set_app_token(token);
	_messenger_client_state = mcsr;
      }
      else{
	(*mcs)->set_app_token(token);
      }
    }

    link<user, "u"> _user;
    own<messenger::client_state, "m"> _messenger_client_state;
    
  };
    
} // End namespace events.

#endif

