//
// Copyright Metaspex - 2024
// mailto:admin@metaspex.com
//

#include "events/exception.hpp"
#include "events/payloads.hpp"

namespace events {

  using namespace hx2a;

  open_invite_data_payload:: open_invite_data_payload(const open_invite_r& oi):
    contacts(*this)
  {
    auto i = oi->contacts_cbegin();
    auto e = oi->contacts_cend();

    while (i != e){
      contact_p c = *i;
      HX2A_ASSERT(c);
      contact_r cr = *c;

      contacts.push_back(cr->copy());
      ++i;
    }
  }
  
} // End namespace events.
